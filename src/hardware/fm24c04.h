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

#include <Wire.h>

#define FM24C04_DEFAULT_ADDRESS                        \
  (0x50) /* 1010 + A2 + A1 + A0 = 0x50 default \ \ \ \ \
          */

class FM24C04 {
 public:
  FM24C04(void);

  bool begin(uint8_t addr = FM24C04_DEFAULT_ADDRESS);
  void write8(uint8_t framAddr, uint8_t value);
  uint8_t read8(uint8_t framAddr) const;
  void write16(uint8_t framAddr, uint16_t value);
  uint16_t read16(uint8_t framAddr) const;
  void write32(uint8_t framAddr, uint32_t value);
  uint32_t read32(uint8_t framAddr) const;

  void setRange(uint8_t from, uint8_t to, uint8_t with);
  void refreshChecksum(uint8_t from, uint8_t to, uint8_t checksumAddr);
  /**
   * @return true if valid
   */
  bool validate(uint8_t from, uint8_t to, uint8_t checksumAddr) const;
  uint32_t calcChecksum(uint8_t from, uint8_t to) const;

 protected:
  void print_wire_result(uint8_t r) const;

 private:
  uint8_t i2c_addr;
  bool _framInitialised;
};