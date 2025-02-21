#pragma once

#include <WString.h>

String FormatString(const __FlashStringHelper* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  const auto size = vsnprintf_P(nullptr, 0, reinterpret_cast<const char*>(fmt), args);
  va_end(args);

  reserve(size + 1);
  va_start(args, fmt);
  len = vsnprintf_P(buffer, size + 1, reinterpret_cast<const char*>(fmt), args);
  va_end(args);
}