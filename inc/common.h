#ifndef COMMON_H
#define COMMON_H

#include <string>
#include "opencv2/opencv.hpp"
extern "C" {
  #include <libavformat/avio.h>
  #include <libavdevice/avdevice.h>
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
}
#include "inc/frame.h"

void init_ffmpeg();

std::string av_error(int errnum);

std::string date_time(const std::string& format = "%Y-%m-%d %H:%M:%S");

cv::Mat av2cv(AVFrame *frame);

void save_jpeg(const Frame& frame, const std::string& filename);

#endif // COMMON_H
