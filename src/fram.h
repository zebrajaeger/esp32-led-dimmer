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

#include "fm24c04.h"
#include "Logger.h"

class FRAM {
 public:
  static const uint8_t MAX_CHANNEL_COUNT = 16;
  static const uint16_t DATA_START_ADR = 0;
  static const uint16_t BASE_ADR_FREQUENCY = DATA_START_ADR;
  static const uint16_t BASE_ADR_CHANNELVALUES = BASE_ADR_FREQUENCY + 2;
  static const uint16_t BASE_ADR_CRC = BASE_ADR_CHANNELVALUES + (MAX_CHANNEL_COUNT * 2);
  static const uint16_t DATA_END_ADR = BASE_ADR_CRC - 1;
  static const uint16_t COMPLETE_END_ADR = BASE_ADR_CRC + 3;

  FRAM();
  bool begin();
  void setFrequency(uint16_t frequency);
  uint16_t getFrequency() const;
  void setChannelValue(uint8_t channel, uint16_t value);
  uint16_t getChannelValue(uint8_t channel) const;
  void recalculateCRC();
  bool validateCRC();
  // void dump();

 private:
  Logger LOG;
  FM24C04 fm24c04_;
};