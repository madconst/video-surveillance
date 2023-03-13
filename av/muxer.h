#ifndef AV__MUXER_H
#define AV__MUXER_H

extern "C" {

#include <libavformat/avformat.h>
}
#include "output.h"
#include "packet.h"

namespace AV {

class Muxer {
public:
  Muxer(const std::string &uri, const std::string &format = "");

  ~Muxer();

  void add_stream(const AVCodecParameters *params);

  void write(Packet packet);

private:
  AVFormatContext *format_;
  bool header_written_ = false;
  Output output_;
};

} // namespace AV

#endif // AV__MUXER_H

