#include <catch2/catch_test_macros.hpp>
#include <av/frame.h>
#include <av/scaler.h>

TEST_CASE("Downscale frame", "[scale][down]") {
  AV::Frame a(640, 480);
  AV::Frame b(320, 240);
  AV::Scaler scale;
  scale
  .from(640, 480)
  .to(320, 240)
  .open();

  b = scale(a);

  REQUIRE(a.get()->linesize[0] == 3 * 640);
  REQUIRE(b.get()->linesize[0] == 3 * 320);
}
