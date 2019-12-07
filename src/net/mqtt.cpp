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

#include "mqtt.h"
#include "util/jsonparser.h"

//------------------------------------------------------------------------------
Mqtt::Mqtt()
    : wifiClient_(),
      pubSubClient_(wifiClient_)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool Mqtt::begin() { return true; }

//------------------------------------------------------------------------------
void Mqtt::loop()
//------------------------------------------------------------------------------
{
  pubSubClient_.loop();
}

//------------------------------------------------------------------------------
bool Mqtt::connect(const String& id, const String& topic, const String& server, uint16_t port, const String& userName,
                   const String& password)
//------------------------------------------------------------------------------
{
  if (pubSubClient_.connected()) {
    pubSubClient_.disconnect();
  }

  // prevent consequential error due to dns requests with empty string
  if (server.isEmpty()) {
    Serial.println("[MQTT] Error: no server specified. Can not connect");
    return false;
  }

  pubSubClient_.setServer(server.c_str(), port);

  using namespace std;
  using namespace std::placeholders;
  pubSubClient_.setCallback(bind(&Mqtt::callback_, this, _1, _2, _3));

  if (pubSubClient_.connect(id.c_str(), (topic + "/alive").c_str(), 2, true, "false")) {
    pubSubClient_.publish((topic + "/alive").c_str(), "true");
    pubSubClient_.subscribe((topic + "/set").c_str());
  }
  return pubSubClient_.connected();
}

//------------------------------------------------------------------------------
bool Mqtt::disconnect()
//------------------------------------------------------------------------------
{
  pubSubClient_.disconnect();
  return pubSubClient_.connected();
}

//------------------------------------------------------------------------------
bool Mqtt::isConnected()
//------------------------------------------------------------------------------
{
  return pubSubClient_.connected();
}

//------------------------------------------------------------------------------
void Mqtt::onData(SubscriptionResponseFunction_t f)
//------------------------------------------------------------------------------
{
  cb_ = f;
}

//------------------------------------------------------------------------------
void Mqtt::callback_(char* topic, byte* payload, unsigned int length)
//------------------------------------------------------------------------------
{
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  if (cb_ != NULL) {
    cb_(doc);
  }
}

//------------------------------------------------------------------------------
void Mqtt::publish(const String& topic, const String& payload)
//------------------------------------------------------------------------------
{
  pubSubClient_.publish(topic.c_str(), payload.c_str());
}

//------------------------------------------------------------------------------
void Mqtt::publish(const String& topic, const JsonDocument& doc, bool pretty)
//------------------------------------------------------------------------------
{
  String json;
  JsonParser::doc2String(doc, json, pretty);
  pubSubClient_.publish(topic.c_str(), json.c_str());
}