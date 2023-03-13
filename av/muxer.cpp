#include "muxer.h"
#include "common/exception.h"
#include "exception.h"

namespace AV {

Muxer::Muxer(const std::string& uri, const std::string& format) :
  output_(Output{uri})
{
  format_ = ::avformat_alloc_context();
  if (!format_) {
    throw Common::Exception("Failed to allocate muxer context");
  }
  format_->oformat = av_guess_format(
    format.empty() ? nullptr : format.c_str(), // short format name
    uri.c_str(), // filename
    nullptr/*mime*/);
  if (!format_->oformat) {
    ::avformat_free_context(format_);
    throw Common::Exception("Failed to determine output format");
  }
  format_->pb = output_.ctx_;
}
Muxer::~Muxer()
{
  if (header_written_) {
    ::av_write_trailer(format_);
    // TODO: check result and log error
  }
  avformat_free_context(format_);
}
void Muxer::add_stream(const AVCodecParameters* params)
{
  auto stream = ::avformat_new_stream(format_, nullptr);
  if (!stream) {
    throw Common::Exception("Failed to create stream");
  }
  int ret = ::avcodec_parameters_copy(stream->codecpar, params);
  if (ret < 0) {
    throw Exception("Failed to copy stream parameters", ret);
  }
}
void Muxer::write(Packet packet)
{
  if (!header_written_) {
    int ret = ::avformat_write_header(format_, nullptr /* options */);
    if (ret < 0) {
      throw Exception("Failed to write header", ret);
    }
    header_written_ = true;
  }
  ::av_interleaved_write_frame(format_, packet.get());
}

} // namespace AV
