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
#include "ota.h"

#include <FS.h>
#include <SPIFFS.h>
#include <arduino.h>

#include <ArduinoOTA.h>

//------------------------------------------------------------------------------
OTA::OTA()
//------------------------------------------------------------------------------
{
  isUpdating_ = false;
}

//------------------------------------------------------------------------------
void OTA::begin()
//------------------------------------------------------------------------------
{
  ArduinoOTA
      .onStart([=]() {
        isUpdating_ = true;
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
          type = "sketch";
        } else {
          type = "filesystem";
          SPIFFS.end();
        }
        Serial.println("Start updating " + type);
      })
      .onEnd([=]() {
        Serial.println("\nEnd");
        isUpdating_ = false;
      })
      .onProgress([](unsigned int progress, unsigned int total) { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([=](ota_error_t error) {
        isUpdating_ = false;
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();
}

//------------------------------------------------------------------------------
void OTA::handle()
//------------------------------------------------------------------------------
{
  ArduinoOTA.handle();
}

//------------------------------------------------------------------------------
bool OTA::isUpdating()
//------------------------------------------------------------------------------
{
  return isUpdating_;
}