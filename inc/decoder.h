#ifndef DECODER_H
#define DECODER_H

extern "C"
{
  #include <libavformat/avformat.h>
  #include <libavcodec/avcodec.h>
}
#include "inc/packet.h"

class Decoder
{
public:
  explicit Decoder(const AVCodecParameters* codec_params);
  ~Decoder();
  int put(Packet packet);
  int get(AVFrame* frame);
private:
  AVCodecContext* codec_ctx_;
};

#endif // DECODER_H
