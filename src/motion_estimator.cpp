#include "inc/motion_estimator.h"

double MotionEstimator::operator()(const cv::Mat& frame) // must be grayscale
{
  cv::Mat filtered;
  // cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);
  cv::GaussianBlur(frame, filtered, cv::Size{21, 21}, 0);
  if (prev_.empty()) {
    prev_ = filtered;
    return 0.0;
  }
  // compare frames:
  cv::Mat diff;
  cv::absdiff(filtered, prev_, diff);
  auto value = cv::threshold(diff, diff, threshold_, 255, cv::THRESH_BINARY);
  diff_ = diff; // DELETEME
  cv::swap(filtered, prev_);
  return cv::mean(diff)[0];
}
cv::Mat MotionEstimator::diff() const
{
  return diff_;
}
