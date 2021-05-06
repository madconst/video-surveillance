#include "inc/exception.h"
#include "inc/decoder.h"
#include "inc/common.h"

Decoder::Decoder(const AVCodecParameters* codec_params)
{
  AVCodec* codec = avcodec_find_decoder(codec_params->codec_id);
  if (!codec) {
    throw Exception("Decoder not found");
  }
  codec_ctx_ = avcodec_alloc_context3(codec);
  if (!codec_ctx_) { // this is probably not required due to `avcodec_open2`
    throw Exception("Failed to allocate codec context");
  }
  if (avcodec_open2(codec_ctx_, codec, nullptr/*opts*/) < 0) {
    avcodec_free_context(&codec_ctx_);
    throw Exception("Failed to open codec context");
  }
}
Decoder::~Decoder()
{
  avcodec_free_context(&codec_ctx_);
}
void Decoder::put(Packet packet)
{
  int ret = avcodec_send_packet(codec_ctx_, packet.get());
  if (ret < 0) {
    throw Exception(av_error(ret));
  }
  while (true) {
    Frame frame;
    ret = avcodec_receive_frame(codec_ctx_, frame.get());
    if (ret == AVERROR(EAGAIN)) {
      break;
    }
    if (ret < 0) {
      throw Exception(av_error(ret));
    }
    frames_.push(std::move(frame));
  }
}
bool Decoder::has_output() const
{
  return frames_.size();
}
Frame Decoder::get()
{
  if (frames_.empty()) {
    throw Exception("No frames available in decoder");
  }
  auto frame = std::move(frames_.front());
  frames_.pop();
  return frame;
}
