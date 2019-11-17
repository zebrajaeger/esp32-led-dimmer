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

#include "fm24c04.h"

#include <Arduino.h>
#include <CRC32.h>

FM24C04::FM24C04(void) { _framInitialised = false; }

//------------------------------------------------------------------------------
bool FM24C04::begin(uint8_t addr)
//------------------------------------------------------------------------------
{
  i2c_addr = addr;

  Wire.beginTransmission(i2c_addr);
  Wire.write(0);
  if (Wire.endTransmission() == 0) {
    _framInitialised = true;
    return true;
  } else {
    return false;
  }
}

//------------------------------------------------------------------------------
void FM24C04::write8(uint8_t framAddr, uint8_t value)
//------------------------------------------------------------------------------
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(framAddr);
  Wire.write(value);
  Wire.endTransmission();
}

//------------------------------------------------------------------------------
uint8_t FM24C04::read8(uint8_t framAddr) const
//------------------------------------------------------------------------------
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(framAddr);
  Wire.endTransmission();

  Wire.requestFrom(i2c_addr, (uint8_t)1);

  return Wire.read();
}

//------------------------------------------------------------------------------
void FM24C04::write16(uint8_t framAddr, uint16_t value)
//------------------------------------------------------------------------------
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(framAddr);
  Wire.write((value >> 8) & 0xff);
  Wire.write(value & 0xff);
  Wire.endTransmission();
}

//------------------------------------------------------------------------------
uint16_t FM24C04::read16(uint8_t framAddr) const
//------------------------------------------------------------------------------
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(framAddr);
  Wire.endTransmission();

  Wire.requestFrom(i2c_addr, (uint8_t)2);

  uint16_t result = Wire.read();
  return (result << 8) + Wire.read();
}

//------------------------------------------------------------------------------
void FM24C04::write32(uint8_t framAddr, uint32_t value)
//------------------------------------------------------------------------------
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(framAddr);
  Wire.write((value >> 24) & 0xff);
  Wire.write((value >> 16) & 0xff);
  Wire.write((value >> 8) & 0xff);
  Wire.write(value & 0xff);
  Wire.endTransmission();
}

//------------------------------------------------------------------------------
uint32_t FM24C04::read32(uint8_t framAddr) const
//------------------------------------------------------------------------------
{
  Wire.beginTransmission(i2c_addr);
  Wire.write(framAddr);
  Wire.endTransmission();

  Wire.requestFrom(i2c_addr, (uint8_t)4);

  uint32_t result = Wire.read();
  result = (result << 8) | Wire.read();
  result = (result << 8) | Wire.read();
  return (result << 8) + Wire.read();
}

//------------------------------------------------------------------------------
void FM24C04::print_wire_result(uint8_t r) const
//------------------------------------------------------------------------------
{
  if (r == 0) {
    Serial.println("FRAM: success");
  } else if (r == 1) {
    Serial.println("FRAM: data too long to fit in transmit buffer");
  } else if (r == 2) {
    Serial.println("FRAM: received NACK on transmit of address");
  } else if (r == 3) {
    Serial.println("FRAM: received NACK on transmit of data");
  } else if (r == 4) {
    Serial.println("FRAM: other error");
  } else {
    Serial.println("FRAM: unknown error");
  }
}

//------------------------------------------------------------------------------
void FM24C04::setRange(uint8_t from, uint8_t to, uint8_t with)
//------------------------------------------------------------------------------
{
  for (uint8_t i = from; i <= to; ++i) {
    write8(i, with);
  }
}

//------------------------------------------------------------------------------
void FM24C04::refreshChecksum(uint8_t from, uint8_t to, uint8_t checksumAdr)
//------------------------------------------------------------------------------
{
  uint32_t checksum = calcChecksum(from, to);
  write32(checksumAdr, checksum);
}

//------------------------------------------------------------------------------
bool FM24C04::validate(uint8_t from, uint8_t to, uint8_t checksumAdr) const
//------------------------------------------------------------------------------
{
  uint32_t checksum = calcChecksum(from, to);
  uint32_t storedChecksum = read32(checksumAdr);
  return (checksum == storedChecksum);
}

//------------------------------------------------------------------------------
uint32_t FM24C04::calcChecksum(uint8_t from, uint8_t to) const
//------------------------------------------------------------------------------
{
  CRC32 crc;
  for (uint8_t i = from; i <= to; ++i) {
    crc.update(read8(i));
  }
  return crc.finalize();
}
