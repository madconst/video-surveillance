#ifndef AV__PACKET_H
#define AV__PACKET_H

#include <memory>
extern "C" {
  #include <libavcodec/avcodec.h>
}

namespace AV
{

class Packet
{
public:
  Packet(int size = 0);
  explicit Packet(const AVPacket& packet);
  Packet(Packet&& other) noexcept;
  Packet(const Packet& other) noexcept;
  ~Packet();
  Packet& operator=(Packet&& other) noexcept;
  Packet& operator=(const Packet& other) noexcept;
  void clear() noexcept;
  AVPacket* get() noexcept;
  const AVPacket* get() const noexcept;
  bool is_corrupt() const noexcept;
  bool is_keyframe() const noexcept;
  size_t stream_index() const noexcept;
private:
  AVPacket* packet_;
};

} // namespace AV

#endif // AV__PACKET_H
