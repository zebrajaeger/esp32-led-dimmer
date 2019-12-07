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

#include "state.h"

//------------------------------------------------------------------------------
State::State()
    : LOG("State")
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
uint16_t State::getFrequency() const
//------------------------------------------------------------------------------
{
  return frequency_;
}

//------------------------------------------------------------------------------
void State::setFrequency(uint16_t frequency)
//------------------------------------------------------------------------------
{
  frequency_ = frequency;
}

//------------------------------------------------------------------------------
uint16_t State::getChannelValue(uint8_t channel) const
//------------------------------------------------------------------------------
{
  return channels_[channel];
}

//------------------------------------------------------------------------------
void State::setChannelValue(uint8_t channel, uint16_t value)
//------------------------------------------------------------------------------
{
  LOG.i("set [%u] = %u", channel, value);
  channels_[channel] = value;
}

//------------------------------------------------------------------------------
void State::dump()
//------------------------------------------------------------------------------
{
  LOG.d("frequency: %u; ", frequency_);
  for (uint8_t i = 0; i < 16; ++i) {
    LOG.d("[%u]: %u; ", i, channels_[i]);
  }
}
