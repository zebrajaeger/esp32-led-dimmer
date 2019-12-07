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

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <SPIFFS.h>

//------------------------------------------------------------------------------
OTA::OTA()
    : LOG("OTA"),
      isUpdating_(false)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool OTA::begin()
//------------------------------------------------------------------------------
{
  ArduinoOTA
      .onStart([&]() {
        if (startCallback_) {
          startCallback_();
        }

        isUpdating_ = true;
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
          type = "sketch";
        } else {
          type = "filesystem";
          SPIFFS.end();
        }
        Serial.println("[OTA] Start updating " + type);
      })
      .onEnd([&]() {
        if (endCallback_) {
          endCallback_();
        }

        Serial.println("\nEnd");
        isUpdating_ = false;
      })
      .onProgress([&](unsigned int progress, unsigned int total) {
        if (progressCallback_) {
          progressCallback_((double)progress/(double)total);
        }

        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([=](ota_error_t error) {
        isUpdating_ = false;
        Serial.printf("[OTA] Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("[OTA] Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("[OTA] Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("[OTA] Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("[OTA] Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("[OTA] End Failed");
      });

  ArduinoOTA.begin();
  return true;
}

//------------------------------------------------------------------------------
void OTA::loop()
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

//------------------------------------------------------------------------------
void OTA::onStart(StartEndCallback cb)
//------------------------------------------------------------------------------
{}
//------------------------------------------------------------------------------
void OTA::onEnd(StartEndCallback cb)
//------------------------------------------------------------------------------
{}
//------------------------------------------------------------------------------
void OTA::onProgress(ProgressCallback cb)
//------------------------------------------------------------------------------
{}