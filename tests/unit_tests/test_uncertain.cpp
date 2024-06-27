#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>

TEST_CASE("Uncertain") {
    SECTION("Constructors") {
        SECTION("Default") {
            auto value = sigma::UDouble();
            REQUIRE(value.mean() == 0.0);
            REQUIRE(value.std() == 0.0);
        }
        SECTION("With Values") {
            auto value = sigma::UDouble(1.0, 0.1);
            REQUIRE(value.mean() == 1.0);
            REQUIRE(value.std() == 0.1);
        }
        SECTION("Addition") {
            auto a = sigma::UDouble(1.0, 0.1);
            auto b = sigma::UDouble(2.0, 0.2);
            auto c = a + b;
            REQUIRE(c.mean() == 3.0);
            REQUIRE(c.std() == Approx(0.2236).margin(1.0e-4));
        }
    }
}