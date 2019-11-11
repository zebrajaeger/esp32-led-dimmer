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
#include <arduino.h>

const char siteRoot[] PROGMEM = R"raw(
<html>
    <body>
        <a href="_ac">Config</a>
    </body>
</html>
)raw";

const char menuPageMqtt[] PROGMEM = R"raw(
[
  {
    "title": "Device",
    "uri": "/device",
    "menu": true,
    "element": [
      {
        "name": "devicename",
        "type": "ACInput",
        "label": "Devicename",
        "placeholder": "Devicename"
      },
      {
        "name": "set",
        "type": "ACSubmit",
        "value": "SET",
        "uri" : "/device_set"
      }
    ]
  },
  {
    "title": "Mqtt",
    "uri": "/mqtt",
    "menu": true,
    "element": [
      {
        "name": "mqtt-server",
        "type": "ACInput",
        "label": "MQTT-Server",
        "placeholder": "MQTT-Server"
      },
      {
        "name": "username",
        "type": "ACInput",
        "label": "User",
        "placeholder": "Username"
      },
      {
        "name": "password",
        "type": "ACInput",
        "label": "Password",
        "placeholder": "Password"
      },
      {
        "name": "topic",
        "type": "ACInput",
        "label": "Topic",
        "placeholder": "Topic"
      },
      {
        "name": "set",
        "type": "ACSubmit",
        "value": "SET",
        "uri" : "/mqtt_set"
      }
    ]
  }
]
)raw";