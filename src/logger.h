#pragma once

#include <Arduino.h>
#include <Print.h>
#include <chrono>

#include "PriUint64.h"

#ifndef LOGGER_BUFFER_SIZE
#define LOGGER_BUFFER_SIZE 1024
#endif

class Logger : public Print {
 public:
  enum Loglevel_t { FATAL, ERROR, WARN, INFO, DEBUG, VERBOSE };

  Logger(const String& module);

  const void f(const char* msg, ...);
  const void e(const char* msg, ...);
  const void w(const char* msg, ...);
  const void i(const char* msg, ...);
  const void d(const char* msg, ...);
  const void v(const char* msg, ...);

  virtual size_t write(uint8_t c);

 private:
  const void printPrefix(Loglevel_t loglevel);

  const String module_;
  static const char* level_str_[6];
  char buffer[LOGGER_BUFFER_SIZE];
};