/**
 * @file esp_net.cpp
 */
#include "esp_net.h"

// #include <Arduino.h>

#include "at.h"
#include "util/format_string.h"
#include "util/stream_util.h"

#define LOG_ENABLE (0)
#include "util/log.h"

namespace em {

ErrorCode EspNet::Init() {
  const auto ret = at::Restart(stream_);
  if (ret != kOK) {
    CLOG("restart failed");
    return ret;
  }
  const String targets[] = {F("\r\nOK\r\n"), F("\r\nERROR\r\n"), F("busy p...\r\n")};
  stream_.println(F("ATE0"));
  auto index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  CLOG("ATE0: %" PRId16, index);
  if (index != 0) {
    return ErrorCode::kError;
  }

  stream_.println(F("AT+CWINIT=1"));
  index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  CLOG("AT+CWINIT=1: %" PRId16, index);
  if (index != 0) {
    return ErrorCode::kError;
  }

  stream_.println(F("AT+CWMODE=1"));
  index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  CLOG("AT+CWMODE=1: %" PRId16, index);
  if (index != 0) {
    return ErrorCode::kError;
  }

  stream_.println(F("AT+CIPMUX=1"));
  index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  CLOG("AT+CIPMUX=1: %" PRId16, index);
  if (index != 0) {
    return ErrorCode::kError;
  }

  stream_.println(F("AT+CIPDINFO=1"));
  index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  CLOG("AT+CIPDINFO=1: %" PRId16, index);
  if (index != 0) {
    return ErrorCode::kError;
  }

  stream_.println(F("AT+CWAUTOCONN=0"));
  index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  CLOG("AT+CWAUTOCONN=0: %" PRId16, index);
  if (index != 0) {
    return ErrorCode::kError;
  }

  stream_.println(F("AT+CWDHCP=1,1"));
  index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  CLOG("AT+CWDHCP=1,1: %" PRId16, index);
  if (index != 0) {
    return ErrorCode::kError;
  }

  stream_.println(F("AT+CIPRECVTYPE=5,1"));
  index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  CLOG("AT+CIPRECVTYPE=5,1: %" PRId16, index);
  if (index != 0) {
    return ErrorCode::kError;
  }

  return kOK;
}

ErrorCode EspNet::ConnectWifi(const String& ssid, const String& password) {
  CLOG("ssid: %s, password: %s", ssid.c_str(), password.c_str());
  const String targets[] = {F("\r\nOK\r\n"), F("\r\nERROR\r\n"), F("busy p...\r\n")};
  auto cmd = FormatString(F("AT+CWJAP=\"%s\",\"%s\""), ssid.c_str(), password.c_str());
  stream_.println(cmd);
  const int16_t index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 20000);
  CLOG("%s: %" PRId16, cmd.c_str(), index);
  if (index == 1) {
    return ErrorCode::kError;
  } else if (index == 2) {
    return ErrorCode::kBusy;
  } else if (index < 0) {
    return ErrorCode::kTimedout;
  }

  return ErrorCode::kOK;
}

ErrorCode EspNet::LocalIpAddress(net::IpAddress* const ip_address) {
  return at::LocalIpAddress(stream_, ip_address);
}

ErrorCode EspNet::MacAddress(net::MacAddress* const mac_address) {
  return at::MacAddress(stream_, mac_address);
}

ErrorCode EspNet::Ssid(String* const ssid) {
  return at::Ssid(stream_, ssid);
}

ErrorCode EspNet::Bssid(net::MacAddress* const bssid) {
  return at::Bssid(stream_, bssid);
}
}  // namespace em