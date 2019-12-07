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
#include <map>

class MultiTimer {
 public:
  typedef void (*TimerCallback)();

  MultiTimer();

  bool begin();
  void loop();
  void set(String name, uint64_t periodMs, TimerCallback cb);
  void remove(String name);

 private:
  struct TimerData {
    uint64_t nextCall;
    uint64_t period;
    TimerCallback cb;
    uint64_t called;
  };

  std::map<String, TimerData*> timers_;
};