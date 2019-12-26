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

#include "websocket.h"

//------------------------------------------------------------------------------
Websocket::Websocket()
    : initialized_(false),
      wsServer_(NULL),
      onStatusRequest_(NULL),
      onData_(NULL)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
Websocket::~Websocket()
//------------------------------------------------------------------------------
{
  if (!initialized_) {
    wsServer_->close();
    delete wsServer_;
  }
}

//------------------------------------------------------------------------------
bool Websocket::begin(uint16_t port)
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  if (!initialized_) {
    wsServer_ = new WebSocketsServer(port);
    wsServer_->begin();
    wsServer_->onEvent(bind(&Websocket::onWsEvent, this, _1, _2, _3, _4));
    initialized_ = true;
  }
  return initialized_;
}

//------------------------------------------------------------------------------
void Websocket::loop()
//------------------------------------------------------------------------------
{
  if (initialized_) {
    wsServer_->loop();
  }
}

//------------------------------------------------------------------------------
void Websocket::send(String& data)
//------------------------------------------------------------------------------
{
  if (initialized_) {
    wsServer_->broadcastTXT(data);
  }
}

//------------------------------------------------------------------------------
void Websocket::send(uint8_t clientId, String& data)
//------------------------------------------------------------------------------
{
  if (initialized_) {
    wsServer_->sendTXT(clientId, data);
  }
}

//------------------------------------------------------------------------------
void Websocket::onStatusRequest(onStatusRequest_t cb)
//------------------------------------------------------------------------------
{
  onStatusRequest_ = cb;
}

//------------------------------------------------------------------------------
void Websocket::onData(onData_t cb)
//------------------------------------------------------------------------------
{
  onData_ = cb;
}

//------------------------------------------------------------------------------
void Websocket::onWsEvent(uint8_t clientId, WStype_t type, uint8_t* payload, size_t length)
//------------------------------------------------------------------------------
{
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", clientId);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = wsServer_->remoteIP(clientId);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", clientId, ip[0], ip[1], ip[2], ip[3], payload);
      if (onStatusRequest_) {
        onStatusRequest_(clientId);
      }
    } break;
    case WStype_TEXT:
      if (onData_) {
        String data((char*)payload);
        onData_(clientId, data);
      }
      break;
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}