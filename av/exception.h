#ifndef AV__EXCEPTION_H
#define AV__EXCEPTION_H

#include "common/exception.h"

namespace AV {

class Exception: public Common::Exception
{
public:
  explicit Exception(int errnum);
  explicit Exception(const std::string& what, int errnum);
};

} // AV namespace

#endif // AV__EXCEPTION_H
