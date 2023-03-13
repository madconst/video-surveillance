#include "packet.h"
#include "exception.h"

namespace AV {

Packet::Packet(int size)
{
  packet_ = av_packet_alloc();
}
Packet::Packet(const AVPacket& packet)
{
  packet_ = ::av_packet_clone(&packet);
}
Packet::Packet(Packet&& other) noexcept :
  packet_(other.packet_)
{
  other.packet_ = nullptr;
}
Packet::Packet(const Packet& other) noexcept :
  packet_(av_packet_clone(other.packet_))
{}
Packet::~Packet()
{
  clear();
}
Packet& Packet::operator=(Packet&& other) noexcept
{
  if (&other == this) {
    return *this;
  }
  clear();
  packet_ = other.packet_;
  other.packet_ = nullptr;
  return *this;
}
Packet& Packet::operator=(const Packet& other) noexcept
{
  if (&other == this) {
    return *this;
  }
  clear();
  packet_ = av_packet_clone(other.packet_);
  return *this;
}
void Packet::clear() noexcept
{
  if (!packet_) {
    return;
  }
  ::av_packet_free(&packet_);
}
AVPacket* Packet::get() noexcept
{
  return packet_;
}
const AVPacket* Packet::get() const noexcept
{
  return packet_;
}
bool Packet::is_corrupt() const noexcept
{
  return packet_->flags & AV_PKT_FLAG_CORRUPT;
}
bool Packet::is_keyframe() const noexcept
{
  return packet_->flags & AV_PKT_FLAG_KEY;
}
size_t Packet::stream_index() const noexcept
{
  return packet_->stream_index;
}

} // namespace AV
