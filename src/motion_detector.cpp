#include "inc/motion_detector.h"

MotionDetector::MotionDetector(std::chrono::seconds threshold) :
  threshold_(threshold)
{}
bool MotionDetector::operator()(const cv::Mat& frame)
{
  double diff = estimator_(frame);
  if (diff > 2.0) {
    last_detected_ = std::chrono::steady_clock::now();
  }
  return std::chrono::steady_clock::now() - last_detected_ < threshold_;
}
cv::Mat MotionDetector::diff()
{
  return estimator_.diff();
}
