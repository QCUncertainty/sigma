#include "../testing.hpp"
#include "catch2/catch_test_macros.hpp"
#include <sstream>

using testing::test_affine;

TEMPLATE_TEST_CASE("Basic", "", sigma::AFloat, sigma::ADouble) {
    using affine_t = TestType;
    using value_t  = typename affine_t::value_t;

    value_t zero = 0.0;
    value_t one  = 1.0;
    value_t two  = 2.0;
    SECTION("abs") {
        TestType empty;
        REQUIRE(sigma::abs(empty).empty());

        TestType point(one);
        test_affine(sigma::abs(point), one, one);

        TestType negative_point(-one);
        test_affine(sigma::abs(negative_point), one, one);

        TestType positive_interval(one, two);
        REQUIRE(sigma::abs(positive_interval) == positive_interval);

        TestType negative_interval(-two, -one);
        test_affine(sigma::abs(negative_interval), one, two);

        // Straddles 0 asymmetrically: the exact range of |x| for
        // x in [-2, 1] is [0, 2], i.e. [0, max(|-2|, 1)].
        TestType straddling_interval(-two, one);
        test_affine(sigma::abs(straddling_interval), zero, two);
    }

    SECTION("fabs") {
        TestType empty;
        REQUIRE(sigma::fabs(empty).empty());

        TestType point(one);
        test_affine(sigma::fabs(point), one, one);

        TestType negative_point(-one);
        test_affine(sigma::fabs(negative_point), one, one);

        TestType positive_interval(one, two);
        REQUIRE(sigma::fabs(positive_interval) == positive_interval);

        TestType negative_interval(-two, -one);
        test_affine(sigma::fabs(negative_interval), one, two);

        TestType straddling_interval(-two, one);
        test_affine(sigma::fabs(straddling_interval), zero, two);
    }
}
