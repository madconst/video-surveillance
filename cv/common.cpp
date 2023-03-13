#include "common.h"

namespace CV {

cv::Mat grayscale_from_raw_data(int width, int height, uint8_t* data, int row_size) {
  return cv::Mat(height, width, CV_8UC1, data,row_size);
}

} // namespace CV
