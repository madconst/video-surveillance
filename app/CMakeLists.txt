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
  pthread
)
