#ifndef ENCODER_H
#define ENCODER_H

#include <queue>
extern "C"
{
  #include <libavcodec/avcodec.h>
}
#include "inc/packet.h"
#include "inc/frame.h"

class Encoder
{
public:
  Encoder(const std::string& codec_name, const AVCodecParameters* params);
  ~Encoder();
  Encoder& set_option(const std::string& key, const std::string& value, int flags = 0);
  Encoder& set_option(const std::string& key, int64_t value, int flags = 0);
  Encoder& open();
  void put(const Frame& frame);
  bool has_output() const;
  void flush();
  Packet get();
  AVCodecContext& ctx();
private:
  AVCodecContext* codec_ctx_;
  AVDictionary* options_ = nullptr;
  std::queue<Packet> out_queue_;
};

#endif // ENCODER_H
