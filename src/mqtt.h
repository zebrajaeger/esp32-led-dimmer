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

#include <map>
#include <string>

//#include <WiFi.h>
#include <arduino.h>

#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <SoftTimer.h>
#include <DelayRun.h>

#include "state.h"

class MQTT {
 public:
  MQTT();
  void init(const char *host, uint16_t port = 1883);
  void connect();
  void disconnect();
  bool isConnected();
  void onConnected(void (*cb)(bool sessionPresent));
  void onDisconnected(void (*cb)(AsyncMqttClientDisconnectReason reason));
  void subscribeJson(const char *topic, void (*cb)(JsonDocument &doc));
  void publish(const char *topic, const char *data);
  void lastWill(const char *topic, const char *data);

 protected:
  //void wifiEvent(WiFiEvent_t event);
  void onMqttConnect(bool sessionPresent);
  void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
  void onMqttSubscribe(uint16_t packetId, uint8_t qos);
  void onMqttUnsubscribe(uint16_t packetId);
  void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
  void onMqttPublish(uint16_t packetId);

  void (*onConnectedCallback_)(bool);
  void (*onDisconnectedCallback_)(AsyncMqttClientDisconnectReason);

  AsyncMqttClient mqttClient_;
  //TimerHandle_t mqttReconnectTimer_;
  std::map<std::string, void (*)(JsonDocument &doc)> subscriptions_;
};