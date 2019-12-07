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
#include <Print.h>
#include <chrono>

#ifndef LOGGER_BUFFER_SIZE
#define LOGGER_BUFFER_SIZE 1024
#endif

class Logger : public Print {
 public:
  enum Loglevel_t { FATAL, ERROR, WARN, INFO, DEBUG, VERBOSE };

  Logger(const String& module);

  const void f(const char* msg, ...);
  const void e(const char* msg, ...);
  const void w(const char* msg, ...);
  const void i(const char* msg, ...);
  const void d(const char* msg, ...);
  const void v(const char* msg, ...);

  virtual size_t write(uint8_t c);

 private:
  const void printPrefix(Loglevel_t loglevel);

  const String module_;
  static const char* level_str_[6];
  char buffer[LOGGER_BUFFER_SIZE];
};