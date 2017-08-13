#include "inc/packet.h"

Packet::Packet()
{
  packet_ = av_packet_alloc();
  av_init_packet(packet_);
}
Packet::Packet(const AVPacket& packet)
{
  packet_ = av_packet_clone(&packet);
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
  reset();
}
Packet& Packet::operator=(Packet&& other) noexcept
{
  if (&other != this) {
    reset();
    packet_ = other.packet_;
    other.packet_ = nullptr;
  }
  return *this;
}
Packet& Packet::operator=(const Packet& other) noexcept
{
  if (&other != this) {
    reset();
    packet_ = av_packet_clone(other.packet_);
  }
  return *this;
}
void Packet::reset() noexcept
{
  if (!packet_) {
    return;
  }
  av_packet_unref(packet_);
  av_packet_free(&packet_);
}
AVPacket* Packet::get() noexcept
{
  return packet_;
}
const AVPacket* Packet::get() const noexcept
{
  return packet_;
}
