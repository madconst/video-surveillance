cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(surveillance)
set(CMAKE_CXX_STANDARD 17)

if(ENABLE_ASAN)
  set(CMAKE_CXX_FLAGS "-fsanitize=leak")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# FFmpeg libs
find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(SWSCALE_LIBRARY swscale)

include_directories(${AVFORMAT_INCLUDE_DIR} ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR})
# message("FFmpeg headers: ${AVFORMAT_INCLUDE_DIR}, ${AVCODEC_INCLUDE_DIR}, ${AVUTIL_INCLUDE_DIR}, ${AVDEVICE_INCLUDE_DIR}, ${SWSCALE_INCLUDE_DIR}")
# message("FFmpeg libs: ${AVFORMAT_LIBRARY}, ${AVCODEC_LIBRARY}, ${AVUTIL_LIBRARY}, ${AVDEVICE_LIBRARY}, ${SWSCALE_LIBRARY}")

include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(app/)
add_subdirectory(src/)

if(ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests/)
endif()
