#include "../testing.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>

// The operations themselves are covered for both interval types by the other
// files in this directory. This file covers what only a GeneralInterval can
// express: an argument with an open bound, and the openness of the result.

TEMPLATE_TEST_CASE("General interval operations", "", sigma::GIFloat,
                   sigma::GIDouble) {
    using testing_t    = TestType;
    using value_t      = typename testing_t::value_t;
    constexpr auto inf = std::numeric_limits<value_t>::infinity();

    SECTION("Natural Logarithm") {
        // An OPEN bound at zero is not a value the interval contains, so the
        // logarithm of it is defined -- and unbounded below.
        auto a = sigma::log(testing_t(value_t{0}, value_t{1}, true, false));
        REQUIRE(a.lower() == -inf);
        REQUIRE(a.left_open());
        REQUIRE(a.contains(value_t{0}));

        REQUIRE_THROWS_AS(sigma::log(testing_t(value_t{0}, value_t{1})),
                          std::domain_error);
    }

    SECTION("Power") {
        // A bound merely AT zero, left open, does not contain zero
        auto b = sigma::pow(testing_t(value_t{0}, value_t{1}, true, false), -1);
        REQUIRE(b.lower() == value_t{1});
        REQUIRE(b.upper() == inf);
        REQUIRE(b.right_open());
    }

    SECTION("Tangent") {
        // Every acceptable argument is within one increasing branch, so the
        // openness of each bound carries over
        auto b = sigma::tan(testing_t(value_t{0}, value_t{1}, true, false));
        REQUIRE(b.left_open());
        REQUIRE_FALSE(b.right_open());
    }

    SECTION("Arccosine") {
        // Decreasing, so the openness of the bounds trades places
        auto a = sigma::acos(testing_t(value_t{0}, value_t{1}, true, false));
        REQUIRE_FALSE(a.left_open());
        REQUIRE(a.right_open());
    }

    SECTION("Inverse Hyperbolic Tangent") {
        // An OPEN bound at plus or minus one is not a value the interval
        // contains, so the argument is in domain -- and the result is
        // unbounded on that side
        auto a = sigma::atanh(testing_t(value_t{-1}, value_t{0}, true, false));
        REQUIRE(a.lower() == -inf);
        REQUIRE(a.left_open());
        REQUIRE(a.contains(value_t{0}));

        REQUIRE_THROWS_AS(sigma::atanh(testing_t(value_t{-1}, value_t{0})),
                          std::domain_error);
    }

    SECTION("Interior extrema are attained") {
        // The minimum of cosh, and of abs, over an argument spanning zero is
        // an interior point, so it is a closed bound whatever the argument's
        // openness
        testing_t open_span(value_t{-1}, value_t{2}, true, true);
        REQUIRE(sigma::cosh(open_span).left_closed());
        REQUIRE(sigma::cosh(open_span).right_open());
        REQUIRE(sigma::abs(open_span) ==
                testing_t(value_t{0}, value_t{2}, false, true));
    }
}
