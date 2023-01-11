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

#include "wifistate.h"

//------------------------------------------------------------------------------
const String WiFiState::eventNames_[28]{"SYSTEM_EVENT_WIFI_READY",
                                        "SYSTEM_EVENT_SCAN_DONE",
                                        "SYSTEM_EVENT_STA_START",
                                        "SYSTEM_EVENT_STA_STOP",
                                        "SYSTEM_EVENT_STA_CONNECTED",
                                        "SYSTEM_EVENT_STA_DISCONNECTED",
                                        "SYSTEM_EVENT_STA_AUTHMODE_CHANGE",
                                        "SYSTEM_EVENT_STA_GOT_IP",
                                        "SYSTEM_EVENT_STA_LOST_IP",
                                        "SYSTEM_EVENT_STA_WPS_ER_SUCCESS",
                                        "SYSTEM_EVENT_STA_WPS_ER_FAILED",
                                        "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT",
                                        "SYSTEM_EVENT_STA_WPS_ER_PIN",
                                        "SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP",
                                        "SYSTEM_EVENT_AP_START",
                                        "SYSTEM_EVENT_AP_STOP",
                                        "SYSTEM_EVENT_AP_STACONNECTED",
                                        "SYSTEM_EVENT_AP_STADISCONNECTED",
                                        "SYSTEM_EVENT_AP_STAIPASSIGNED",
                                        "SYSTEM_EVENT_AP_PROBEREQRECVED",
                                        "SYSTEM_EVENT_GOT_IP6",
                                        "SYSTEM_EVENT_ETH_START",
                                        "SYSTEM_EVENT_ETH_STOP",
                                        "SYSTEM_EVENT_ETH_CONNECTED",
                                        "SYSTEM_EVENT_ETH_DISCONNECTED",
                                        "SYSTEM_EVENT_ETH_GOT_IP",
                                        "SYSTEM_EVENT_MAX",
                                        "UNKNOWN"};

//------------------------------------------------------------------------------
const String WiFiState::statusNames_[9]{"WL_IDLE_STATUS",     "WL_NO_SSID_AVAIL", "WL_SCAN_COMPLETED", "WL_CONNECTED", "WL_CONNECT_FAILED",
                                        "WL_CONNECTION_LOST", "WL_DISCONNECTED",  "WL_NO_SHIELD",      "UNKNOWN"};

//------------------------------------------------------------------------------
WiFiState::WiFiState()
    : LOG("WiFiState"),
      cbConnect_(NULL),
      cbDisconnect_(NULL)
//------------------------------------------------------------------------------
{
  // WiFi.onEvent(std::bind(&WiFiState::onWifiEvent, this, std::placeholders::_1, this, std::placeholders::_2));
  WiFi.onEvent(std::bind(&WiFiState::onWifiEvent, this, std::placeholders::_1, std::placeholders::_2));
}

//------------------------------------------------------------------------------
void WiFiState::onWifiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
//------------------------------------------------------------------------------
{
  LOG.d("event: %d - %s", event, getEventName(event).c_str());
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      LOG.d("connected");
      isConnected_ = true;
      if (cbConnect_ != NULL) {
        cbConnect_();
      }
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      LOG.d("disconnected, Reason: %u", info.wps_fail_reason);
      if (cbDisconnect_ != NULL) {
        cbDisconnect_(isConnected_, info.wps_fail_reason);
      }
      isConnected_ = false;
      break;
    default:
      break;
  }
}

//------------------------------------------------------------------------------
void WiFiState::printStatus()
//------------------------------------------------------------------------------
{
  LOG.i("Status: %d - %s", WiFi.status(), getStatusName(WiFi.status()).c_str());
}

//------------------------------------------------------------------------------
void WiFiState::onConnect(CallbackConnect cb)
//------------------------------------------------------------------------------
{
  cbConnect_ = cb;
}

//------------------------------------------------------------------------------
void WiFiState::onDisconnect(CallbackDisconnect cb)
//------------------------------------------------------------------------------
{
  cbDisconnect_ = cb;
}

//------------------------------------------------------------------------------
const String& WiFiState::getEventName(WiFiEvent_t idx) const
//------------------------------------------------------------------------------
{
  return eventNames_[idx];
}

//------------------------------------------------------------------------------
const String& WiFiState::getStatusName(wl_status_t status) const
//------------------------------------------------------------------------------
{
  uint8_t idx = (uint8_t)status;
  if (idx == 255) {
    idx = 8;
  } else if (idx > 6 && idx < 255) {
    idx = 9;
  }
  return statusNames_[idx];
}

//------------------------------------------------------------------------------
bool WiFiState::isConnected() const
//------------------------------------------------------------------------------
{
  return isConnected_;
}
