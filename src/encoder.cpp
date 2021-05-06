#include <iostream>
#include "inc/common.h"
#include "inc/exception.h"
#include "inc/encoder.h"

Encoder::Encoder(const std::string& codec_name, const AVCodecParameters* params)
{
  AVCodec* codec = avcodec_find_encoder_by_name(codec_name.c_str());
  if (!codec) {
    throw Exception("Encoder not found: " + codec_name);
  }
  codec_ctx_ = avcodec_alloc_context3(codec);
  if (!codec_ctx_) {
    throw Exception("Failed to allocate encoder context");
  }
}
Encoder::~Encoder()
{
  avcodec_free_context(&codec_ctx_);
  av_dict_free(&options_);
}
Encoder& Encoder::set_option(const std::string& key, const std::string& value, int flags)
{
  int ret = av_dict_set(&options_, key.c_str(), value.c_str(), flags);
  if (ret < 0) {
    throw Exception("Failed to set option " + key + "=" + value + ": " + av_error(ret));
  }
  return *this;
}
Encoder& Encoder::set_option(const std::string& key, int64_t value, int flags)
{
  int ret = av_dict_set_int(&options_, key.c_str(), value, flags);
  if (ret < 0) {
    throw Exception("Failed to set option " + key + "=" + std::to_string(value) + ": " + av_error(ret));
  }
  return *this;
}
Encoder& Encoder::open()
{
  int result = avcodec_open2(codec_ctx_, codec_ctx_->codec, &options_);
  if (result < 0) {
    throw Exception("Failed to open encoder context: " + av_error(result));
  }
  AVDictionaryEntry *entry = nullptr;
  while (entry = av_dict_get(options_, "", entry, AV_DICT_IGNORE_SUFFIX)) {
    std::cerr << "Option \"" << entry->key << "\" has not been recognized by encoder" << std::endl;
  }
  return *this;
}
void Encoder::put(const Frame& frame)
{
  int ret = avcodec_send_frame(codec_ctx_, frame.get());
  if (ret < 0) {
    throw Exception("Error on pushing frame to encoder: " + av_error(ret));
  }
  while (true) {
    Packet packet;
    ret = avcodec_receive_packet(codec_ctx_, packet.get());
    if (ret == AVERROR(EAGAIN)) {
      break;
    }
    if (ret < 0) {
      throw Exception("Error on pulling packet from encoder: " + av_error(ret));
    }
    out_queue_.push(std::move(packet));
  }
}
bool Encoder::has_output() const
{
  return !out_queue_.empty();
}
void Encoder::flush()
{
  avcodec_send_frame(codec_ctx_, nullptr);
}
Packet Encoder::get()
{
  if (out_queue_.empty()) {
    throw Exception("Empty encoder output buffer");
  }
  Packet packet = std::move(out_queue_.front());
  out_queue_.pop();
  return packet;
}
AVCodecContext& Encoder::ctx()
{
  return *codec_ctx_;
}
