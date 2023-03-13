#include "encoder.h"
#include "common.h"
#include "common/exception.h"
#include "fmt/format.h"
#include "exception.h"

namespace AV {

Encoder::Encoder(const std::string& codec_name, const AVCodecParameters* params/* = nullptr*/)
{
  AVCodec* codec = ::avcodec_find_encoder_by_name(codec_name.c_str());
  if (!codec) {
    throw Common::Exception("Encoder not found: " + codec_name);
  }
  codec_ = ::avcodec_alloc_context3(codec);
  if (!codec_) {
    throw Common::Exception("Failed to allocate encoder context");
  }
  if (params == nullptr) {
    return;
  }
  const auto ret = ::avcodec_parameters_to_context(codec_, params);
  if (ret < 0) {
    throw Exception("Failed to set codec context parameters", ret);
  }
}
Encoder::~Encoder()
{
  ::avcodec_free_context(&codec_);
  ::av_dict_free(&options_);
}
Encoder& Encoder::set_option(const std::string& key, const std::string& value, int flags)
{
  int ret = ::av_dict_set(&options_, key.c_str(), value.c_str(), flags);
  if (ret < 0) {
    throw Common::Exception(fmt::format("Failed to set option {} = {}: {}",
                                        key, value, errnum_to_string(ret)));
  }
  return *this;
}
Encoder& Encoder::set_option(const std::string& key, int64_t value, int flags)
{
  int ret = ::av_dict_set_int(&options_, key.c_str(), value, flags);
  if (ret < 0) {
    throw Common::Exception(fmt::format("Failed to set option {} = {}: {}",
                                        key, value, errnum_to_string(ret)));
  }
  return *this;
}
Encoder& Encoder::open()
{
  int ret = ::avcodec_open2(codec_, codec_->codec, &options_);
  if (ret < 0) {
    throw Exception("Failed to open encoder context", ret);
  }
  return *this;
}
void Encoder::put(const Frame& frame)
{
  int ret = ::avcodec_send_frame(codec_, frame.get());
  if (ret < 0) {
    throw Exception("Error on pushing frame to encoder", ret);
  }
  while (true) {
    Packet packet;
    ret = ::avcodec_receive_packet(codec_, packet.get());
    if (ret == AVERROR(EAGAIN)) {
      break;
    }
    if (ret < 0) {
      throw Exception("Error on pulling packet from encoder", ret);
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
  avcodec_send_frame(codec_, nullptr);
}
Packet Encoder::get()
{
  if (out_queue_.empty()) {
    throw Common::Exception("Empty encoder output buffer");
  }
  Packet packet = std::move(out_queue_.front());
  out_queue_.pop();
  return packet;
}
AVCodecContext* Encoder::codec()
{
  return codec_;
}
const AVCodecContext* Encoder::codec() const
{
  return codec_;
}

} // namespace AV
