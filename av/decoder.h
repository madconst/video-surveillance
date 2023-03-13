#ifndef AV__DECODER_H
#define AV__DECODER_H

#include <queue>
extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
#include "packet.h"
#include "frame.h"

namespace AV {

class Decoder
{
public:
  explicit Decoder(const AVCodecParameters* codec_params);
  ~Decoder();
  void put(Packet packet);
  Frame get();
  bool has_output() const;
private:
  AVCodecContext* codec_ctx_;
  std::queue<Frame> frames_;
};

} // namespace AV

#endif // AV__DECODER_H
