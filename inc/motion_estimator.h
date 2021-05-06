#ifndef MOTION_ESTIMATOR_H
#define MOTION_ESTIMATOR_H

#include "opencv2/opencv.hpp" // FIXME: more specific header

class MotionEstimator
{
public:
  double operator()(const cv::Mat& frame); // must be grayscale
  cv::Mat diff() const;
private:
  const double threshold_ = 10.0;
  cv::Mat prev_;
  cv::Mat diff_;
};

#endif // MOTION_ESTIMATOR_H
