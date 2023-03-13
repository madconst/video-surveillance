#ifndef AV__OUTPUT_H
#define AV__OUTPUT_H

#include <string>
extern "C" {
  #include <libavformat/avio.h>
}

namespace AV {

class Output
{
public:
  explicit Output(const std::string& url);
  Output(Output&& other);
  ~Output();
  Output& operator=(Output&& other);
private:
  AVIOContext* ctx_ = nullptr;
  friend class Muxer;
};

} // namespace AV

#endif // AV__OUTPUT_H
