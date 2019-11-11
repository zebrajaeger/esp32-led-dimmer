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

#include <WiFi.h>
#include <WiFiUdp.h>

#include <AutoConnect.h>
#include <ESPmDNS.h>
#include <FaBoPWM_PCA9685.h>
#include <WebServer.h>
#include <SimpleTimer.h>

#include "config.h"
#include "fm24c04.h"
#include "jsonparser.h"
#include "mqtt.h"
#include "ota.h"
#include "state.h"

#include "menu.h"

//------------------------------------------------------------------------------
#define SOFTWARE_VERSION "1.0.0"

#define I2C_SPEED 400000
#define I2C_SCL 22
#define I2C_SDA 23

#define WIRE_SPEED 115200

//------------------------------------------------------------------------------
WebServer server;
AutoConnect portal(server);
AutoConnectConfig autoConnectConfig;
FaBoPWM faboPWM;
FM24C04 fram;
MQTT mqtt;
OTA ota;
JsonParser jsonParser;
State state;
Config config;
SimpleTimer mqttReconnectionTimer();
int mqttReconnectionTimerHandle = 0;

//------------------------------------------------------------------------------
void setPortalDataFromConfig()
//------------------------------------------------------------------------------
{
  // device
  AutoConnectAux* device = portal.aux("/device");
  AutoConnectInput& deviceName = device->getElement<AutoConnectInput>("devicename");
  deviceName.value = config.getDeviceName();

  // mqtt
  AutoConnectAux* mqtt = portal.aux("/mqtt");
  AutoConnectInput& topic = mqtt->getElement<AutoConnectInput>("topic");
  topic.value = config.getTopic();

  AutoConnectInput& mqttServer = mqtt->getElement<AutoConnectInput>("mqtt-server");
  mqttServer.value = config.getMqttServer();

  AutoConnectInput& username = mqtt->getElement<AutoConnectInput>("username");
  username.value = config.getUsername();

  AutoConnectInput& password = mqtt->getElement<AutoConnectInput>("password");
  password.value = config.getPassword();
}

//------------------------------------------------------------------------------
void onDeviceSet()
//------------------------------------------------------------------------------
{
  config.setDeviceName(server.arg("devicename"));
  config.save();

  setPortalDataFromConfig();

  server.sendHeader("Location", "/device", true);
  server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop();
}

//------------------------------------------------------------------------------
void onMqttSet()
//------------------------------------------------------------------------------
{
  config.setTopic(server.arg("topic"));
  config.setMqttServer(server.arg("mqtt-server"));
  config.setUsername(server.arg("username"));
  config.setPassword(server.arg("password"));
  config.save();
  setPortalDataFromConfig();

  server.sendHeader("Location", "/mqtt", true);
  server.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop();
}

//------------------------------------------------------------------------------
void rootPage()
//------------------------------------------------------------------------------
{
  server.send(200, "text/html", siteRoot);
}

//------------------------------------------------------------------------------
void publishState()
//------------------------------------------------------------------------------
{
  StaticJsonDocument<1024> doc;
  jsonParser.createState(doc, state);

  char x[1024];
  serializeJson((JsonDocument)doc, x);
  mqtt.publish((config.getTopic() + "/state").c_str(), (const char*)&x);
}

//------------------------------------------------------------------------------
void setFrequency(uint16_t frequency)
//------------------------------------------------------------------------------
{
  if (frequency < 24) frequency = 24;
  if (frequency > 1526) frequency = 1526;

  faboPWM.set_hz(frequency);
  fram.write16(32, frequency);
  state.setFrequency(frequency);
}

//------------------------------------------------------------------------------
void setChannelValue(uint8_t channel, uint16_t value)
//------------------------------------------------------------------------------
{
  if (channel < 0) channel = 0;
  if (channel > 15) channel = 15;

  if (value < 0) value = 0;
  if (value > 4095) value = 4095;

  faboPWM.set_channel_value(channel, value);
  fram.write16(channel + channel, value);
  state.setChannelValue(channel, value);
}

//------------------------------------------------------------------------------
boolean onMqttReconnect()
//------------------------------------------------------------------------------
{
  if(!mqtt.isConnected()){
    if(WiFi.status() == WL_CONNECTED){
      Serial.println("try to connect to MQTT-Server.");
      mqtt.connect();
    }else{
      mqttReconnectionTimer.setTimeout(2000, onMqttReconnect);
    }
  }
}

