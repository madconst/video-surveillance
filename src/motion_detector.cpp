#include "inc/motion_detector.h"

MotionDetector::MotionDetector(std::chrono::seconds time_threshold, double diff_threshold) :
  time_threshold_(time_threshold),
  diff_threshold_(diff_threshold)
{}
MotionState MotionDetector::operator()(const cv::Mat& frame)
{
  diff_ = estimator_(frame);
  if (diff_ > diff_threshold_) {
    last_detected_ = std::chrono::steady_clock::now();
    return MotionState::PRESENT;
  }
  if (std::chrono::steady_clock::now() - last_detected_ < time_threshold_) {
    return MotionState::SUSPENDED;
  }
  return MotionState::ABSENT;
}
double MotionDetector::diff() const
{
  return diff_;
}
cv::Mat MotionDetector::delta_image() const
{
  return estimator_.diff();
}
