
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
#include <mqtt.h>

#include <WiFi.h>

#include "utils.h"

//------------------------------------------------------------------------------
MQTT::MQTT()
    : onConnectedCallback_(0),
      onDisconnectedCallback_(0)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
void MQTT::init(const char *host, uint16_t port)
//------------------------------------------------------------------------------
{
  mqttClient_.onConnect(std::bind(&MQTT::onMqttConnect, this, std::placeholders::_1));
  mqttClient_.onDisconnect(std::bind(&MQTT::onMqttDisconnect, this, std::placeholders::_1));
  mqttClient_.onSubscribe(std::bind(&MQTT::onMqttSubscribe, this, std::placeholders::_1, std::placeholders::_2));
  mqttClient_.onUnsubscribe(std::bind(&MQTT::onMqttUnsubscribe, this, std::placeholders::_1));
  mqttClient_.onMessage(std::bind(&MQTT::onMqttMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                                  std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
  mqttClient_.onPublish(std::bind(&MQTT::onMqttPublish, this, std::placeholders::_1));
  mqttClient_.setServer(host, port);
}

//------------------------------------------------------------------------------
void MQTT::connect()
//------------------------------------------------------------------------------
{
  Serial.println("Connecting to MQTT...");
  mqttClient_.connect();
}

//------------------------------------------------------------------------------
void MQTT::disconnect()
//------------------------------------------------------------------------------
{
  Serial.println("Connecting to MQTT...");
  mqttClient_.disconnect();
}

//------------------------------------------------------------------------------
bool MQTT::isConnected()
//------------------------------------------------------------------------------
{
  return mqttClient_.connected();
}

//------------------------------------------------------------------------------
void MQTT::onMqttConnect(bool sessionPresent)
//------------------------------------------------------------------------------
{
  Serial.println("Connected to MQTT.");
  if (onConnectedCallback_) {
    onConnectedCallback_(sessionPresent);
  }
}

//------------------------------------------------------------------------------
void MQTT::onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
//------------------------------------------------------------------------------
{
  Serial.println("Disconnected from MQTT.");
  if (onDisconnectedCallback_) {
    onDisconnectedCallback_(reason);
  }
}

//------------------------------------------------------------------------------
void MQTT::onMqttSubscribe(uint16_t packetId, uint8_t qos)
//------------------------------------------------------------------------------
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

//------------------------------------------------------------------------------
void MQTT::onMqttUnsubscribe(uint16_t packetId)
//------------------------------------------------------------------------------
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

//------------------------------------------------------------------------------
void MQTT::onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
//------------------------------------------------------------------------------
{
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.print("  len: ");
  Serial.println(len);

  std::map<std::string, void (*)(JsonDocument & doc)>::iterator it = subscriptions_.find(topic);
  if (it != subscriptions_.end()) {
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, len);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }

    it->second(doc);
  }
}

//------------------------------------------------------------------------------
void MQTT::onMqttPublish(uint16_t packetId)
//------------------------------------------------------------------------------
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

//------------------------------------------------------------------------------
void MQTT::onConnected(void (*cb)(bool sessionPresent))
//------------------------------------------------------------------------------
{
  onConnectedCallback_ = cb;
}

//------------------------------------------------------------------------------
void MQTT::subscribeJson(const char *topic, void (*cb)(JsonDocument &doc))
//------------------------------------------------------------------------------
{
  subscriptions_[topic] = cb;
  mqttClient_.subscribe(topic, 2);
}

//------------------------------------------------------------------------------
void MQTT::publish(const char *topic, const char *data)
//------------------------------------------------------------------------------
{
  mqttClient_.publish(topic, 2, true, data);
}

//------------------------------------------------------------------------------
void MQTT::lastWill(const char *topic, const char *data)
//------------------------------------------------------------------------------
{
  mqttClient_.setWill(topic, 2, true, data);
}