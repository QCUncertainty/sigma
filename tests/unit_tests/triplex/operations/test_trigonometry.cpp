#include "../testing.hpp"
#include <cmath>
#include <stdexcept>

using testing::test_result;

TEMPLATE_TEST_CASE("Trigonometry", "", TRIPLEX_TYPES) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    testing_t a(value_t{0.2}, value_t{0.3}, value_t{0.7});
    testing_t empty;

    SECTION("Sine") {
        test_result(sigma::sin(a), sigma::sin(a.interval()),
                    std::sin(a.nominal()));
        REQUIRE(sigma::sin(empty).empty());
    }

    SECTION("Cosine") {
        test_result(sigma::cos(a), sigma::cos(a.interval()),
                    std::cos(a.nominal()));
        REQUIRE(sigma::cos(empty).empty());
    }

    SECTION("Tangent") {
        test_result(sigma::tan(a), sigma::tan(a.interval()),
                    std::tan(a.nominal()));
        REQUIRE(sigma::tan(empty).empty());
        // [1, 2] contains pi/2, even though the nominal value does not
        testing_t pole(value_t{1}, value_t{1.2}, value_t{2});
        REQUIRE_THROWS_AS(sigma::tan(pole), std::domain_error);
    }

    SECTION("Arcsine") {
        test_result(sigma::asin(a), sigma::asin(a.interval()),
                    std::asin(a.nominal()));
        REQUIRE(sigma::asin(empty).empty());
        testing_t out(value_t{0.5}, value_t{0.9}, value_t{1.5});
        REQUIRE_THROWS_AS(sigma::asin(out), std::domain_error);
    }

    SECTION("Arccosine") {
        test_result(sigma::acos(a), sigma::acos(a.interval()),
                    std::acos(a.nominal()));
        REQUIRE(sigma::acos(empty).empty());
        testing_t out(value_t{0.5}, value_t{0.9}, value_t{1.5});
        REQUIRE_THROWS_AS(sigma::acos(out), std::domain_error);
    }

    SECTION("Arctangent") {
        test_result(sigma::atan(a), sigma::atan(a.interval()),
                    std::atan(a.nominal()));
        REQUIRE(sigma::atan(empty).empty());
    }
}
