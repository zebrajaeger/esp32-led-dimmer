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

#pragma once

#include <Arduino.h>

#include <ArduinoJson.h>

#define CONFIG_FILE "/config.json"

#define JSON_DEVICENAME "deviceName"
#define JSON_TOPIC "topic"
#define JSON_MQTTSERVER "mqttServer"
#define JSON_USERNAME "userName"
#define JSON_PASSWORD "password"
#define JSON_ARTNET_UNIVERSE "artnetUniverse"

/**
 * Load and Save Configuration Data to/from flash.
 * 
 * This Data can only rarely written because the flash cells have a short amount of write cycle before they die.
 * 
 * In case of ESP32-WROOM-32 module, the Datasheet (https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf)
 * doesn't say which chip is soldered.
 * But i found pictures of the opened device in the internet and saw that one version uses 
 * a FM25Q32 (http://www.fmsh.com/AjaxFile/DownLoadFile.aspx?FilePath=/UpLoadFile/20150401/FM25Q32_ds_eng.pdf&fileExt=file)
 * wich has 100.000 program/erase cycles. What is much more than i expected.
 * Another version has a GD25Q32 (http://www.elm-tech.com/en/products/spi-flash-memory/gd25q32/gd25q32.pdf)
 * which also has 100.000 program/erase cycles.
 * Here (https://forum.makehackvoid.com/t/playing-with-the-esp-32/1144/21) is an excellent image with another version.
 * 
 * In short: All Datasheet i saw have 100.000 program/erase cycles.
 */
class Config {
 public:
  const String& getDeviceName() const { return deviceName_; };
  void setDeviceName(String value) { deviceName_ = value; };

  const String& getTopic() const { return topic_; };
  void setTopic(String value) { topic_ = value; };

  const String& getMqttServer() const { return mqttServer_; };
  void setMqttServer(String value) { mqttServer_ = value; };

  const String& getUsername() const { return username_; };
  void setUsername(String value) { username_ = value; };

  const String& getPassword() const { return password_; };
  void setPassword(String value) { password_ = value; };

  const uint16_t getArtnetUniverse() const { return artnetUniverse_; };
  void setArtnetUniverse(uint16_t value) { artnetUniverse_ = value; };

  bool fromJson(JsonDocument& doc);
  void toJson(JsonDocument& doc);
  void dump();
  bool load();
  void save();

 protected:
  // device stuff
  String deviceName_;

  // mqtt stuff
  String topic_;
  String mqttServer_;
  String username_;
  String password_;
  uint16_t artnetUniverse_;
};
