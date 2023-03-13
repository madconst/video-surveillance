#ifndef AV__ENCODER_H
#define AV__ENCODER_H

#include <queue>
extern "C"
{
  #include <libavcodec/avcodec.h>
}
#include "packet.h"
#include "frame.h"

namespace AV {

class Encoder
{
public:
  Encoder(const std::string& codec_name, const AVCodecParameters* params = nullptr);
  ~Encoder();
  Encoder& set_option(const std::string& key, const std::string& value, int flags = 0);
  Encoder& set_option(const std::string& key, int64_t value, int flags = 0);
  Encoder& open();
  void put(const Frame& frame);
  bool has_output() const;
  void flush();
  Packet get();
  AVCodecContext* codec();
  const AVCodecContext* codec() const;
private:
  AVCodecContext* codec_;
  AVDictionary* options_ = nullptr;
  std::queue<Packet> out_queue_;
};

} // namespace AV

#endif // AV__ENCODER_H
