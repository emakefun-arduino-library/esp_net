#pragma once

#include <Arduino.h>
#include <WString.h>
#include <stdint.h>

namespace em {
class Session {
 public:
  static constexpr uint8_t kMaxLinkNum = 4;
  struct ReceivedData {
    String remote_ip;
    uint16_t remote_port = 0;
    String data;
  };

  explicit Session(Stream& stream) : stream_(stream) {
  }

  virtual ~Session();

  virtual bool Open() = 0;

  virtual bool Close();

  bool Connected() const;

  uint8_t LinkId() const {
    return link_id_;
  }

  uint16_t Available() const;

  ReceivedData Read(const uint16_t length = UINT16_MAX) const;

  size_t Write(const String& data);

  size_t Write(const char* data, const size_t length);

 protected:
  Stream& stream_;
  uint8_t link_id_ = 0xFF;
};
}  // namespace em