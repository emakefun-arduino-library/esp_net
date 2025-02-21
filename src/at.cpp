#include "at.h"

#include <Arduino.h>
#include <errno.h>

#include "util/log.h"
#include "util/stream_util.h"

namespace em {
namespace at {
ErrorCode LocalIpAddress(Stream& stream, net::IpAddress* const ip_address) {
  if (ip_address == nullptr) {
    return kInvalidParameters;
  }

  const String targets[] = {F("+CIFSR:STAIP,\""), F("\r\nERROR\r\n"), F("busy p...\r\n")};
  stream.println(F("AT+CIFSR"));
  const int16_t index = FindUtil(stream, targets, sizeof(targets) / sizeof(targets[0]), 1000);

  if (index == 1) {
    return kError;
  } else if (index == 2) {
    return kBusy;
  } else if (index < 0) {
    return kTimedout;
  }

  uint8_t pos = 0;
  do {
    ip_address->bytes[pos++] = stream.parseInt();
  } while (pos < sizeof(ip_address->bytes) && stream.find('.'));

  if (pos != sizeof(ip_address->bytes)) {
    CLOG();
    goto Error;
  }

  if (!FindUtil(stream, F("\r\nOK\r\n"), 1000)) {
    goto Error;
  }

  return kOK;
Error:
  *ip_address = net::IpAddress();
  return kError;
}

ErrorCode MacAddress(Stream& stream, net::MacAddress* const mac_address) {
  if (mac_address == nullptr) {
    return kInvalidParameters;
  }

  const String targets[] = {F("+CIFSR:STAMAC,\""), F("\r\nERROR\r\n"), F("busy p...\r\n")};
  stream.println(F("AT+CIFSR"));
  const int16_t index = FindUtil(stream, targets, sizeof(targets) / sizeof(targets[0]));

  if (index == 1) {
    return kError;
  } else if (index == 2) {
    return kBusy;
  } else if (index < 0) {
    return kTimedout;
  }

  char data[18] = {0};
  if (sizeof(data) - 1 != stream.readBytesUntil('\"', data, sizeof(data) - 1)) {
    return kError;
  }

  if (!FindUtil(stream, F("\r\nOK\r\n"), 1000)) {
    goto Error;
  }

  for (uint8_t i = 0; i < sizeof(net::MacAddress::bytes); i++) {
    mac_address->bytes[i] = strtol(data + i * 3, nullptr, 16);
    if (errno != 0) {
      goto Error;
    }
  }

  return kOK;

Error:
  *mac_address = net::MacAddress();
  return kError;
}

ErrorCode Ssid(Stream& stream, String* const ssid) {
  stream.println(F("AT+CWJAP?"));
  const String targets[] = {F("+CWJAP:\""), F("\r\nERROR\r\n")};
  const int16_t index = FindUtil(stream, targets, sizeof(targets) / sizeof(targets[0]), 1000, true);
  if (index == 1) {
    return kError;
  } else if (index == 2) {
    return kBusy;
  } else if (index < 0) {
    return kTimedout;
  }

  *ssid = stream.readStringUntil('\"');
  if (FindUtil(stream, F("\r\nOK\r\n"), 100)) {
    return kOK;
  } else {
    EmptyRx(stream, 100);
    return kError;
  }
}

ErrorCode Bssid(Stream& stream, net::MacAddress* const bssid) {
  stream.println(F("AT+CWJAP?"));
  const String targets[] = {F("+CWJAP:\""), F("\r\nERROR\r\n"), F("busy p...\r\n")};
  const int16_t index = FindUtil(stream, targets, sizeof(targets) / sizeof(targets[0]));
  if (index == 1) {
    return kError;
  } else if (index == 2) {
    return kBusy;
  } else if (index < 0) {
    return kTimedout;
  }

  if (!FindUtil(stream, "\",\"")) {
    return kError;
  }

  char data[18] = {0};
  if (sizeof(data) - 1 != stream.readBytesUntil('\"', data, sizeof(data) - 1)) {
    return kError;
  }

  for (uint8_t i = 0; i < sizeof(net::MacAddress::bytes); i++) {
    bssid->bytes[i] = strtol(data + i * 3, nullptr, 16);
    if (errno != 0) {
      goto Error;
    }
  }

  return kOK;

Error:
  EmptyRx(stream, 100);
  *bssid = net::MacAddress();
  return kError;
}

bool CancelSend(Stream& stream) {
  stream.print(F("+++"));
  if (!FindUtil(stream, F("\r\nSEND Canceled\r\n"), 100)) {
    stream.println();
    EmptyRx(stream, 100);
    return false;
  } else {
    return true;
  }
}

ErrorCode Restart(Stream& stream) {
  const auto start_time = millis();
  do {
    stream.println(F("AT+RST"));
    CLOG("AT+RST");
    const String targets[] = {F("\r\nOK\r\n"), F("\r\nERROR\r\n"), F("busy p...\r\n")};
    const int16_t index = FindUtil(stream, targets, sizeof(targets) / sizeof(targets[0]), 100);
    if (index == 0 && FindUtil(stream, "\r\nready\r\n", 2000)) {
      stream.println(F("AT"));
      if (0 == FindUtil(stream, targets, sizeof(targets) / sizeof(targets[0]), 100)) {
        return kOK;
      }
    } else {
      CancelSend(stream);
    }
  } while (millis() - start_time < 5000);

  return kError;
}

}  // namespace at
}  // namespace em