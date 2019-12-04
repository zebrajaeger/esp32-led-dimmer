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

#include "multitimer.h"

//------------------------------------------------------------------------------
MultiTimer::MultiTimer()
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool MultiTimer::begin()
//------------------------------------------------------------------------------
{
  return true;
}

//------------------------------------------------------------------------------
void MultiTimer::set(String name, uint64_t periodMs, TimerCallback cb)
//------------------------------------------------------------------------------
{
  TimerData* temp = new TimerData();
  temp->nextCall = esp_timer_get_time();
  temp->period = periodMs * 1000;
  temp->cb = cb;
  timers_[name] = temp;
}

//------------------------------------------------------------------------------
void MultiTimer::remove(String name)
//------------------------------------------------------------------------------
{
  std::map<String, TimerData*>::const_iterator it = timers_.find(name);
  if (it != timers_.end()) {
    if (it->second) {
      delete it->second;
    }
  }
  timers_.erase(name);
}

//------------------------------------------------------------------------------
void MultiTimer::loop()
//------------------------------------------------------------------------------
{
  uint64_t now = esp_timer_get_time();
  std::for_each(timers_.begin(), timers_.end(), [&now](std::pair<String, TimerData*> timer) {
    if (timer.second->nextCall <= now) {
      timer.second->nextCall += timer.second->period;
      timer.second->cb();
    }
  });
}