/*
 * This file is part of the ESP32-LED-Dimmer distribution (https://github.com/zebrajaeger/esp32-led-dimmer).
 * Copyright (c) 2019 Lars Brandt.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>

#include <ESPmDNS.h>

#include "config.h"
#include "configserver.h"
#include "fram.h"
#include "jsonparser.h"
#include "mqtt.h"
#include "multitimer.h"
#include "ota.h"
#include "pwm.h"
#include "reconnector.h"
#include "state.h"
#include "statistic.h"
#include "utils.h"
#include "wifistate.h"

//------------------------------------------------------------------------------
#define SOFTWARE_VERSION "1.0.1"

#define WIRE_SPEED 115200

#define I2C_SPEED 400000
#define I2C_SCL 22
#define I2C_SDA 23
//------------------------------------------------------------------------------
String id;
Config config;
FRAM fram;
PWM pwm;
WiFiState wifiState;
OTA ota;
Mqtt mqtt;
State state;
ConfigServer configServer;
Reconnector reconnector;
Statistic statistic;
MultiTimer multiTimer;

//------------------------------------------------------------------------------
void setFrequency(uint16_t frequency)
//------------------------------------------------------------------------------
{
  pwm.setFrequency(frequency);
  fram.setFrequency(frequency);
  state.setFrequency(frequency);
}

//------------------------------------------------------------------------------
void setChannelValue(uint8_t channel, uint16_t value)
//------------------------------------------------------------------------------
{
  pwm.setChannelValue(channel, value);
  fram.setChannelValue(channel, value);
  state.setChannelValue(channel, value);
}

//------------------------------------------------------------------------------
bool mqttConnect()
//------------------------------------------------------------------------------
{
  bool result = mqtt.connect(id, config.getTopic(), config.getMqttServer(), 1883, config.getUsername(), config.getPassword());
  if (result) {
    Serial.printf("[APP] MQTT connected to '%s'\n", config.getMqttServer().c_str());
    Serial.println("[APP] Topics are:");
    Serial.printf("         '%s/alive' for online status\n", (config.getTopic().c_str()));
    Serial.printf("         '%s/status' for data status\n", (config.getTopic().c_str()));
    Serial.printf("         '%s/set' for seting data\n", (config.getTopic().c_str()));

  } else {
    Serial.printf("[APP] ERROR: Could not connect to MQTT server: '%s'\n", config.getMqttServer().c_str());
  }
  return result;
}

//------------------------------------------------------------------------------
void publishState()
//------------------------------------------------------------------------------
{
  DynamicJsonDocument doc(1024);
  JsonParser::DeviceData dd;
  dd.name = config.getDeviceName();
  dd.ip = WiFi.localIP().toString();
  JsonParser::toJson(doc, state, dd);
  mqtt.publish(config.getTopic() + "/status", doc, false);
}

//------------------------------------------------------------------------------
void setup()
//------------------------------------------------------------------------------
{
  // Debug out
  Serial.begin(WIRE_SPEED);
  // delay(1000);
  Serial.println("+-----------------------+");
  Serial.println("|        Booting        |");
  Serial.println("+-----------------------+");

  // ID
  id = Utils::createId();
  Serial.println("[APP] ID: " + id);

  // Config
  if (config.load()) {
    Serial.println("[APP] Config file loaded successfully");
    config.dump();
  } else {
    Serial.println("[APP] Could not load config file from flash. Using default values");
    config.setDeviceName(id);
    config.setTopic(id);
  }

  // I2C Bus
  Wire.begin(I2C_SDA, I2C_SCL, I2C_SPEED);

  // FRAM
  if (fram.begin()) {
    Serial.println("[APP] Found I2C FRAM");
    if (!fram.validateCRC()) {
      Serial.println("[APP]     FRAM CRC check faild. Initialize...");
      setFrequency(1500);
      for (uint8_t i = 0; i < 16; ++i) {
        setChannelValue(i, 2048);  // 50%
      }
      fram.recalculateCRC();
    } else {
      Serial.println("[APP] FRAM CRC ok");
      state.setFrequency(fram.getFrequency());
      for (uint8_t i = 0; i < 16; ++i) {
        state.setChannelValue(i, fram.getChannelValue(i));
      }
    }
    fram.dump();
  } else {
    Serial.println("[APP] I2C FRAM not identified ... check your connections?\r\n");
  }

  // PWM
  if (pwm.begin()) {
    Serial.println("[APP] Found I2C PWM(PCA9685)");
    uint16_t f = fram.getFrequency();
    Serial.printf("[APP] PWM f: %dHz\n", f);
    pwm.setFrequency(f);
    for (uint8_t i = 0; i < 16; ++i) {
      uint16_t v = fram.getChannelValue(i);
      Serial.printf("[APP] CH %d: %d\n", i, v);
      pwm.setChannelValue(i, v);
    }
  } else {
    Serial.println("[APP] ERROR I2C PWM(PCA9685) not found!");
  }

  // WIFI
  wifiState.onConnect([]() {
    Serial.println("[APP] WiFi connected, IP address: " + WiFi.localIP().toString());
    reconnector.onWifiConnected();
  });
  wifiState.onDisconnect([](bool wasConnected) {
    Serial.printf("[APP] WiFi lost connection. Was connected: '%s'\n", wasConnected ? "true" : "false");
    if (wasConnected) {
      reconnector.onWifiDisconnected();
    }
  });
  Serial.println("[APP] WiFiState initialized");

  // ConfigServer
  configServer.onDeviceSet([](DeviceData data) {
    id = data.deviceName;
    config.setDeviceName(data.deviceName);
    config.save();
    MDNS.end();
    if (MDNS.begin(id.c_str())) {
      Serial.printf("[APP] mDNS Restartet with name: %s\n", id.c_str());
    }
    mqttConnect();
  });
  configServer.onMqttSet([](MqttData& data) {
    config.setTopic(data.topic);
    config.setMqttServer(data.server);
    config.setUsername(data.userName);
    config.setPassword(data.password);
    config.save();
    mqttConnect();
  });
  if (configServer.begin(String("LED-Dimmer ") + SOFTWARE_VERSION)) {
    DeviceData deviceData;
    deviceData.deviceName = config.getDeviceName();
    configServer.setDeviceData(deviceData);
    MqttData mqttData;
    mqttData.topic = config.getTopic();
    mqttData.server = config.getMqttServer();
    mqttData.userName = config.getUsername();
    mqttData.password = config.getPassword();
    configServer.setMqttData(mqttData);
    Serial.printf("[APP] Configserver startet @ http://%s\n", id.c_str());
  }

  // OTA Update
  if (ota.begin()) {
    Serial.println("[APP] OTA startet");
  }

  // mDNS Server
  if (MDNS.begin(id.c_str())) {
    MDNS.addService("http", "tcp", 80);
    Serial.printf("[APP] mDNS started with name: %s\n", id.c_str());
  }

  // Reconnector
  if (reconnector.begin()) {
    reconnector.tryToConnect([]() { return mqttConnect(); });
    reconnector.tryToDisconnect([]() { return mqtt.disconnect(); });
    reconnector.isConnected([]() { return mqtt.isConnected(); });
    Serial.println("[APP] reconnector started");
  }

  // MQTT
  if (mqtt.begin()) {
    Serial.println("[APP] mqtt initialized");
    mqtt.onData([](JsonDocument& doc) {
      JsonParser::parseChannelData(doc, [](uint16_t frequency) { setFrequency(frequency); },
                                   [](uint8_t channel, uint16_t value) { setChannelValue(channel, value); });
      fram.recalculateCRC();
      fram.dump();
      publishState();
    });
    reconnector.onMQTTConfigured();
    if (wifiState.isConnected()) {
      mqttConnect();
    }
  } else {
    Serial.println("[APP] ERROR: mqtt initialization failed");
  }

  // Statistic
  if (statistic.begin()) {
    Serial.println("[APP] Statistic started.");
  } else {
    Serial.println("[APP] ERROR: Statistic could not started.");
  }

  // timer
  if (multiTimer.begin()) {
    Serial.println("[APP] Initialize Timer.");
    multiTimer.set("mqtt-status", 300000, []() { publishState(); });
  } else {
    Serial.println("[APP] Timer initializing failed.");
  }

  // DONE
  Serial.println("[APP] Boot done, executing main loop.");
}

//------------------------------------------------------------------------------
void loop()
//------------------------------------------------------------------------------
{
  ota.handle();
  // speed up updates.
  if (!ota.isUpdating()) {
    mqtt.loop();
    configServer.loop();
    reconnector.loop();
    multiTimer.loop();
  }
  statistic.loop();
}
