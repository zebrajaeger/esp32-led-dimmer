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
#include <Wifi.h>
#include <Wire.h>

#include "config/config.h"
#include "config/configserver.h"
#include "hardware/fram.h"
#include "hardware/pwm.h"
#include "hardware/statusled.h"
#include "net/artnet.h"
#include "net/mqtt.h"
#include "net/ota.h"
#include "net/reconnector.h"
#include "net/websocket.h"
#include "net/wifistate.h"
#include "state.h"
#include "statistic.h"
#include "util/jsonparser.h"
#include "util/logger.h"
#include "util/multitimer.h"
#include "util/utils.h"

//------------------------------------------------------------------------------
#define SOFTWARE_VERSION "1.0.1"

#define WIRE_SPEED 115200

#define I2C_SPEED 400000
#define I2C_SCL 22
#define I2C_SDA 23
//------------------------------------------------------------------------------

enum MODE { MODE_LAMP, MODE_E131, MODE_OTA };

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
StatusLed statusLed(2);
Artnet artNet;
Logger LOG("App");
Websocket websocketServer;
MODE mode = MODE_LAMP;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) { Serial.println("XXXX"); }

//------------------------------------------------------------------------------
void publishState(bool publishOnMqtt, bool publishOnSerial, bool pretty)
//------------------------------------------------------------------------------
{
  DynamicJsonDocument doc(1024);
  JsonParser::DeviceData dd;
  dd.name = config.getDeviceName();
  dd.ip = WiFi.localIP().toString();

  JsonParser::toJson(doc, state, dd);
  String json;
  JsonParser::doc2String(doc, json, pretty);
  if (publishOnMqtt) {
    mqtt.publish(config.getTopic() + "/status", json);
  }
  if (publishOnSerial) {
    Serial.println(json);
  }
}

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
    publishState(true, false, false);
    LOG.i("[APP] MQTT connected to '%s'", config.getMqttServer().c_str());
    LOG.i("[APP] Topics are:");
    LOG.i("         '%s/alive' for online status", (config.getTopic().c_str()));
    LOG.i("         '%s/status' for data status", (config.getTopic().c_str()));
    LOG.i("         '%s/set' for seting data", (config.getTopic().c_str()));
  } else {
    LOG.e("[APP] ERROR: Could not connect to MQTT server: '%s'", config.getMqttServer().c_str());
  }
  return result;
}

