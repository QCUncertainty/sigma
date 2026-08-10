#include "../testing.hpp"
#include "catch2/catch_test_macros.hpp"
#include <sstream>

using testing::test_taylor;

TEMPLATE_TEST_CASE("Taylor Basic", "", sigma::TFloat, sigma::TDouble) {
    using taylor_t = TestType;
    using value_t  = typename taylor_t::value_t;

    value_t zero = 0.0;
    value_t one  = 1.0;
    value_t two  = 2.0;

    SECTION("abs") {
        taylor_t empty;
        REQUIRE(sigma::abs(empty).empty());

        taylor_t point(one);
        test_taylor(sigma::abs(point), one, one);

        taylor_t negative_point(-one);
        test_taylor(sigma::abs(negative_point), one, one);

        taylor_t positive_interval(one, two);
        REQUIRE(sigma::abs(positive_interval) == positive_interval);

        taylor_t negative_interval(-two, -one);
        test_taylor(sigma::abs(negative_interval), one, two);

        // Straddles 0 asymmetrically: the exact range of |x| for
        // x in [-2, 1] is [0, 2], i.e. [0, max(|-2|, 1)]. Unlike the
        // one-sided cases above, this does not return *this or -*this
        // unchanged -- it mints a fresh single-term Taylor from that
        // interval, mirroring Affine::abs.
        taylor_t straddling_interval(-two, one);
        test_taylor(sigma::abs(straddling_interval), zero, two);
    }

    SECTION("fabs") {
        taylor_t empty;
        REQUIRE(sigma::fabs(empty).empty());

        taylor_t point(one);
        test_taylor(sigma::fabs(point), one, one);

        taylor_t negative_point(-one);
        test_taylor(sigma::fabs(negative_point), one, one);

        taylor_t positive_interval(one, two);
        REQUIRE(sigma::fabs(positive_interval) == positive_interval);

        taylor_t negative_interval(-two, -one);
        test_taylor(sigma::fabs(negative_interval), one, two);

        taylor_t straddling_interval(-two, one);
        test_taylor(sigma::fabs(straddling_interval), zero, two);
    }
}
