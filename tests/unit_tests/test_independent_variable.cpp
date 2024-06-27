#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>

TEST_CASE("IndependentVariable") {
    SECTION("Constructors") {
        SECTION("Default") {
            auto value = sigma::IndependentVariable<double>();
            REQUIRE(value.mean() == 0.0);
            REQUIRE(value.std() == 0.0);
        }
        SECTION("With Values") {
            auto value = sigma::IndependentVariable<double>(1.0, 0.1);
            REQUIRE(value.mean() == 1.0);
            REQUIRE(value.std() == 0.1);
        }
    }
}