extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavutil/imgutils.h>
}
#include "inc/exception.h"
#include "inc/common.h"
#include "inc/frame.h"

/** Allocates AVFrame or throws if failed */
static AVFrame* allocate()
{
  auto frame = av_frame_alloc();
  if (!frame) {
    throw Exception("Failed to allocate AVFrame");
  }
  return frame;
}

Frame::Frame()
  : frame_(allocate())
{}
Frame::Frame(size_t width, size_t height, AVPixelFormat pixel_format /*= AV_PIX_FMT_RGB24*/)
  : frame_(allocate())
{
  frame_->width = width;
  frame_->height = height;
  frame_->format = pixel_format;
  int ret = av_frame_get_buffer(frame_, 32 /*align*/);
  if (ret < 0) {
    throw Exception("Failed to allocate memory for frame data");
  }
}
Frame::Frame(const AVFrame& other) :
  frame_(av_frame_clone(&other))
{
  if (!frame_) {
    throw Exception("Failed to copy AVFrame");
  }
}
Frame::Frame(AVFrame&& frame) :
  frame_(allocate())
{
  av_frame_move_ref(frame_, &frame);
}
Frame::Frame(const Frame& other) :
  frame_(av_frame_clone(other.frame_))
{
  if (!frame_) {
    throw Exception("Failed to copy AVFrame");
  }
}
Frame::Frame(Frame&& other) noexcept :
  frame_(allocate())
{
  av_frame_move_ref(frame_, other.frame_);
}
Frame::~Frame()
{
  clear();
}
Frame& Frame::operator=(const Frame& other)
{
  if (&other == this) {
    return *this;
  }
  clear();
  frame_ = av_frame_clone(other.frame_);
  return *this;
}
Frame& Frame::operator=(Frame&& other) noexcept
{
  if (&other == this) {
    return *this;
  }
  clear();
  frame_ = allocate();
  av_frame_move_ref(frame_, other.frame_);
}
void Frame::clear()
{
  av_frame_unref(frame_);
  av_frame_free(&frame_);
}
AVFrame* Frame::get()
{
  return frame_;
}
const AVFrame* Frame::get() const
{
  return frame_;
}
AVFrame* Frame::operator->()
{
  return frame_;
}
const AVFrame* Frame::operator->() const
{
  return frame_;
}