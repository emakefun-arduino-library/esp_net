#pragma once

#include <Arduino.h>
#include <WString.h>
#include <stdint.h>

#include "session.h"

namespace em {
class UdpSession : public Session {
 public:
  UdpSession(Stream& stream, const String& remote, const uint16_t remote_port, const uint16_t local_port);
  ~UdpSession() override;

  bool Open() override;

 private:
  const String remote_;
  const uint16_t remote_port_ = 0;
  const uint16_t local_port_ = 0;
};
}  // namespace em
