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

#include "util/logger.h"

class Reconnector {
 public:
  typedef enum {
    DISCONNECTED = 0,
    DISCONNECTION_REQUEST,
    CONNECTION_REQUEST,
    CONNECTION_REQUEST_DELAYED,
    CONNECTED
  } ReconnectorState;

  Reconnector();

  bool begin(uint64_t delayMs = 2000);

  void tryToConnect(bool (*cb)());
  void tryToDisconnect(bool (*cb)());
  void isConnected(bool (*cb)());

  void onWifiConnected();
  void onWifiDisconnected();
  void onMQTTConfigured();

  void loop();

  ReconnectorState getState() const;
  const String& getStateName(ReconnectorState state) const;

 private:
  void setState(ReconnectorState newState);
  bool onMqttConnectionRequest();
  bool onMqttDisconnectionRequest();
  bool onIsConnectedRequest();

  Logger LOG;
  bool configured_;
  uint64_t delayMs_;
  uint64_t timer_;
  bool (*tryToConnectCB_)();
  bool (*tryToDisconnectCB_)();
  bool (*isConnectedCB_)();
  ReconnectorState state_;
  const String stateNames_[5];
};