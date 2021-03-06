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

#include "util/logger.h"

class State {
 public:
  State();
  uint16_t getFrequency() const;
  void setFrequency(uint16_t frequency);

  uint16_t getChannelValue(uint8_t channel) const;
  void setChannelValue(uint8_t channel, uint16_t value);
  void dump();

 protected:
  uint16_t frequency_;
  uint16_t channels_[16];

  private:
  Logger LOG;
};