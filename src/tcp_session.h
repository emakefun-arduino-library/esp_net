#pragma once

#include <Arduino.h>
#include <WString.h>
#include <stdint.h>

#include "session.h"

namespace em {
class TcpSession : public Session {
 public:
  TcpSession(Stream& stream, const String& remote, const uint16_t remote_port);
  ~TcpSession() override;

  bool Open() override;

 private:
  const String remote_;
  const uint16_t remote_port_ = 0;
};
}  // namespace em
