#pragma once

#include <WString.h>

class FormatString : public String {
 public:
  FormatString(const __FlashStringHelper* fmt, ...) : String() {
    va_list args;
    va_start(args, fmt);
    const auto size = vsnprintf_P(nullptr, 0, reinterpret_cast<const char*>(fmt), args);
    va_end(args);

    reserve(size + 1);
    va_start(args, fmt);
    len = vsnprintf_P(buffer, size + 1, reinterpret_cast<const char*>(fmt), args);
    va_end(args);
  }

  FormatString(const String& fmt, ...) : String() {
    va_list args;
    va_start(args, fmt);
    const auto size = vsnprintf(nullptr, 0, fmt.c_str(), args);
    va_end(args);

    reserve(size + 1);
    va_start(args, fmt);
    len = vsnprintf(buffer, size + 1, fmt.c_str(), args);
    va_end(args);
  }
};