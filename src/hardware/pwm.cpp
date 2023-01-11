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

#include "pwm.h"
#include "../util/dimcurve.h"

//------------------------------------------------------------------------------
PWM::PWM()
    : faboPWM_()
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool PWM::begin()
//------------------------------------------------------------------------------
{
  bool result = faboPWM_.begin();
  if (result) {
    faboPWM_.init(0);
  }
  return result;
}

//------------------------------------------------------------------------------
uint16_t PWM::setFrequency(uint16_t frequency)
//------------------------------------------------------------------------------
{
  if (frequency < 24) frequency = 24;
  if (frequency > 1526) frequency = 1526;
  faboPWM_.set_hz(frequency);
  return frequency;
}

//------------------------------------------------------------------------------
void PWM::setChannelValue(uint8_t channel, uint16_t value)
//------------------------------------------------------------------------------
{
  if (channel > 15)  channel = 15;
  if (value > 4095) value = 4095;
  faboPWM_.set_channel_value(channel, value);
}

//------------------------------------------------------------------------------
void PWM::setChannelValue8(uint8_t channel, uint8_t value)
//------------------------------------------------------------------------------
{
  if (channel > 15)  return;
  faboPWM_.set_channel_value(channel, value);
}

//------------------------------------------------------------------------------
void PWM::setChannelValueLog(uint8_t channel, uint8_t value)
//------------------------------------------------------------------------------
{
  if (channel > 15)  return;
  faboPWM_.set_channel_value(channel, pwmtable_16[value] >> 4);
}