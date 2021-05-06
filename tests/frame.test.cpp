#include "inc/frame.h"
#include "catch.hpp"

TEST_CASE("Frame, create", "[frame][create]") {
  Frame frame(640, 480);
  REQUIRE(frame.get()->linesize[0] != 3 * 1920);
  REQUIRE(frame.get()->linesize[1] == 0);
}
TEST_CASE("Frame, copy", "[frame][copy]") {
  Frame a(640,480);
  REQUIRE(a.get()->linesize[0] == 3 * 640);
  Frame b(a);
  REQUIRE(a.get()->linesize[0] == 3 * 640);
  REQUIRE(b.get()->linesize[0] == 3 * 640);
  Frame c(1920, 1080);
  c = a;
  REQUIRE(a.get()->linesize[0] == 3 * 640);
  REQUIRE(b.get()->linesize[0] == 3 * 640);
  REQUIRE(c.get()->linesize[0] == 3 * 640);
}
TEST_CASE("Frame, move", "[frame][move]") {
  Frame a(640,480);
  REQUIRE(a.get()->linesize[0] == 3 * 640);
  Frame b(std::move(a));
  REQUIRE(a.get()->linesize[0] == 0);
  REQUIRE(b.get()->linesize[0] == 3 * 640);
  Frame c(1920, 1080);
  c = std::move(b);
  REQUIRE(a.get()->linesize[0] == 0);
  REQUIRE(b.get()->linesize[0] == 0);
  REQUIRE(c.get()->linesize[0] == 3 * 640);
}
