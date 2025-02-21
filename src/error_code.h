#pragma once

#include <WString.h>
#include <stdint.h>

namespace em {
enum ErrorCode : uint8_t {
  kOK = 0,
  kError,
  kBusy,
  kTimedout,
  kInvalidParameters,
};

String ToString(ErrorCode error);
}  // namespace em