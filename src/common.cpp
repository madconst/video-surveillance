#include <fstream>
#include <ctime>
extern "C" {
  #include <libavutil/error.h>
}
#include "inc/encoder.h"
#include "inc/common.h"

void init_ffmpeg()
{
  avdevice_register_all();
  avformat_network_init();
}

std::string av_error(int errnum)
{
  char buff[AV_ERROR_MAX_STRING_SIZE];
  return av_make_error_string(buff, AV_ERROR_MAX_STRING_SIZE, errnum);
}

std::string date_time(const std::string& format)
{
  time_t     now = time(0);
  struct tm  tstruct;
  char       buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), format.c_str(), &tstruct);
  return buf;
}

cv::Mat av2cv(AVFrame *frame)
{
  return cv::Mat(
    frame->height, frame->width,
    CV_8UC1, // grayscale
    frame->data[0],
    frame->linesize[0]);
}

void save_jpeg(const Frame& frame, const std::string& filename)
{
  AVCodecParameters params{};
  params.width = frame->width;
  params.height = frame->height;
  params.format = AV_PIX_FMT_YUVJ420P;
  Encoder jpeg_encoder("mjpeg", &params);
  // save frame as JPEG:
  jpeg_encoder.put(frame);
  auto jpeg_packet = jpeg_encoder.get();
  std::string filename_part = filename + ".part";
  std::ofstream jpeg_file(filename_part, std::ios::out | std::ios::binary);
  jpeg_file.write(reinterpret_cast<char*>(jpeg_packet.get()->data), jpeg_packet.get()->size);
  jpeg_file.close();
  rename(filename_part.c_str(), filename.c_str());
}
