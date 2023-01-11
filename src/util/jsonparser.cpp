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

#include "jsonparser.h"

const char* JsonParser::JSON_LIGHT = "light";
const char* JsonParser::JSON_FREQUENCY = "frequency";
const char* JsonParser::JSON_DATA = "data";
const char* JsonParser::JSON_CHANNEL = "channel";
const char* JsonParser::JSON_CHANNEL_VALUE = "value";
const char* JsonParser::JSON_CHANNEL_ALL_VALUE = "all";

const char* JsonParser::JSON_DEVICE = "device";
const char* JsonParser::JSON_DEVICE_NAME = "name";
const char* JsonParser::JSON_DEVICE_IP = "ip";

//------------------------------------------------------------------------------
void JsonParser::parseChannelData(JsonDocument& doc, void (*cb_f)(uint16_t value), void (*cb_c)(uint8_t channel, uint16_t value))
//------------------------------------------------------------------------------
{
  // frequency
  if (doc.containsKey(JSON_FREQUENCY)) {
    uint16_t f = doc[JSON_FREQUENCY];
    cb_f(f);
  }

  // data
  if (doc.containsKey(JSON_DATA)) {
    if (doc[JSON_DATA].is<JsonArray>()) {
      JsonArray array = doc[JSON_DATA];
      for (const JsonObject section : array) {
        parseChannelDataSection(section, cb_c);
      }
    } else if (doc[JSON_DATA].is<JsonObject>()) {
      parseChannelDataSection(doc[JSON_DATA], cb_c);
    }
  }
}

//------------------------------------------------------------------------------
void JsonParser::parseChannelDataSection(const JsonObject& section, void (*cb_c)(uint8_t channel, uint16_t value))
//------------------------------------------------------------------------------
{
  if (section.containsKey(JSON_CHANNEL_ALL_VALUE)) {
    if (section[JSON_CHANNEL_ALL_VALUE].is<JsonArray>()) {
      const JsonArray& array = section[JSON_CHANNEL_ALL_VALUE].as<JsonArray>();
      uint8_t arrayIndex = 0;
      uint8_t size = array.size();
      for (uint8_t i = 0; i < 16; ++i) {
        cb_c(i, array[arrayIndex++ % size]);
      }
    } else if (section[JSON_CHANNEL_ALL_VALUE].is<uint16_t>()) {
      uint16_t all = section[JSON_CHANNEL_ALL_VALUE];
      for (uint8_t i = 0; i < 16; ++i) {
        cb_c(i, all);
      }
    }
  }

  if (section.containsKey(JSON_CHANNEL) && section.containsKey(JSON_CHANNEL_VALUE)) {
    // channel
    uint8_t c = section[JSON_CHANNEL];

    // channel value(s)
    if (section[JSON_CHANNEL_VALUE].is<JsonArray>()) {
      const JsonArray& array = section[JSON_CHANNEL_VALUE].as<JsonArray>();
      uint8_t l = array.size();
      for (uint8_t i = 0; i < l; i++) {
        cb_c(c + i, array[i]);
      }
    } else if (section[JSON_CHANNEL_VALUE].is<uint16_t>()) {
      cb_c(c, section[JSON_CHANNEL_VALUE]);
    }
  }
}

//------------------------------------------------------------------------------
void JsonParser::toJson(JsonDocument& result, const State& state, const DeviceData& deviceData)
//------------------------------------------------------------------------------
{
  // device
  JsonObject device = result.createNestedObject(JSON_DEVICE);
  device[JSON_DEVICE_NAME] = deviceData.name;
  device[JSON_DEVICE_IP] = deviceData.ip;

  // light
  JsonObject light = result.createNestedObject(JSON_LIGHT);
  light[JSON_FREQUENCY] = state.getFrequency();
  JsonObject data = light.createNestedObject(JSON_DATA);
  data[JSON_CHANNEL] = 0;
  JsonArray values = data.createNestedArray(JSON_CHANNEL_VALUE);
  for (uint8_t i = 0; i < 16; ++i) {
    values.add(state.getChannelValue(i));
  }
}

//------------------------------------------------------------------------------
void JsonParser::doc2String(const JsonDocument& doc, String& result, bool pretty)
//------------------------------------------------------------------------------
{
  if (pretty) {
    serializeJsonPretty(doc, result);
  } else {
    serializeJson(doc, result);
  }
}

//------------------------------------------------------------------------------
void JsonParser::channelDataToJsonArray(String& result, const String& msgId, const State& state)
//------------------------------------------------------------------------------
{
  const size_t capacity = JSON_ARRAY_SIZE(16) + JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);
  doc["msgId"] = msgId;
  JsonArray data = doc.createNestedArray("data");
  for (uint8_t i = 0; i < 16; ++i) {
    data.add(state.getChannelValue(i));
  }
  JsonParser::doc2String(doc, result, false);
}

//------------------------------------------------------------------------------
bool JsonParser::channelDataFromJsonArray(const String& json, String& msgId, uint16_t result[16])
//------------------------------------------------------------------------------
{
  const size_t capacity = JSON_ARRAY_SIZE(16) + JSON_OBJECT_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(err.c_str());
    return false;
  } else {
    msgId = doc["msgId"].as<const char*>();
    JsonArray data = doc["data"];
    for (uint8_t i = 0; i < 16; ++i) {
      result[i] = data[i];
    }
    return true;
  }
}

//------------------------------------------------------------------------------
void JsonParser::channelDataResponse(String& json, String& msgId)
//------------------------------------------------------------------------------
{
  const size_t capacity = JSON_OBJECT_SIZE(1);
  DynamicJsonDocument doc(capacity);

  doc["msgId"] = msgId;

  serializeJson(doc, json);
}
