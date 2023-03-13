#ifndef AV__FRAME_H
#define AV__FRAME_H

extern "C" {
  #include <libavutil/frame.h>
}

namespace AV {

class Frame
{
public:
  Frame();
  explicit Frame(size_t width, size_t height, AVPixelFormat pixel_format = AV_PIX_FMT_RGB24);
  explicit Frame(const AVFrame& frame);
  explicit Frame(AVFrame&& frame);
  Frame(const Frame& other);
  Frame(Frame&& other) noexcept;
  ~Frame();
  Frame& operator=(const Frame& other);
  Frame& operator=(Frame&& other) noexcept;
  AVFrame* get();
  const AVFrame* get() const;
  AVFrame* operator->();
  const AVFrame* operator->() const;
private:
  void clear() noexcept;
  AVFrame* frame_ = nullptr;
};

} // namespace AV

#endif // AV__FRAME_H