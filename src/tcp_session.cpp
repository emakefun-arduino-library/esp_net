#include "tcp_session.h"

#include "util/format_string.h"
#include "util/stream_util.h"

#define LOG_ENABLE (0)
#include "util/log.h"

namespace em {

TcpSession::TcpSession(Stream& stream, const String& remote, const uint16_t remote_port)
    : Session(stream), remote_(remote), remote_port_(remote_port) {
}

TcpSession::~TcpSession() {
}

bool TcpSession::Open() {
  CLOG();
  const auto cmd = FormatString(F("AT+CIPSTARTEX=\"TCP\",\"%s\",%" PRIu16), remote_.c_str(), remote_port_);
  stream_.println(cmd);
  const String targets[] = {
      F("0,CONNECT\r\n"),
      F("1,CONNECT\r\n"),
      F("2,CONNECT\r\n"),
      F("3,CONNECT\r\n"),
      F("\r\nERROR\r\n"),
  };
  const auto index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 10000);
  CLOG("%s: %" PRId16, cmd.c_str(), index);
  if (index >= 0 && index < kMaxLinkNum && FindUtil(stream_, "\r\nOK\r\n", 1000)) {
    link_id_ = index;
    return true;
  } else {
    link_id_ = 0xFF;
    return false;
  }
}

};  // namespace em