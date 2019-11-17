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

#include <WIFI.h>

class WiFiState {
 public:
  typedef void (*CallbackConnect)();
  typedef void (*CallbackDisconnect)(bool wasConnected);

  WiFiState();

  void onWifiEvent(WiFiEvent_t event);

  bool isConnected() const;
  void onConnect(CallbackConnect cb);
  void onDisconnect(CallbackDisconnect cb);
  
  const String& getEventName(WiFiEvent_t idx) const;
  const String& getStatusName(wl_status_t status) const;
  void printStatus() const;

 private:
  static const String eventNames_[28];
  static const String statusNames_[9];

  CallbackConnect cbConnect_;
  CallbackDisconnect cbDisconnect_;
  bool isConnected_ = false;
};
