cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(surveillance)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-fsanitize=address")

find_package(OpenCV REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})
add_subdirectory(app/)
add_subdirectory(src/)
