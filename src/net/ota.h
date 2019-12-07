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

#include "Arduino.h"

#include "util/logger.h"

class OTA {
 public:
  typedef void (*StartEndCallback)();
  typedef void (*ProgressCallback)(double uploaded);

  OTA();
  bool begin();
  void loop();

  bool isUpdating();

  void onStart(StartEndCallback cb);
  void onEnd(StartEndCallback cb);
  void onProgress(ProgressCallback cb);

 private:
  Logger LOG;
  bool isUpdating_;
  StartEndCallback startCallback_;
  StartEndCallback endCallback_;
  ProgressCallback progressCallback_;
};