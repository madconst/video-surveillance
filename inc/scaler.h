#ifndef SCALER_H
#define SCALER_H

extern "C" {
  #include <libswscale/swscale.h>
}
#include "inc/frame.h"

/* https://www.ffmpeg.org/doxygen/trunk/group__libsws.html#details */

class Scaler
{
public:
  Scaler() = default;
  ~Scaler();
  Scaler& from(int width, int height, AVPixelFormat pixel_format = AV_PIX_FMT_RGB24);
  Scaler& to(int width, int height, AVPixelFormat pixel_format = AV_PIX_FMT_RGB24);
  Scaler& open(int flags = SWS_FAST_BILINEAR);
  Frame operator()(const Frame& other);
private:
  SwsContext* context_ = nullptr;
  int src_width_ = -1;
  int src_height_ = -1;
  AVPixelFormat src_pixel_format_;
  int dst_width_ = -1;
  int dst_height_ = -1;
  AVPixelFormat dst_pixel_format_;
};

#endif // SCALER_H