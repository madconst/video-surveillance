find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(nvr main.cpp)

target_link_libraries(nvr
        PRIVATE av cv common
        ${Boost_LIBRARIES}
        fmt::fmt
        ${OpenCV_LIBS}
)

target_include_directories(nvr PRIVATE ${fmt_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${OpenCV_INCLUDE_DIR})

target_compile_options(nvr PRIVATE -fsanitize=address)
target_link_options(nvr PRIVATE -fsanitize=address)

target_compile_options(nvr PRIVATE -fsanitize=address)
target_link_options(nvr PRIVATE -fsanitize=address)
