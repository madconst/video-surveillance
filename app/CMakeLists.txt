find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_executable(netcam
  netcam.cpp
)

target_link_libraries(netcam
  surveillance
  ${OpenCV_LIBS}
  ${Boost_LIBRARIES}
  ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY} ${SWSCALE_LIBRARY}
  pthread
)

add_executable(keyframe
  keyframe.cpp
)

target_link_libraries(keyframe
  surveillance
  ${OpenCV_LIBS}
  ${Boost_LIBRARIES}
  ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY} ${SWSCALE_LIBRARY}
)
