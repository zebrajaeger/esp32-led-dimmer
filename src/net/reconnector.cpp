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

#include "reconnector.h"

//------------------------------------------------------------------------------
Reconnector::Reconnector()
    : LOG("Reconnector"),
      configured_(false),
      delayMs_(2000),
      timer_(0),
      tryToConnectCB_(NULL),
      tryToDisconnectCB_(NULL),
      isConnectedCB_(NULL),
      state_(DISCONNECTED),
      stateNames_{"DISCONNECTED", "DISCONNECTION_REQUEST", "CONNECTION_REQUEST", "CONNECTION_REQUEST_DELAYED", "CONNECTED"}
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool Reconnector::begin(uint64_t delayMs)
//------------------------------------------------------------------------------
{
  delayMs_ = delayMs;
  return true;
}

//------------------------------------------------------------------------------
void Reconnector::tryToConnect(bool (*cb)())
//------------------------------------------------------------------------------
{
  tryToConnectCB_ = cb;
}

//------------------------------------------------------------------------------
void Reconnector::tryToDisconnect(bool (*cb)())
//------------------------------------------------------------------------------
{
  tryToDisconnectCB_ = cb;
}

//------------------------------------------------------------------------------
void Reconnector::isConnected(bool (*cb)())
//------------------------------------------------------------------------------
{
  isConnectedCB_ = cb;
}

//------------------------------------------------------------------------------
void Reconnector::onWifiConnected()
//------------------------------------------------------------------------------
{
  setState(CONNECTION_REQUEST);
}

//------------------------------------------------------------------------------
void Reconnector::onWifiDisconnected()
//------------------------------------------------------------------------------
{
  switch (state_) {
    case DISCONNECTED:
    case DISCONNECTION_REQUEST:
      break;

    case CONNECTION_REQUEST:
    case CONNECTION_REQUEST_DELAYED:
      setState(DISCONNECTED);
      break;

    case CONNECTED:
      setState(DISCONNECTION_REQUEST);
      break;
  }
}

//------------------------------------------------------------------------------
void Reconnector::onMQTTConfigured()
//------------------------------------------------------------------------------
{
  configured_ = true;
}

//------------------------------------------------------------------------------
void Reconnector::loop()
//------------------------------------------------------------------------------
{
  switch (state_) {
    case DISCONNECTED:
      break;

    case CONNECTION_REQUEST:
      if (configured_) {
        onMqttConnectionRequest();
      }
      break;

    case DISCONNECTION_REQUEST:
      onMqttDisconnectionRequest();
      break;

    case CONNECTION_REQUEST_DELAYED:
      if (timer_ < millis()) {
        onMqttConnectionRequest();
      }
      break;

    case CONNECTED:
      if (!onIsConnectedRequest()) {
        setState(CONNECTION_REQUEST_DELAYED);  // prevent fast reconnection loop
        timer_ = millis() + delayMs_;
      }
      break;
  }
}

//------------------------------------------------------------------------------
const String& Reconnector::getStateName(ReconnectorState state) const
//------------------------------------------------------------------------------
{
  return stateNames_[state];
}

//------------------------------------------------------------------------------
Reconnector::ReconnectorState Reconnector::getState() const
//------------------------------------------------------------------------------
{
  return state_;
}

//------------------------------------------------------------------------------
void Reconnector::setState(ReconnectorState newState)
//------------------------------------------------------------------------------
{
  if (state_ != newState) {
    LOG.d("state changed from '%s' to '%s'\n", getStateName(state_).c_str(), getStateName(newState).c_str());
    state_ = newState;
  }
}

//------------------------------------------------------------------------------
bool Reconnector::onMqttConnectionRequest()
//------------------------------------------------------------------------------
{
  bool result = false;
  if (tryToConnectCB_ != NULL) {
    result = tryToConnectCB_();
  }

  if (result) {
    setState(CONNECTED);
  } else {
    setState(CONNECTION_REQUEST_DELAYED);
    timer_ = millis() + delayMs_;
  }

  return result;
}

//------------------------------------------------------------------------------
bool Reconnector::onMqttDisconnectionRequest()
//------------------------------------------------------------------------------
{
  bool result = false;
  if (tryToDisconnectCB_ != NULL) {
    result = tryToDisconnectCB_();
  }

  setState(DISCONNECTED);

  return result;
}

//------------------------------------------------------------------------------
bool Reconnector::onIsConnectedRequest()
//------------------------------------------------------------------------------
{
  bool result = true;
  if (isConnectedCB_ != NULL) {
    result = isConnectedCB_();
  }
  return result;
}
