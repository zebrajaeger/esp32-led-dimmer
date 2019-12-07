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

#include "Arduino.h"

class StatusLed {
 public:
  enum ledStatus_t { ON, OFF, BLINK_ON, BLINK_OFF };
  StatusLed(uint8_t pin, uint8_t channel = 0, double frequency = 2);
  bool begin();
  void on();
  void off();
  void blink(double duty = 0.5);
  void setFrequency(double frequency);
  double getFrequency() const;
  void alternate();

 private:
  uint8_t pin_;
  uint8_t channel_;
  double frequency_;
  uint64_t nextSwitch_;
  ledStatus_t status_;
};
