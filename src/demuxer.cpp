#include <sstream>
#include "inc/common.h"
#include "inc/exception.h"
#include "inc/demuxer.h"

Demuxer::Demuxer(const std::string& format)
{
  if (format.empty()) {
    return;
  }
  in_format_ = av_find_input_format(format.c_str());
  if (!in_format_) {
    throw Exception("Failed to find format: " + format);
  }
}
Demuxer& Demuxer::set_option(const std::string& key, const std::string& value, int flags)
{
  av_dict_set(&options_, key.c_str(), value.c_str(), flags);
  return *this;
}
Demuxer& Demuxer::set_option(const std::string& key, int value, int flags)
{
  av_dict_set_int(&options_, key.c_str(), value, flags);
  return *this;
}
void Demuxer::open(const std::string& url)
{
  // check video source
  int result = avformat_open_input(&ctx_, url.c_str(), in_format_, &options_);
  if (result) {
    std::ostringstream what;
    what << "Failed to open '" << url << "'"
         << ": " << av_error(result);
    throw Exception(what.str());
  }
  // ctx_->flags |= AVFMT_FLAG_GENPTS;
  result = avformat_find_stream_info(ctx_, nullptr /* options */);
  if (result < 0) {
    std::ostringstream what;
    what << "Failed to find stream info: " << av_error(result);
    throw Exception(what.str());
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
  Packet packet;
  int result = av_read_frame(ctx_, packet.get());
  if (result) {
    throw Exception("Failed to read packet: " + av_error(result));
  }
  return packet;
}
const AVCodecParameters* Demuxer::stream_parameters(size_t index) const
{
  throw_if_not_open();
  if (!(index < ctx_->nb_streams)) {
    throw Exception("Stream " + std::to_string(index) + " not found");
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
  throw Exception("Demuxer is not opened");
}
