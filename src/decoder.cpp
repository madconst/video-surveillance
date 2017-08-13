#include "inc/exception.h"
#include "inc/decoder.h"

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
int Decoder::put(Packet packet)
{
  return avcodec_send_packet(codec_ctx_, packet.get());
}
int Decoder::get(AVFrame* frame)
{
  return avcodec_receive_frame(codec_ctx_, frame);
}
