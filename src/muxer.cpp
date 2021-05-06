#include "inc/exception.h"
#include "inc/muxer.h"
#include "inc/common.h"

Muxer::Muxer(const std::string& uri, const std::string& format) :
  output_(Output{uri})
{
  ctx_ = avformat_alloc_context();
  if (!ctx_) {
    throw Exception("Failed to allocate muxer context");
  }
  ctx_->oformat = av_guess_format(
    format.empty() ? nullptr : format.c_str(), // short format name
    uri.c_str(), // filename
    nullptr/*mime*/);
  if (!ctx_->oformat) {
    avformat_free_context(ctx_);
    throw Exception("Failed to determine output format");
  }
  ctx_->pb = output_.ctx_;
}
Muxer::~Muxer()
{
  if (header_written_) {
    av_write_trailer(ctx_);
    // TODO: check result and log error
  }
  avformat_free_context(ctx_);
}
void Muxer::add_stream(const AVCodecParameters* params)
{
  auto stream = avformat_new_stream(ctx_, nullptr);
  if (!stream) {
    throw Exception("Failed to create stream");
  }
  stream->time_base = { 1, 90'000 }; // AV_TIME_BASE_Q;
  stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  stream->codecpar->codec_id = params->codec_id;
  stream->codecpar->width = params->width;
  stream->codecpar->height = params->height;
}
void Muxer::write(Packet packet)
{
  if (!header_written_) {
    int result = avformat_write_header(ctx_, nullptr /* options */);
    if (result < 0) {
      throw Exception("Failed to write header: " + av_error(result));
    }
    header_written_ = true;
  }
  av_interleaved_write_frame(ctx_, packet.get());
}
