#include <catch2/catch_test_macros.hpp>
#include <av/packet.h>

TEST_CASE("Packet, create", "[packet][create]" ) {
  AV::Packet packet{};
  REQUIRE(packet.get()->size == 0);
}

TEST_CASE("Packet, copy", "[packet][copy]") {
  AV::Packet a{};
  REQUIRE(a.get()->size == 0);
  AV::Packet b{a};
  REQUIRE(b.get()->size == 0);
}
