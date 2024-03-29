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

#include "configserver.h"

#include <SPIFFS.h>

#include "webapp/files.h"
extern const char configServerMenu[] asm("_binary_src_config_configserver_menu_json_start");

//------------------------------------------------------------------------------
ConfigServer::ConfigServer()
    : LOG("ConfigServer"),
      webServer_(),
      autoConnect_(webServer_),
      onDeviceSetCB_(NULL),
      onMqttSetCB_(NULL)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool ConfigServer::begin(String& title)
//------------------------------------------------------------------------------
{
  #include "webapp/webserver.h"

  webServer_.on("/device_set", std::bind(&ConfigServer::onDeviceSet_, this));
  webServer_.on("/mqtt_set", std::bind(&ConfigServer::onMqttSet_, this));
  webServer_.on("/artnet_set", std::bind(&ConfigServer::onArtnetSet_, this));

  AutoConnectConfig autoConnectConfig;
  autoConnectConfig.title = title;
  autoConnectConfig.hostName = title;
  autoConnectConfig.autoReconnect = true;
  autoConnect_.config(autoConnectConfig);

  // load config
  bool result = autoConnect_.load(configServerMenu);
  if (result) {
    LOG.i("AutoConnect loaded.");
  } else {
    LOG.i("ERROR: Autoconnect load failed.");
  }

  // start
  if (result) {
    result = autoConnect_.begin();
    if (result) {
      LOG.i("AutoConnect started.");
    } else {
      LOG.i("ERROR: Autoconnect start failed.");
    }
  }

  return result;
}

//------------------------------------------------------------------------------
void ConfigServer::loop()
//------------------------------------------------------------------------------
{
  autoConnect_.handleClient();
}

//------------------------------------------------------------------------------
void ConfigServer::setDeviceData(DeviceData& data)
//------------------------------------------------------------------------------
{
  AutoConnectAux* device = autoConnect_.aux("/device");
  if (!device) {
    Serial.println("[ConfigServer] Error: could not found device section in configuration");
    return;
  }
  AutoConnectInput& deviceName = device->getElement<AutoConnectInput>("devicename");
  deviceName.value = data.deviceName;
}

//------------------------------------------------------------------------------
void ConfigServer::setMqttData(MqttData& data)
//------------------------------------------------------------------------------
{
  AutoConnectAux* mqtt = autoConnect_.aux("/mqtt");
  if (!mqtt) {
    Serial.println("[ConfigServer] Error: could not found mqtt section in configuration");
    return;
  }
  AutoConnectInput& topic = mqtt->getElement<AutoConnectInput>("topic");
  topic.value = data.topic;

  AutoConnectInput& mqttServer = mqtt->getElement<AutoConnectInput>("mqtt-server");
  mqttServer.value = data.server;

  AutoConnectInput& username = mqtt->getElement<AutoConnectInput>("username");
  username.value = data.userName;

  AutoConnectInput& password = mqtt->getElement<AutoConnectInput>("password");
  password.value = data.password;
}

//------------------------------------------------------------------------------
void ConfigServer::setArtnetData(ArtnetData& data)
//------------------------------------------------------------------------------
{
  AutoConnectAux* mqtt = autoConnect_.aux("/artnet");
  if (!mqtt) {
    Serial.println("[ConfigServer] Error: could not found artnet section in configuration");
    return;
  }
  AutoConnectInput& topic = mqtt->getElement<AutoConnectInput>("universe");
  topic.value = data.universe;
}

//------------------------------------------------------------------------------
void ConfigServer::onDeviceSet(deviceSetFunction_t f)
//------------------------------------------------------------------------------
{
  onDeviceSetCB_ = f;
}

//------------------------------------------------------------------------------
void ConfigServer::onMqttSet(mqttSetFunction_t f)
//------------------------------------------------------------------------------
{
  onMqttSetCB_ = f;
}

//------------------------------------------------------------------------------
void ConfigServer::onArtnetSet(artnetSetFunction_t f)
//------------------------------------------------------------------------------
{
  onArtnetSetCB_ = f;
}

//------------------------------------------------------------------------------
void ConfigServer::onDeviceSet_()
//------------------------------------------------------------------------------
{
  if (onDeviceSetCB_ != NULL) {
    DeviceData data;
    data.deviceName = webServer_.arg("devicename");
    onDeviceSetCB_(data);
  }
  redirect("/device");
}

//------------------------------------------------------------------------------
void ConfigServer::onMqttSet_()
//------------------------------------------------------------------------------
{
  if (onMqttSetCB_ != NULL) {
    MqttData data;
    data.topic = webServer_.arg("topic");
    data.server = webServer_.arg("mqtt-server");
    data.userName = webServer_.arg("username");
    data.password = webServer_.arg("password");
    onMqttSetCB_(data);
  }
  redirect("/mqtt");
}
//------------------------------------------------------------------------------
void ConfigServer::onArtnetSet_()
//------------------------------------------------------------------------------
{
  if (onArtnetSetCB_ != NULL) {
    ArtnetData data;
    data.universe = webServer_.arg("universe").toInt();
    onArtnetSetCB_(data);
  }
  redirect("/artnet");
}

//------------------------------------------------------------------------------
void ConfigServer::redirect(const String toLocation)
//------------------------------------------------------------------------------
{
  webServer_.sendHeader("Location", toLocation, true);
  webServer_.send(302, "text/plain", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webServer_.client().stop();
}
