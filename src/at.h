#pragma once

#include <Stream.h>

#include "error_code.h"
#include "net_type.h"

namespace em {
namespace at {
ErrorCode LocalIpAddress(Stream& stream, net::IpAddress* const ip_address);
ErrorCode MacAddress(Stream& stream, net::MacAddress* const mac_address);
ErrorCode Ssid(Stream& stream, String* const ssid);
ErrorCode Bssid(Stream& stream, net::MacAddress* const bssid);
bool CancelSend(Stream& stream);
ErrorCode Restart(Stream& stream);
}  // namespace at
}  // namespace em