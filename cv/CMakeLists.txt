add_library(cv motion_detector.cpp common.cpp)

find_package(OpenCV REQUIRED)

target_include_directories(cv PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" ${fmt_INCLUDE_DIR} ${OpenCV_INCLUDE_DIR})

target_link_libraries(cv
        PRIVATE common
        ${OpenCV_LIBS}
        fmt::fmt
)
