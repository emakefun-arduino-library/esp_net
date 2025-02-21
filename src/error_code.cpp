#include "error_code.h"

namespace em {
String ToString(ErrorCode error) {
  switch (error) {
    case kOK:
      return F("OK");
    case kError:
      return F("Error");
    case kBusy:
      return F("Busy");
    case kTimedout:
      return F("Timedout");
    case kInvalidParameters:
      return F("InvalidParameters");
    default:
      return F("Unknown Error Code");
  }
}
}  // namespace em