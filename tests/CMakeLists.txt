Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2
)

FetchContent_MakeAvailable(Catch2)

foreach(test av_packet av_frame)
  add_executable(${test}.test ${test}.test.cpp)

  target_link_libraries(${test}.test PRIVATE Catch2::Catch2WithMain av common)

  target_compile_options(${test}.test PRIVATE -fsanitize=address)
  target_link_options(${test}.test PRIVATE -fsanitize=address)

  add_test(${test} ${test}.test)
endforeach(test)

