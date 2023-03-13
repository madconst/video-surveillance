#ifndef COMMON__EXCEPTION_H
#define COMMON__EXCEPTION_H

#include <stdexcept>

namespace Common
{

class Exception: public std::runtime_error
{
public:
  Exception(const std::string& what);
};

} // namespace Common

#endif // COMMON__EXCEPTION_H
