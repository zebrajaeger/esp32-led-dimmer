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

#include <Arduino.h>
// #include <ArtnetWifi.h>
#include <ESPAsyncE131.h>
#include <stdio.h>

#include <functional>

#include "util/logger.h"

#define UNIVERSE_COUNT 1  // Total number of Universes to listen for, starting at UNIVERSE

// thats why i hate c++
// many thx to https://stackoverflow.com/questions/1000663/using-a-c-class-member-function-as-a-c-callback-function/56943930#56943930
template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
  template <typename... Args>
  static Ret callback(Args... args) {
    return func(args...);
  }
  static std::function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;

/**
 * https://en.wikipedia.org/wiki/Art-Net
 */
class Artnet {
 public:
  // enum messageType_t { PERMANENT = 0, TEMPORARY = 1, RESET = 2 };

  typedef void (*callback_t)(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);
  typedef void (*DataCallback)(uint8_t* data, uint16_t length);
  typedef void (*TimeoutCallback)();

  Artnet();
  bool begin(uint16_t universe, uint64_t timeoutUs = 5000*1000); // default timeout: 5s
  void loop();
  void onData(DataCallback cb);
  void onTimeout(TimeoutCallback cb);

 protected:
  // void onArtnetData(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);

 private:
  Logger LOG;
  // ArtnetWifi artnet_;
  ESPAsyncE131 e131;
  // uint16_t universe_;

  uint64_t timeoutUs_;
  uint64_t lastPaketTimestampUs_;

  DataCallback dataCallback_;
  TimeoutCallback timeoutCallback_;
};