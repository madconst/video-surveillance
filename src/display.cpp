#include "inc/display.h"

Display::Display()
{
  display_ = std::thread([&](){
    while (!stop_) {
      {
        std::lock_guard<std::mutex> lock(mutex_);
    	  if (!frame_.empty()) {
          imshow("Motion", frame_);
        }
    	}
      cv::waitKey(1);
    }
  });
}
Display::~Display()
{
  stop_ = true;
  if (display_.joinable()) {
    display_.join();
  }
}
void Display::operator()(const cv::Mat& frame)
{
  std::lock_guard<std::mutex> lock(mutex_);
  frame_ = frame;
}
