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

#include <ArduinoJson.h>

#include "state.h"

class JsonParser {
 public:
  static const char* JSON_FREQUENCY;
  static const char* JSON_DATA;
  static const char* JSON_CHANNEL;
  static const char* JSON_CHANNEL_VALUE;

  static void parseChannelData(JsonDocument& doc, void (*cb_f)(uint16_t value), void (*cb_c)(uint8_t channel, uint16_t value));
  static void createState(JsonDocument& result, const State& state);

 protected:
  static void parseChannelDataSection(const JsonObject& section, void (*cb_c)(uint8_t channel, uint16_t value));
};