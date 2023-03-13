#ifndef CV__COMMON_H
#define CV__COMMON_H

#include "opencv2/opencv.hpp"

namespace CV {

cv::Mat grayscale_from_raw_data(int width, int height, uint8_t* data, int row_size);

} // namespace CV

#endif // CV__COMMON_H
