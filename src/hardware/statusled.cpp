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

#include "statusled.h"

//------------------------------------------------------------------------------
StatusLed::StatusLed(uint8_t pin, uint8_t channel, double frequency)
    : pin_(pin),
      channel_(channel),
      frequency_(frequency),
      nextSwitch_(0),
      status_(OFF)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool StatusLed::begin()
//------------------------------------------------------------------------------
{
  pinMode(pin_, OUTPUT);
  ledcSetup(channel_, frequency_, 8);
  ledcAttachPin(pin_, channel_);
  return true;
}

//------------------------------------------------------------------------------
void StatusLed::on()
//------------------------------------------------------------------------------
{
  status_ = ON;
  ledcWrite(channel_, 255);
}

//------------------------------------------------------------------------------
void StatusLed::off()
//------------------------------------------------------------------------------
{
  status_ = OFF;
  ledcWrite(channel_, 0);
}

//------------------------------------------------------------------------------
void StatusLed::blink(double duty)
//------------------------------------------------------------------------------
{
  ledcWrite(channel_, (uint32_t)(duty * 255));
}

//------------------------------------------------------------------------------
void StatusLed::setFrequency(double frequency)
//------------------------------------------------------------------------------
{
  frequency_ = frequency;
  ledcSetup(channel_, frequency_, 8);
}

//------------------------------------------------------------------------------
double StatusLed::getFrequency() const
//------------------------------------------------------------------------------
{
  return frequency_;
}

//------------------------------------------------------------------------------
void StatusLed::alternate()
//------------------------------------------------------------------------------
{
  if (status_ == BLINK_ON) {
    status_ = BLINK_OFF;
    digitalWrite(pin_, false);
  } else if (status_ == BLINK_OFF) {
    status_ = BLINK_ON;
    digitalWrite(pin_, true);
  }
}