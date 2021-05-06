#ifndef DECODER_H
#define DECODER_H

#include <queue>
extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
#include "inc/packet.h"
#include "inc/frame.h"

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

#endif // DECODER_H
