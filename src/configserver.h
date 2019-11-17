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

#include <WebServer.h>

#include <AutoConnect.h>

#include "configserver_menu.h"
class DeviceData {
 public:
  String deviceName;
};

class MqttData {
 public:
  String topic;
  String server;
  String userName;
  String password;
};

class ConfigServer {
 public:
  typedef std::function<void(DeviceData& data)> deviceSetFunction_t;
  typedef std::function<void(MqttData& data)> mqttSetFunction_t;

  ConfigServer();

  bool begin(String& title);
  void loop();
  void setDeviceData(DeviceData& data);
  void setMqttData(MqttData& data);
  void onDeviceSet(deviceSetFunction_t f);
  void onMqttSet(mqttSetFunction_t f);

 private:
  void onDeviceSet_();
  void onMqttSet_();
  void redirect(const String toLocation);

  WebServer webServer_;
  AutoConnect autoConnect_;
  deviceSetFunction_t onDeviceSetCB_;
  mqttSetFunction_t onMqttSetCB_;
};
