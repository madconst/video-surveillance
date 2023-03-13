#include <fstream>
#include <ctime>
extern "C" {
  #include <libavutil/error.h>
  #include <libavutil/avutil.h>
  #include <libavformat/avio.h>
  #include <libavdevice/avdevice.h>
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
}
#include "common.h"

namespace AV
{

void init_ffmpeg()
{
  avdevice_register_all();
  avformat_network_init();
}

std::string errnum_to_string(int errnum)
{
  char buff[AV_ERROR_MAX_STRING_SIZE];
  return av_make_error_string(buff, AV_ERROR_MAX_STRING_SIZE, errnum);
}

std::string media_type_to_string(::AVMediaType type)
{
  return ::av_get_media_type_string(type);
}

} // namespace AV
