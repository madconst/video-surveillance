add_executable(app
  app.cpp
)

target_link_libraries(app
  surveillance
  avformat
  avdevice
  avcodec
  avutil
  ${OpenCV_LIBS}
  pthread
)