void scanI2C() {
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (uint8_t address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

//------------------------------------------------------------------------------
void setup()
//------------------------------------------------------------------------------
{
  // for(;;);

  // Debug out
  Serial.begin(WIRE_SPEED);

  // LED
  if (statusLed.begin()) {
    LOG.d("StatusLED started");
  } else {
    LOG.e("StatusLED failed");
  }
  statusLed.blink();

  // Boot msg
  LOG.i("+-----------------------+");
  LOG.i("|        Booting        |");
  LOG.i("+-----------------------+");

  // ID
  id = Utils::createId();
  LOG.i("ID: '%s'", id.c_str());

  // Config
  if (config.load()) {
    LOG.d("Config file loaded successfully");
  } else {
    LOG.w("Could not load config file from flash. Using default values");
    config.setDeviceName(id);
    config.setTopic(id);
    config.setArtnetUniverse(666);
  }

  // I2C Bus
  Wire.end();
  if (Wire.begin(I2C_SDA, I2C_SCL, I2C_SPEED)) {
    LOG.i("Wire initialized");
    scanI2C();
  } else {
    LOG.e("Could not initialize Wire");
  }

  // FRAM
  if (fram.begin()) {
    LOG.i("Found I2C FRAM");
    if (!fram.validateCRC()) {
      LOG.w("     FRAM CRC check failed. Initialize...");
      setFrequency(1500);
      for (uint8_t i = 0; i < 16; ++i) {
        setChannelValue(i, 2048);  // 50%
      }
      fram.setFrequency(1500);
      fram.recalculateCRC();
    } else {
      LOG.i("FRAM CRC ok");
      state.setFrequency(fram.getFrequency());
      for (uint8_t i = 0; i < 16; ++i) {
        state.setChannelValue(i, fram.getChannelValue(i));
      }
    }
  } else {
    LOG.e("I2C FRAM not identified ... check your connections?");
  }

  // PWM
  if (pwm.begin()) {
    LOG.i("Found I2C PWM(PCA9685)");
    uint16_t f = fram.getFrequency();
    LOG.i("PWM f: %dHz", f);
    pwm.setFrequency(f);
    for (uint8_t i = 0; i < 16; ++i) {
      uint16_t v = fram.getChannelValue(i);
      LOG.i("CH %d: %d", i, v);
      pwm.setChannelValue(i, v);
    }
  } else {
    LOG.e("ERROR I2C PWM(PCA9685) not found!");
  }

  // WIFI
  wifiState.onConnect([]() {
    LOG.i("WiFi connected, IP address: '%s'", WiFi.localIP().toString().c_str());
    reconnector.onWifiConnected();
    statusLed.on();
  });
  wifiState.onDisconnect([](bool wasConnected, uint8_t reason) {
    LOG.w("WiFi lost connection. Was connected: '%s'", wasConnected ? "true" : "false");
    // hack for WiFI connection, see https://github.com/espressif/arduino-esp32/issues/2501
    if (reason == 202) {  // AUTH FAILED
      Serial.println("Connection failed, REBOOT/SLEEP!");
      esp_sleep_enable_timer_wakeup(10);
      esp_deep_sleep_start();
      delay(100);
    }
    if (wasConnected) {
      reconnector.onWifiDisconnected();
    }
    statusLed.off();
  });

  LOG.i("WiFiState initialized");

  // ConfigServer
  configServer.onDeviceSet([](DeviceData data) {
    configServer.setDeviceData(data);
    id = data.deviceName;
    config.setDeviceName(data.deviceName);
    config.save();
    MDNS.end();
    if (MDNS.begin(id.c_str())) {
      LOG.i("mDNS Restartet with name: %s", id.c_str());
    }
    mqttConnect();
  });
  configServer.onMqttSet([](MqttData& data) {
    configServer.setMqttData(data);
    config.setTopic(data.topic);
    config.setMqttServer(data.server);
    config.setUsername(data.userName);
    config.setPassword(data.password);
    config.save();
    mqttConnect();
  });
  configServer.onArtnetSet([](ArtnetData& data) {
    configServer.setArtnetData(data);
    config.setArtnetUniverse(data.universe);
    config.save();
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
    LOG.i("Configserver startet @ http://%s", id.c_str());
  }

  // OTA Update
  if (ota.begin()) {
    LOG.i("OTA startet");
    ota.onStart([]() { statusLed.off(); });
    ota.onProgress([](double uploaded) { statusLed.blink(uploaded); });
    ota.onEnd([]() { statusLed.on(); });
  }

  // mDNS Server
  if (MDNS.begin(id.c_str())) {
    MDNS.addService("http", "tcp", 80);
    LOG.i("mDNS started with name: %s", id.c_str());
  }

  // Reconnector
  if (reconnector.begin()) {
    reconnector.tryToConnect([]() { return mqttConnect(); });
    reconnector.tryToDisconnect([]() { return mqtt.disconnect(); });
    reconnector.isConnected([]() { return mqtt.isConnected(); });
    LOG.i("Reconnector started");
  }

  // MQTT
  if (mqtt.begin()) {
    LOG.i("Mqtt client initialized");
    mqtt.onData([](JsonDocument& doc) {
      JsonParser::parseChannelData(
          doc, [](uint16_t frequency) { setFrequency(frequency); },
          [](uint8_t channel, uint16_t value) { setChannelValue(channel, value); });
      fram.recalculateCRC();
      // fram.dump();
      publishState(true, false, false);
    });
    reconnector.onMQTTConfigured();
    if (wifiState.isConnected()) {
      mqttConnect();
    }
  } else {
    LOG.e("ERROR: mqtt initialization failed");
  }

  // Statistic
  if (statistic.begin()) {
    LOG.i("Statistic started.");
  } else {
    LOG.e("ERROR: Statistic could not started.");
  }

  // Timer / Mqtt-Status, Serial-Status
  if (multiTimer.begin()) {
    LOG.i("Initialize Timer.");
    multiTimer.set("mqtt-status", 300000, []() { publishState(true, false, false); });
    multiTimer.set("serial-status", 60000, []() { publishState(false, true, true); });
  } else {
    LOG.e("Timer initializing failed.");
  }

  // Websocket
  LOG.i("Initialize Websocket server.");
  if (websocketServer.begin(81)) {
    websocketServer.onStatusRequest([](uint8_t clientId) {
      String json;
      String msgId = "";
      JsonParser::channelDataToJsonArray(json, msgId, state);
      websocketServer.send(clientId, json);
    });
    websocketServer.onData([](const uint8_t clientId, const String& json) {
      String msgId;
      uint16_t channelData[16];
      if (JsonParser::channelDataFromJsonArray(json, msgId, channelData)) {
        for (uint8_t i = 0; i < 16; ++i) {
          // set only if different (because writing to FRAM and PWM needs time)
          if (state.getChannelValue(i) != channelData[i]) {
            setChannelValue(i, channelData[i]);
          }
        }
      }
      String jsonTemp;
      JsonParser::channelDataResponse(jsonTemp, msgId);
      websocketServer.send(clientId, jsonTemp);
    });
    LOG.i("Websocket server initialized");
  }

  // e1.31
  LOG.i("Initialize e1.31 with universe: '%u'", config.getArtnetUniverse());
  if (artNet.begin(config.getArtnetUniverse())) {
    LOG.i("Artnet initialized");
    artNet.onData([](uint8_t* channelData, uint16_t length) {
      if (mode == MODE_LAMP) {
        LOG.i("MODE: LAMP -> E131");
        mode = MODE_E131;
        pwm.setFrequency(1500);
      }
      for (uint16_t i = 0; i < length && i < 16; ++i) {
        pwm.setChannelValueLog(i, channelData[i]);
      }
    });
    artNet.onTimeout([]() {
      if (mode == MODE_E131) {
        LOG.i("MODE: E131 -> LAMP");
        mode == MODE_LAMP;
        pwm.setFrequency(state.getFrequency());
        for (uint8_t i = 0; i < 16; ++i) {
          pwm.setChannelValue(i, state.getChannelValue(i));
        }
      }
    });
  } else {
    LOG.i("Failed to initialize Artnet");
  }

  // STATE output (only serial)
  publishState(false, true, true);

  // DONE
  LOG.i("Boot done, executing main loop.");
}

//------------------------------------------------------------------------------
void loop()
//------------------------------------------------------------------------------
{
  switch (mode) {
    case MODE_LAMP:
      ota.loop();
      if (ota.isUpdating()) {
        mode = MODE_OTA;
      } else {
        artNet.loop();
        mqtt.loop();
        configServer.loop();
        reconnector.loop();
        multiTimer.loop();
        websocketServer.loop();
        statistic.loop();
      }
      break;

    case MODE_E131:
      // on artnet paket, we switch to E131 Mode
      // on artnet timeout we switch back to LAMP modus
      artNet.loop();
      break;

    case MODE_OTA:
      ota.loop();
      if (!ota.isUpdating()) {
        mode = MODE_LAMP;
      }
      break;
  }
  // artNet.loop();
  // if(mode==LAMP){
  //   ota.loop();
  //   // speed up updates.
  //   if (!ota.isUpdating()) {
  //     mqtt.loop();
  //     configServer.loop();
  //     reconnector.loop();
  //     multiTimer.loop();
  //     websocketServer.loop();
  //   }
  // }
  //
  // statistic.loop();
}
