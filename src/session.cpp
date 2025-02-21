#include "session.h"

#include "util/format_string.h"
#include "util/stream_util.h"

#define LOG_ENABLE (0)
#include "util/log.h"

namespace em {

Session::~Session() {
  Close();
}

bool Session::Close() {
  CLOG();
  const auto cmd = FormatString(F("AT+CIPCLOSE=%" PRIu8), link_id_);
  const String targets[] = {FormatString(F("%" PRIu8 ",CLOSED\r\n\r\nOK\r\n"), link_id_), F("\r\nERROR\r\n")};

  stream_.println(cmd);
  const auto index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 10000);
  CLOG("index:%" PRId16, index);
  if (index == 0) {
    link_id_ = 0xFF;
    return true;
  }
  return false;
}

bool Session::Connected() const {
  if (link_id_ >= kMaxLinkNum) {
    return false;
  }

  const String targets[] = {F("+CIPSTATE:"), F("\r\nERROR\r\n"), F("\r\nOK\r\n")};
  stream_.println("AT+CIPSTATE?");
  while (true) {
    const auto index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
    if (index == 0) {
      if (link_id_ == stream_.parseInt()) {
        return true;
      }
    } else {
      return false;
    }
  }
}

uint16_t Session::Available() const {
  uint16_t size = 0;
  const String targets[] = {F("+CIPRECVLEN:"), F("\r\nERROR\r\n")};
  stream_.println(F("AT+CIPRECVLEN?"));
  const auto index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  if (index != 0) {
    CLOG("ERROR");
    return 0;
  }

  for (uint8_t i = 0; i < link_id_; i++) {
    if (!stream_.find(',')) {
      return 0;
    }
  }
  size = stream_.parseInt();
  if (stream_.find("\r\nOK\r\n")) {
    return size;
  } else {
    return 0;
  }
}

Session::ReceivedData Session::Read(const uint16_t length) const {
  Session::ReceivedData received_data;
  stream_.println(FormatString(F("AT+CIPRECVDATA=%" PRIu8 ",%" PRIu16), link_id_, length));
  const String targets[] = {F("+CIPRECVDATA:"), F("\r\nERROR\r\n")};
  const auto index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 1000);
  // CLOG("index: %" PRId16, index);
  if (index != 0) {
    goto Fail;
  }

  uint16_t actual_length = stream_.parseInt();
  if (!SkipNext(stream_, ',') || !SkipNext(stream_, '"')) {
    goto Fail;
  }
  char remote_ip[16] = {0};
  stream_.readBytesUntil('"', remote_ip, sizeof(remote_ip) - 1);
  received_data.remote_ip = remote_ip;
  if (!SkipNext(stream_, ',')) {
    goto Fail;
  }
  received_data.remote_port = stream_.parseInt();
  if (!SkipNext(stream_, ',')) {
    goto Fail;
  }
  while (actual_length != received_data.data.length()) {
    char c = '\0';
    if (1 != stream_.readBytes(&c, 1)) {
      goto Fail;
    }
    received_data.data += c;
  }
  if (!stream_.find("\r\nOK\r\n")) {
    goto Fail;
  }
  // CLOG("received_data: %s, %u, %s", received_data.remote_ip.c_str(), received_data.remote_port, received_data.data.c_str());
  return received_data;
Fail:
  return Session::ReceivedData();
}

size_t Session::Write(const String& data) {
  return Write(data.c_str(), data.length());
}

size_t Session::Write(const char* data, const size_t length) {
  const auto cmd = FormatString(F("AT+CIPSEND=%" PRIu8 ",%u"), link_id_, length);
  // CLOG("%s", cmd.c_str());
  const String targets[] = {F("OK\r\n\r\n>"), F("ERROR\r\n")};
  stream_.println(cmd);
  auto index = FindUtil(stream_, targets, sizeof(targets) / sizeof(targets[0]), 5000);
  // CLOG("%s: %" PRId16, cmd.c_str(), index);
  if (index != 0) {
    return 0;
  }

  const size_t ret = stream_.write(data, length);
  if (FindUtil(stream_, F("\r\nSEND OK\r\n"), 5000)) {
    // CLOG("send OK");
    return ret;
  }

  return 0;
}

};  // namespace em