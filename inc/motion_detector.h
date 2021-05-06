#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

#include <chrono>
#include "motion_estimator.h"

using namespace std::chrono_literals;

enum class MotionState { PRESENT, SUSPENDED, ABSENT };

class MotionDetector
{
public:
  explicit MotionDetector(std::chrono::seconds time_threshold = 3s, double diff_threshold = 5.0);
  MotionState operator()(const cv::Mat& frame);
  double diff() const;
  cv::Mat delta_image() const;
private:
  MotionEstimator estimator_;
  std::chrono::steady_clock::time_point last_detected_;
  const std::chrono::seconds time_threshold_;
  const double diff_threshold_;
  double diff_ = 0.0;
};

#endif // MOTION_DETECTOR_H
