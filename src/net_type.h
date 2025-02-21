#pragma once

#include <Arduino.h>
#include <WString.h>
#include <inttypes.h>

namespace em {
namespace net {
struct IpAddress {
  uint8_t bytes[4] = {0};
  String ToString() const {
    return String() + bytes[0] + '.' + bytes[1] + '.' + bytes[2] + '.' + bytes[3];
  }
};

struct MacAddress {
  uint8_t bytes[6] = {0};
  String ToString() const {
    char buffer[18];
    snprintf_P(buffer,
               sizeof(buffer),
               PSTR("%02x:%02x:%02x:%02x:%02x:%02x"),
               bytes[0],
               bytes[1],
               bytes[2],
               bytes[3],
               bytes[4],
               bytes[5]);
    return buffer;
  }
};
}  // namespace net
}  // namespace em
