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

#include "fram.h"

//------------------------------------------------------------------------------
FRAM::FRAM()
    : fm24c04_()
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool FRAM::begin()
//------------------------------------------------------------------------------
{
  return fm24c04_.begin();
}

//------------------------------------------------------------------------------
void FRAM::setFrequency(uint16_t frequency)
//------------------------------------------------------------------------------
{
  fm24c04_.write16(BASE_ADR_FREQUENCY, frequency);
}

//------------------------------------------------------------------------------
uint16_t FRAM::getFrequency() const
//------------------------------------------------------------------------------
{
  return fm24c04_.read16(BASE_ADR_FREQUENCY);
}

//------------------------------------------------------------------------------
void FRAM::setChannelValue(uint8_t channel, uint16_t value)
//------------------------------------------------------------------------------
{
  if (channel >= MAX_CHANNEL_COUNT) {
    channel = MAX_CHANNEL_COUNT - 1;
  }
  fm24c04_.write16(BASE_ADR_CHANNELVALUES + channel + channel, value);
}

//------------------------------------------------------------------------------
uint16_t FRAM::getChannelValue(uint8_t channel) const
//------------------------------------------------------------------------------
{
  if (channel >= MAX_CHANNEL_COUNT) {
    channel = MAX_CHANNEL_COUNT - 1;
  }
  return fm24c04_.read16(BASE_ADR_CHANNELVALUES + channel + channel);
}

//------------------------------------------------------------------------------
void FRAM::recalculateCRC()
//------------------------------------------------------------------------------
{
  uint32_t crc = fm24c04_.calcChecksum(DATA_START_ADR, DATA_END_ADR);
  fm24c04_.write32(BASE_ADR_CRC, crc);
}

//------------------------------------------------------------------------------
bool FRAM::validateCRC()
//------------------------------------------------------------------------------
{
  return fm24c04_.validate(DATA_START_ADR, DATA_END_ADR, BASE_ADR_CRC);
}

//------------------------------------------------------------------------------
void FRAM::dump() const
//------------------------------------------------------------------------------
{
  Serial.println("[FRAM] dump");
  Serial.printf("  from: %d to %d", DATA_START_ADR, COMPLETE_END_ADR);
  bool first = true;
  for (uint8_t i = DATA_START_ADR; i <= COMPLETE_END_ADR; ++i) {
    if (first || i % 8 == 0) {
      first = false;
      Serial.printf("\n  0x%.4x  ", i);
    } else {
      if (i % 4 == 0) {
        Serial.print(" ");
      }
    }
    Serial.printf("%.2x ", fm24c04_.read8(i));
  }
  Serial.println();
}