#include "common.h"
#include "common/exception.h"
#include "exception.h"
#include "demuxer.h"
#include "fmt/format.h"

namespace AV
{

const Clock::duration Demuxer::READ_TIMEOUT = 3s;

Demuxer::Demuxer(const std::string& format)
{
  if (format.empty()) {
    return;
  }
  in_format_ = ::av_find_input_format(format.c_str());
  if (!in_format_) {
    throw Common::Exception("Failed to find format: " + format);
  }
}
Demuxer& Demuxer::set_option(const std::string& key, const std::string& value, int flags)
{
  ::av_dict_set(&options_, key.c_str(), value.c_str(), flags);
  return *this;
}
Demuxer& Demuxer::set_option(const std::string& key, int value, int flags)
{
  av_dict_set_int(&options_, key.c_str(), value, flags);
  return *this;
}
void Demuxer::open(const std::string& url)
{
  ctx_ = avformat_alloc_context();
  if (!ctx_) {
    throw Common::Exception("Failed to allocate demuxer context");
  }
  ctx_->interrupt_callback = { &Demuxer::interrupt_callback, this };
  // check video source
  int ret = avformat_open_input(&ctx_, url.c_str(), in_format_, &options_);
  if (ret) {
    throw Common::Exception(fmt::format("Failed to open {}: {}", url, errnum_to_string(ret)));
  }
  ctx_->flags |= AVFMT_FLAG_GENPTS;
  ret = avformat_find_stream_info(ctx_, nullptr /* options */);
  if (ret < 0) {
    throw Exception("Failed to find stream info", ret);
  }
}
Demuxer::~Demuxer()
{
  avformat_close_input(&ctx_);
  av_dict_free(&options_);
}
Packet Demuxer::read()
{
  throw_if_not_open();
  Packet packet{};
  last_read_ = Clock::now();
  int result = av_read_frame(ctx_, packet.get());
  if (result) {
    throw Common::Exception("Failed to read packet: " + AV::errnum_to_string(result));
  }
  return packet;
}
const AVCodecParameters* Demuxer::stream_parameters(size_t index) const
{
  throw_if_not_open();
  if (!(index < ctx_->nb_streams)) {
    throw Common::Exception("Stream " + std::to_string(index) + " not found");
  }
  return ctx_->streams[index]->codecpar;
}
size_t Demuxer::num_of_streams() const
{
  throw_if_not_open();
  return ctx_->nb_streams;
}
size_t Demuxer::video_stream_index() const
{
  throw_if_not_open();
  return av_find_best_stream(ctx_, AVMEDIA_TYPE_VIDEO,
    -1, // wanted stream: auto
    -1, // related stream: none
    nullptr, // decoder detection
    0 /* flags */);
}
void Demuxer::throw_if_not_open() const
{
  if (ctx_) return;
  throw Common::Exception("Demuxer is not opened");
}
int Demuxer::interrupt_callback(void* ptr)
{
  auto this_ = static_cast<Demuxer*>(ptr);
  if (Clock::now() - this_->last_read_ > READ_TIMEOUT) {
    return 1;
  }
  return 0;
}

} // namespace AV
