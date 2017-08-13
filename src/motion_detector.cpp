#include "inc/motion_detector.h"

bool MotionDetector::operator()(const cv::Mat& frame)
{
  using namespace std::chrono_literals;
  double diff = estimator_(frame);
  if (diff > 2.0) {
    last_detected_ = std::chrono::steady_clock::now();
  }
  return std::chrono::steady_clock::now() - last_detected_ < 1s;
}
cv::Mat MotionDetector::diff()
{
  return estimator_.diff();
}
