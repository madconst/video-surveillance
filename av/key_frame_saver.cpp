#include "key_frame_saver.h"
#include <fstream>
#include "common/exception.h"

namespace AV {

KeyFrameSaver::KeyFrameSaver(const std::string& filename, int width/* = -1*/, int height/* = -1*/) :
  filename_(filename + ".jpg"),
  width_(width),
  height_(height)
{}

void KeyFrameSaver::open(const AVCodecParameters *in_params, int flags/* = SWS_FAST_BILINEAR*/) {
  AVCodecParameters out_params{};
  out_params.width = width_ < 0 ? in_params->width : width_;
  out_params.height = height_ < 0 ? in_params->height : height_;
  out_params.format = AV_PIX_FMT_YUVJ420P;

  scale_
  .from(in_params->width, in_params->height, static_cast<AVPixelFormat>(in_params->format))
  .to(out_params.width, out_params.height, static_cast<AVPixelFormat>(out_params.format))
  .open(flags);

  encoder_ = std::make_unique<Encoder>("mjpeg", &out_params);
  encoder_->codec()->bit_rate = 16'000'000; // does this work?
  encoder_->codec()->time_base = AVRational{1, 25};
  encoder_->codec()->framerate = AVRational{25, 1};
  encoder_->set_option("qmin", 2);
  encoder_->set_option("qmax", 2);
  encoder_->open();
}

void KeyFrameSaver::push(Frame frame)
{
  if (!encoder_) {
    throw Common::Exception("JPEG encoder not open");
  }
  frame = scale_(frame);
  encoder_->put(frame);
  if (!encoder_->has_output()) {
    // TODO: warning
    return;
  }
  Packet packet = encoder_->get(); // in MJPEG encoder frame is encoded instantly
  std::filesystem::path temp_name = filename_;
  temp_name.replace_extension("part.jpg");
  std::ofstream file(temp_name, std::ios::out | std::ios::binary);
  file.write(reinterpret_cast<char*>(packet.get()->data), packet.get()->size);
  file.close();
  std::filesystem::rename(temp_name, filename_);
}

} // namespace AV
