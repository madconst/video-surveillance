#ifndef AV__KEY_FRAME_SAVER_H
#define AV__KEY_FRAME_SAVER_H

#include <fstream>
#include <filesystem>
#include "scaler.h"
#include "encoder.h"
#include "common/exception.h"

namespace AV {

class KeyFrameSaver
{
public:
  KeyFrameSaver(const std::string& filename, int width = -1, int height = -1);
  void open(const AVCodecParameters* in_params, int flags = SWS_FAST_BILINEAR);
  void push(Frame frame);
private:
  const std::filesystem::path filename_;
  const int width_;
  const int height_;
  Scaler scale_;
  std::unique_ptr<Encoder> encoder_;
};

} // namespace AV

#endif // AV__KEY_FRAME_SAVER_H
