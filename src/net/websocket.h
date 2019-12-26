
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

#include <WebSocketsServer.h>

class Websocket {
 public:
  typedef void (*onStatusRequest_t)(const uint8_t clientId);
  typedef void (*onData_t)(const uint8_t clientId, const String& json);

  //------------------------------------------------------------------------------
  Websocket();
  ~Websocket();
  bool begin(uint16_t port);
  void loop();
  void send(String& data);
  void send(uint8_t clientId, String& data);
  void onStatusRequest(onStatusRequest_t cb);
  void onData(onData_t cb);

 private:
  void onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);

  bool initialized_;
  WebSocketsServer* wsServer_;
  onStatusRequest_t onStatusRequest_;
  onData_t onData_;
};