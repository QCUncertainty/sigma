#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include <sigma/detail/numeric.hpp>

TEMPLATE_TEST_CASE("is_integer_exponent, floating-point", "", float, double) {
    using value_t = TestType;

    SECTION("Whole numbers are integers") {
        REQUIRE(sigma::detail::is_integer_exponent(value_t(0.0)));
        REQUIRE(sigma::detail::is_integer_exponent(value_t(1.0)));
        REQUIRE(sigma::detail::is_integer_exponent(value_t(2.0)));
        REQUIRE(sigma::detail::is_integer_exponent(value_t(-3.0)));
        REQUIRE(sigma::detail::is_integer_exponent(value_t(100.0)));
    }

    SECTION("Fractional values are not integers") {
        REQUIRE_FALSE(sigma::detail::is_integer_exponent(value_t(0.5)));
        REQUIRE_FALSE(sigma::detail::is_integer_exponent(value_t(2.25)));
        REQUIRE_FALSE(sigma::detail::is_integer_exponent(value_t(-1.5)));
        REQUIRE_FALSE(sigma::detail::is_integer_exponent(value_t(-0.1)));
    }
}

TEMPLATE_TEST_CASE("is_integer_exponent, integral", "", int, long,
                   unsigned int) {
    using value_t = TestType;

    // Every value of an integral type is, trivially, an integer -- the
    // if constexpr branch in is_integer_exponent short-circuits to true
    // without calling std::modf (which wouldn't even compile for these
    // types).
    SECTION("Always true") {
        REQUIRE(sigma::detail::is_integer_exponent(value_t(0)));
        REQUIRE(sigma::detail::is_integer_exponent(value_t(1)));
        REQUIRE(sigma::detail::is_integer_exponent(value_t(42)));
    }
}
