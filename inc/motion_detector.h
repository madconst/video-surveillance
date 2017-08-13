#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <chrono>
#include "motion_estimator.h"

using namespace std::chrono_literals;

class MotionDetector
{
public:
  explicit MotionDetector(std::chrono::seconds threshold = 3s);
  bool operator()(const cv::Mat& frame);
  cv::Mat diff();
private:
  MotionEstimator estimator_;
  std::chrono::steady_clock::time_point last_detected_;
  const std::chrono::seconds threshold_;
};

#endif // MOTION_DETECTOR_H
