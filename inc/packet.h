#ifndef PACKET_H
#define PACKET_H

#include <memory>
extern "C" {
  #include <libavcodec/avcodec.h>
}

class Packet
{
public:
  Packet();
  explicit Packet(const AVPacket& packet);
  Packet(Packet&& other) noexcept;
  Packet(const Packet& other) noexcept;
  ~Packet();
  Packet& operator=(Packet&& other) noexcept;
  Packet& operator=(const Packet& other) noexcept;
  void reset() noexcept;
  AVPacket* get() noexcept;
  const AVPacket* get() const noexcept;
private:
  AVPacket* packet_;
};

#endif // PACKET_H