//------------------------------------------------------------------------------
void setup()
//------------------------------------------------------------------------------
{
  // Debug out
  Serial.begin(WIRE_SPEED);
  Serial.println("-------- Booting --------");

  // WIFI
  WiFi.onEvent([&](WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch (event) {
      case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: " + WiFi.localIP().toString());
        mqtt.connect();
        break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        mqtt.disconnect();
        break;
      default:
        // only to prevent compiler warnings
        break;
    }
  });

  // ID
  char buffer[13];
  uint64_t chipId = ESP.getEfuseMac();
  sprintf(reinterpret_cast<char*>(&(buffer[0])), "%04x", (uint32_t)((chipId >> 32) & 0xffff));
  sprintf(reinterpret_cast<char*>(&(buffer[4])), "%08x", (uint32_t)chipId);
  buffer[12] = 0;  // TODO required?
  String id("ESP-");
  id += (char*)&buffer;
  Serial.println("ID: " + id);

// For debug/development only. Connects to wifi to keep OTA alive if configuration not works.
// create a file 'wificonnect.h' and put as content i.E.:
//   { WiFi.begin("<my_wifi_SSID>", "<my_wifi_password>"); }
#if __has_include("wificonnect.h")
#include "wificonnect.h"
#endif

  // I2C Bus
  Wire.begin(I2C_SDA, I2C_SCL, I2C_SPEED);

  // FRAM
  if (fram.begin(0x50)) {
    Serial.println("Found I2C FRAM");
    if (!fram.validate(0, 33, 34)) {
      Serial.println("    FRAM CRC check faild. Initialize...");
      for (uint8_t i = 0; i < 16; ++i) {
        setChannelValue(i, 2048);
      }
      setFrequency(1500);
      fram.refreshChecksum(0, 33, 34);
    }
  } else {
    Serial.println("I2C FRAM not identified ... check your connections?\r\n");
    Serial.println(
        "Will continue in case this processor doesn't support repeated "
        "start\r\n");
  }

  // PCA9685
  if (faboPWM.begin()) {
    Serial.println("PCA9685 found");
    faboPWM.init(0);
    uint16_t f = fram.read16(32);
    setFrequency(f);
    Serial.printf("    f: %u\n", f);
    for (uint8_t i = 0; i < 16; ++i) {
      uint16_t v = fram.read16(i + i);
      setChannelValue(i, v);
    }
  } else {
    Serial.println("PCA9685 _NOT_ found");
  }

  // Config
  if (config.load()) {
    Serial.println("Config file loaded successfully");
  } else {
    Serial.println("Could not load config file from flash. Using default values");
    config.setDeviceName(id);
    config.setTopic(id);
  }

  // MQTT
  mqtt.onConnected([](bool sessionPresesent) {
    Serial.println("MQTT Server connected");
    state.dump();
    mqtt.lastWill((config.getTopic() + "/connected").c_str(), "false");
    mqtt.publish((config.getTopic() + "/connected").c_str(), "true");
    mqtt.subscribeJson((config.getTopic() + "/set").c_str(), [](JsonDocument& doc) {
      jsonParser.parseChannelData(doc, [](uint16_t frequency) { setFrequency(frequency); },
                                  [](uint8_t channel, uint16_t value) { setChannelValue(channel, value); });
      fram.refreshChecksum(0, 33, 34);
      publishState();
    });
    publishState();
  });

  mqtt.onDisconnected([](AsyncMqttClientDisconnectReason reason) {
    Serial.printf("MQTT Server disconnected. reason: '%d'\n", reason);
    mqttReconnectionTimer.startDelayed();
  });

  if (!config.getMqttServer().isEmpty()) {
    const String& mqttserver = config.getMqttServer();
    Serial.printf("MQTT Server start @ \"%s\"\n", mqttserver.c_str());
    mqtt.init(mqttserver.c_str());
    Serial.println("MQTT Server initialized");
  } else {
    Serial.println("MQTT Server NOT started because no server location is set");
  }

  // Webserver
  server.on("/", rootPage);
  server.on("/device_set", onDeviceSet);
  server.on("/mqtt_set", onMqttSet);

  // Autoconfig
  autoConnectConfig.title = String("LED-Dimmer ") + SOFTWARE_VERSION + " / " + config.getDeviceName();
  autoConnectConfig.autoReconnect = true;
  portal.config(autoConnectConfig);
  if (portal.load(menuPageMqtt)) {
    Serial.println("AutoConnect config loaded successfully.");
  } else {
    Serial.println("ERROR: Autoconnect config could not be loaded.");
  }
  setPortalDataFromConfig();
  if (portal.begin()) {
    Serial.println("Webserver/Portal started");
  }

  // OTA Update
  ota.begin();
  Serial.println("OTA startet");

  // mDNS Server
  if (MDNS.begin(config.getDeviceName().c_str())) {
    Serial.printf("mDNS started with name: %s\n", config.getDeviceName().c_str());
  }
}

//------------------------------------------------------------------------------
void loop()
//------------------------------------------------------------------------------
{
  ota.handle();
  if (!ota.isUpdating()) {
    portal.handleClient();
  }
}
