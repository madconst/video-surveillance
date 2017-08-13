#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <chrono>
#include "motion_estimator.h"

class MotionDetector
{
public:
  bool operator()(const cv::Mat& frame);
  cv::Mat diff();
private:
  MotionEstimator estimator_;
  std::chrono::steady_clock::time_point last_detected_;
};

#endif // MOTION_DETECTOR_H
