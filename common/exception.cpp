#include "exception.h"
#include <stdexcept>
#include <sstream>

namespace Common
{

Exception::Exception(const std::string& what):
    std::runtime_error(what)
{}

} // namespace Common