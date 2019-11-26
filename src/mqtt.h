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

#include <WiFi.h>

#include <ArduinoJson.h>
#include <PubSubClient.h>

class Mqtt {
 public:
  typedef std::function<void(JsonDocument& doc)> SubscriptionResponseFunction_t;

  Mqtt();

  bool begin();

  void loop();

  bool connect(const String& id, const String& topic, const String& server, uint16_t port, const String& userName, const String& password);
  bool disconnect();
  bool isConnected();
  void onData(SubscriptionResponseFunction_t f);

 private:
  void callback_(char* topic, byte* payload, unsigned int length);
  WiFiClient wifiClient_;
  PubSubClient pubSubClient_;
  SubscriptionResponseFunction_t cb_;
};