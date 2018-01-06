find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(netcam
  netcam.cpp
)

target_link_libraries(netcam
  surveillance
  avformat
  avdevice
  avcodec
  avutil
  ${OpenCV_LIBS}
  ${Boost_LIBRARIES}
  pthread
)

add_executable(keyframe
  keyframe.cpp
)

target_link_libraries(keyframe
  surveillance
  avformat
  avdevice
  avcodec
  avutil
  ${OpenCV_LIBS}
  ${Boost_LIBRARIES}
)