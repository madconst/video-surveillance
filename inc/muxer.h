#ifndef MUXER_H
#define MUXER_H

extern "C" {
  #include <libavformat/avformat.h>
}
#include "inc/output.h"
#include "inc/packet.h"

class Muxer
{
public:
  Muxer(const std::string& uri, const std::string& format = "");
  ~Muxer();
  void add_stream(const AVCodecParameters* params);
  void write(Packet packet);
private:
  AVFormatContext* ctx_;
  bool header_written_ = false;
  Output output_;
};

#endif // MUXER_H
