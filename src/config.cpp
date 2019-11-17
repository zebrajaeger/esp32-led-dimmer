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

#include "config.h"

#include <SPIFFS.h>

//------------------------------------------------------------------------------
bool Config::fromJson(JsonDocument& doc)
//------------------------------------------------------------------------------
{
  bool complete = true;

  complete &= doc.containsKey(JSON_DEVICENAME);
  deviceName_ = doc[JSON_DEVICENAME].as<const char*>();

  complete &= doc.containsKey(JSON_TOPIC);
  topic_ = doc[JSON_TOPIC].as<const char*>();

  complete &= doc.containsKey(JSON_MQTTSERVER);
  mqttServer_ = doc[JSON_MQTTSERVER].as<const char*>();

  complete &= doc.containsKey(JSON_USERNAME);
  username_ = doc[JSON_USERNAME].as<const char*>();

  complete &= doc.containsKey(JSON_PASSWORD);
  password_ = doc[JSON_PASSWORD].as<const char*>();

  return complete;
}

//------------------------------------------------------------------------------
void Config::toJson(JsonDocument& doc)
//------------------------------------------------------------------------------
{
  doc[JSON_DEVICENAME] = deviceName_;
  doc[JSON_TOPIC] = topic_;
  doc[JSON_MQTTSERVER] = mqttServer_;
  doc[JSON_USERNAME] = username_;
  doc[JSON_PASSWORD] = password_;
}

//------------------------------------------------------------------------------
void Config::dump()
//------------------------------------------------------------------------------
{
  Serial.println("----- Config dump -----");
  DynamicJsonDocument doc(1024);
  toJson(doc);
  serializeJsonPretty(doc, Serial);
  Serial.println("----- /Config dump -----");
}

//------------------------------------------------------------------------------
bool Config::load()
//------------------------------------------------------------------------------
{
  if (!SPIFFS.begin(true)) {
    Serial.println("CONFIG.ERROR: could not mount SPIFFSn");
    return false;
  }

  bool result = false;
  DynamicJsonDocument doc(1024);
  if (!SPIFFS.exists(CONFIG_FILE)) {
    Serial.println("CONFIG.WARN: config file does not exist.");
    return false;
  }

  File configFile = SPIFFS.open(CONFIG_FILE, "r");
  if (configFile) {
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      Serial.printf("CONFIG.ERROR: could not parse config json: '%s'.", error.c_str());
    } else {
      result = fromJson(doc);
      if (!result) {
        Serial.println("CONFIG.ERROR: data is incomplete");
      }
    }
    configFile.close();
  } else {
    Serial.println("CONFIG.INFO: config file not foundn");
  }

  SPIFFS.end();
  return result;
}

//------------------------------------------------------------------------------
void Config::save()
//------------------------------------------------------------------------------
{
  if (!SPIFFS.begin(true)) {
    Serial.println("CONFIG.ERROR: could not mount SPIFFS.");
    return;
  }

  DynamicJsonDocument doc(1024);
  toJson(doc);

  File configFile = SPIFFS.open(CONFIG_FILE, "w");
  serializeJson(doc, Serial);
  if (configFile) {
    serializeJson(doc, configFile);
    configFile.close();
  } else {
    Serial.println("CONFIG.ERROR: couldn not open config for to writing");
  }

  SPIFFS.end();
}
