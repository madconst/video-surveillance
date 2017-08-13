#ifndef DISPLAY_H
#define DISPLAY_H

#include <thread>
#include <mutex>
#include <atomic>
#include "opencv2/opencv.hpp"

class Display
{
public:
  Display();
  ~Display();
  void operator()(const cv::Mat& frame);
private:
  cv::Mat frame_;
  std::thread display_;
  std::mutex mutex_;
  std::atomic<bool> stop_{false};
};

#endif // DISPLAY_H
