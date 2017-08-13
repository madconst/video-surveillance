#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
extern "C" {
  #include <libavformat/avio.h>
}

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

#endif // OUTPUT_H
