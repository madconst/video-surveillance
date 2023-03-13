#include "exception.h"
#include "fmt/core.h"
#include "common.h"

namespace AV {

Exception::Exception(int errnum):
  Common::Exception(errnum_to_string(errnum))
{}
Exception::Exception(const std::string& what, int errnum):
        Common::Exception(fmt::format("{}: {}", what, errnum_to_string(errnum)))
{}

} // AV namespace