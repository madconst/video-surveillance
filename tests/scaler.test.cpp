#include "inc/frame.h"
#include "inc/scaler.h"
#include "catch.hpp"

TEST_CASE("Downscale frame", "[scale][down]") {
  Frame a(640, 480);
  Frame b(320, 240);
  Scaler scale;
  scale
  .from(640, 480)
  .to(320, 240)
  .open();

  b = scale(a);

  REQUIRE(a.get()->linesize[0] == 3 * 640);
  REQUIRE(b.get()->linesize[0] == 3 * 320);
}
