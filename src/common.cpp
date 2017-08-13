#include "inc/common.h"

extern "C"
{
  #include <libavutil/error.h>
}

std::string av_error(int errnum)
{
  char buff[AV_ERROR_MAX_STRING_SIZE];
  return av_make_error_string(buff, AV_ERROR_MAX_STRING_SIZE, errnum);
}
