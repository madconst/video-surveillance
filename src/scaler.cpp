#include "inc/exception.h"
#include "inc/scaler.h"

Scaler::~Scaler()
{
  if (context_) {
    sws_freeContext(context_);
  }
}
Scaler& Scaler::from(int width, int height, AVPixelFormat pixel_format /*= AV_PIX_FMT_RGB24*/)
{
  src_width_ = width;
  src_height_ = height;
  src_pixel_format_ = pixel_format;
  return *this;
}
Scaler& Scaler::to(int width, int height, AVPixelFormat pixel_format /*= AV_PIX_FMT_RGB24*/)
{
  dst_width_ = width;
  dst_height_ = height;
  dst_pixel_format_ = pixel_format;
  return *this;
}
Scaler& Scaler::open(int flags /*= SWS_FAST_BILINEAR*/)
{
  if (context_) {
    sws_freeContext(context_);
  }
  context_ = sws_getContext(
    src_width_, src_height_, src_pixel_format_, 
    dst_width_, dst_height_, dst_pixel_format_,
    flags, nullptr, nullptr, nullptr);
  if (!context_) {
    throw Exception("Failed to open scaler context");
  }
  return *this;
}
Frame Scaler::operator()(const Frame& other)
{
  if (!context_) {
    throw Exception("Scaler context is not open");
  }
  Frame result(dst_width_, dst_height_, dst_pixel_format_);
  int ret = sws_scale(context_, 
    other.get()->data, other.get()->linesize, /*?*/0, src_height_,
    result.get()->data, result.get()->linesize);
  // ret - height of the output slice
  // copy times:
  result.get()->pts = other.get()->pts;
  result.get()->pkt_dts = other.get()->pkt_dts;
  result.get()->pkt_duration = other.get()->pkt_duration;
  return result;
}
