#ifndef CV__MOTION_ESTIMATOR_H
#define CV__MOTION_ESTIMATOR_H

#include "opencv2/opencv.hpp"

namespace CV {

class MotionDetector
{
public:
  double operator()(const cv::Mat& frame); // must be grayscale
  cv::Mat diff() const;
private:
  const double threshold_ = 10.0;
  cv::Mat prev_;
  cv::Mat diff_;
};

} // namespace CV

#endif // CV__MOTION_ESTIMATOR_H
