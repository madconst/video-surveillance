#include "motion_detector.h"

namespace CV {

double MotionDetector::operator()(const cv::Mat& frame) // must be grayscale
{
  cv::Mat filtered;
  cv::GaussianBlur(frame, filtered, cv::Size{21, 21}, 0);
  if (prev_.empty()) {
    prev_ = filtered;
    return 0.0;
  }
  // compare frames:
  cv::absdiff(filtered, prev_, diff_);
  cv::swap(filtered, prev_);
  return cv::mean(diff_)[0];
}
cv::Mat MotionDetector::diff() const
{
  return diff_;
}

} // namespace CV
