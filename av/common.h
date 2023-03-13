#ifndef AV__COMMON_H
#define AV__COMMON_H

#include <string>

extern "C" {
  #include <libavutil/avutil.h>
}

namespace AV {

void init_ffmpeg();
std::string errnum_to_string(int errnum);
std::string media_type_to_string(::AVMediaType type);

} // AV namespace

#endif // AV__COMMON_H
