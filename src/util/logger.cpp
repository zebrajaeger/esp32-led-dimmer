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

#include "logger.h"

const char* Logger::level_str_[6] = {"F", "E", "W", "I", "D", "V"};

//------------------------------------------------------------------------------
Logger::Logger(const String& module)
    : Print(),
      module_(module)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
const void Logger::f(const char* msg, ...)
//------------------------------------------------------------------------------
{
  print("\u001b[41m");  // background red
  printPrefix(FATAL);

  char buffer[1024];
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  print(&buffer[0]);

  print("\u001b[0m");  // reset
  write('\n');
}

//------------------------------------------------------------------------------
const void Logger::e(const char* msg, ...)
//------------------------------------------------------------------------------
{
  print("\u001b[31;1m");  // bright red
  printPrefix(ERROR);

  char buffer[1024];
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  print(&buffer[0]);

  print("\u001b[0m");  // reset
  write('\n');
}

//------------------------------------------------------------------------------
const void Logger::w(const char* msg, ...)
//------------------------------------------------------------------------------
{
  print("\u001b[33;1m");  // bright yellow
  printPrefix(WARN);

  char buffer[1024];
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  print(&buffer[0]);

  print("\u001b[0m");  // reset
  write('\n');
}

//------------------------------------------------------------------------------
const void Logger::i(const char* msg, ...)
//------------------------------------------------------------------------------
{
  printPrefix(INFO);

  char buffer[1024];
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  print(&buffer[0]);

  write('\n');
}

//------------------------------------------------------------------------------
const void Logger::d(const char* msg, ...)
//------------------------------------------------------------------------------
{
  printPrefix(DEBUG);

  char buffer[1024];
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  print(&buffer[0]);

  write('\n');
}

//------------------------------------------------------------------------------
const void Logger::v(const char* msg, ...)
//------------------------------------------------------------------------------
{
  printPrefix(VERBOSE);

  char buffer[1024];
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  va_end(args);
  print(&buffer[0]);

  write('\n');
}

//------------------------------------------------------------------------------
size_t Logger::write(uint8_t c)
//------------------------------------------------------------------------------
{
  return Serial.write(c);
};

//------------------------------------------------------------------------------
const void Logger::printPrefix(Loglevel_t loglevel)
//------------------------------------------------------------------------------
{
  using namespace std::chrono;
  auto ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());

  auto secs = duration_cast<seconds>(ms);
  ms -= duration_cast<milliseconds>(secs);
  auto mins = duration_cast<minutes>(secs);
  secs -= duration_cast<seconds>(mins);
  auto hour = duration_cast<hours>(mins);
  mins -= duration_cast<minutes>(hour);

  Serial.print("* [");
  Serial.print(level_str_[loglevel]);
  Serial.print("] [");
  Serial.printf("%d:%02d:%02d.%03d", (int16_t)hour.count(), (int16_t)mins.count(), (int16_t)secs.count(), (int16_t)ms.count());
  Serial.print("] [");
  Serial.print(module_);
  Serial.print("] - ");
}
