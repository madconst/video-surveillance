#ifndef ENCODER_H
#define ENCODER_H

extern "C"
{
  // #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
#include "inc/common.h"
#include "inc/packet.h"
#include "inc/exception.h"

class Encoder
{
public:
  Encoder(const std::string& codec_name, const AVCodecParameters* params)
  {
    AVCodec* codec = avcodec_find_encoder_by_name (codec_name.c_str());
    if (!codec) {
      throw Exception("Encoder not found: " + codec_name);
    }
    codec_ctx_ = avcodec_alloc_context3(codec);
    if (!codec_ctx_) {
      throw Exception("Failed to allocate encoder context");
    }
    codec_ctx_->time_base = AV_TIME_BASE_Q;
    codec_ctx_->pix_fmt = static_cast<AVPixelFormat>(params->format);
    codec_ctx_->width = params->width;
    codec_ctx_->height = params->height;
    int result = avcodec_open2(codec_ctx_, codec, nullptr/*opts*/);
    if (result < 0) {
      avcodec_free_context(&codec_ctx_);
      throw Exception("Failed to open encoder context: " + av_error(result));
    }
  }
  int put(const AVFrame* frame)
  {
    return avcodec_send_frame(codec_ctx_, frame);
  }
  int get(Packet& packet)
  {
    return avcodec_receive_packet(codec_ctx_, packet.get());
  }
  ~Encoder()
  {
    avcodec_free_context(&codec_ctx_);
  }
private:
  AVCodecContext* codec_ctx_;
};

#endif // ENCODER_H
