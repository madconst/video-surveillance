cmake_minimum_required(VERSION 3.22.0 FATAL_ERROR)

project(video-surveillance VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("Using CXX compiler ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")

include_directories(${CMAKE_SOURCE_DIR})

Include(FetchContent)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        9.1.0
)
FetchContent_MakeAvailable(fmt)

add_subdirectory(common/)
add_subdirectory(av/)
add_subdirectory(cv/)
add_subdirectory(apps/)

enable_testing()
add_subdirectory(tests/)
