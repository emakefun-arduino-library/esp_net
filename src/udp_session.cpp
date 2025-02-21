#include "udp_session.h"

#include "at.h"
#include "util/format_string.h"
#include "util/log.h"
#include "util/stream_util.h"

namespace em {

UdpSession::UdpSession(Stream& stream, const String& remote, const uint16_t remote_port, const uint16_t local_port)
    : Session(stream), remote_(remote), remote_port_(remote_port), local_port_(local_port) {
}

UdpSession::~UdpSession() {
}

bool UdpSession::Open() {
  CLOG();
  net::IpAddress local_ip;
  if (kOK != at::LocalIpAddress(stream_, &local_ip)) {
    return false;
  }
  stream_.println(FormatString(F("AT+CIPSTARTEX=\"UDP\",\"%s\",%" PRIu16 ",%" PRIu16 ",0,\"%s\""),
                               remote_.c_str(),
                               remote_port_,
                               local_port_,
                               local_ip.ToString().c_str()));
  const String targets[] = {
      F("0,CONNECT\r\n"),
      F("1,CONNECT\r\n"),
      F("2,CONNECT\r\n"),
      F("3,CONNECT\r\n"),
      F("\r\nERROR\r\n"),
  };
  const auto index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 10000);
  if (index >= 0 && index < kMaxLinkNum && FindUtil(stream_, "\r\nOK\r\n", 1000)) {
    link_id_ = index;
    return true;
  } else {
    link_id_ = 0xFF;
    return false;
  }
}

};  // namespace em