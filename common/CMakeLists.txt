add_library(common
        exception.cpp
)

target_include_directories(common PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")