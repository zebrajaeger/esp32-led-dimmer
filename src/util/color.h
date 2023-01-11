#pragma once

#include <Arduino.h>

uint8_t rgbToWhite(uint16_t r, uint16_t g, uint16_t b) {
  r = r * 30;  // 255 = 7650
  g = g * 59;  // 255 = 15045
  b = b * 11;  // 255 = 2805
  return (r + g + b) / 100;
}