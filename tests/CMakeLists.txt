include(ExternalProject)
find_package(Git REQUIRED)

# Catch
ExternalProject_Add(
  catch
  PREFIX ${CMAKE_BINARY_DIR}/catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  TIMEOUT 10
  UPDATE_COMMAND ${GIT_EXECUTABLE} pull
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
)

set(CATCH_INCLUDE_DIR ${CMAKE_BINARY_DIR}/catch2)
add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE ${CATCH_INCLUDE_DIR}/src/catch/single_include/)

add_library(main ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(main Catch2)

foreach(test frame scaler)
  add_executable(${test}.test ${CMAKE_CURRENT_SOURCE_DIR}/frame.test.cpp)
  target_link_libraries(${test}.test main
    surveillance
    avformat avfilter avdevice avcodec avutil swscale
    ${OpenCV_LIBS}
    pthread
  )
  add_test(${test} ${test}.test)
endforeach(test)
