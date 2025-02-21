/**
 * @file esp_net.h
 */
#pragma once

#ifndef _EM_ESP_NET_H_
#define _EM_ESP_NET_H_

#include <WString.h>

#include "error_code.h"
#include "net_type.h"

namespace em {

class EspNet {
 public:
  explicit EspNet(Stream& stream) : stream_(stream) {
  }

  ErrorCode Init();

  ErrorCode ConnectWifi(const String& ssid, const String& password);

  ErrorCode LocalIpAddress(net::IpAddress* const ip_address);

  ErrorCode MacAddress(net::MacAddress* const mac_address);

  ErrorCode Ssid(String* const ssid);

  ErrorCode Bssid(net::MacAddress* const bssid);

 private:
  EspNet(const EspNet&) = delete;
  EspNet& operator=(const EspNet&) = delete;
  Stream& stream_;
};
}  // namespace em
#endif