#ifndef RECORDER_H
#define RECORDER_H

#include "inc/packet.h"
#include "inc/muxer.h"

class Recorder
{
public:
  Recorder()
  {}
  void add_stream(const AVCodecParameters* params)
  {
    stream_parameters_.push_back(params);
  }
  void push(Packet packet)
  {
    if (muxer_ && !suspended_) {
      muxer_->write(std::move(packet));
    } else {
      queue_.push(std::move(packet));
    }
  }
  void start_recording(const std::string& filename)
  {
    muxer_ = std::make_unique<Muxer>(filename);
    for (auto p: stream_parameters_) {
      muxer_->add_stream(p);
    }
    suspended_ = false;
    flush();
  }
  void suspend()
  {
    suspended_ = true;
  }
  void resume()
  {
    suspended_ = false;
    flush();
  }
  void stop_recording()
  {
    muxer_.reset();
  }
  bool recording() const
  {
    return !!muxer_;
  }
  bool suspended() const
  {
    return suspended_;
  }
  void flush()
  {
    while (!queue_.empty()) {
      if (muxer_) {
        muxer_->write(queue_.front());
      }
      queue_.pop();
    }
  }
private:
  std::vector<const AVCodecParameters*> stream_parameters_;
  std::queue<Packet> queue_;
  std::unique_ptr<Muxer> muxer_;
  bool suspended_;
};

#endif // RECORDER_H
