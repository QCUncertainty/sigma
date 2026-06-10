#include "../testing.hpp"
#include "catch2/catch_test_macros.hpp"
#include <sstream>

using testing::test_affine;

TEMPLATE_TEST_CASE("Exponents", "", sigma::AFloat, sigma::ADouble) {
    using affine_t = TestType;
    using value_t  = typename affine_t::value_t;

    value_t one = 1.0;
    value_t two = 2.0;

    SECTION("sqrt") {
        affine_t empty;
        REQUIRE(sigma::sqrt(empty).empty());

        affine_t point(two);
        test_affine(sigma::sqrt(point), value_t(std::sqrt(2.0)),
                    value_t(std::sqrt(2.0)));

        affine_t interval(one, two);
        // Tight value is [1, sqrt(2)] = [1, 1.41421356237...], but
        // the affine transformation adds some error, so we get a slightly
        // looser upper bound.
        test_affine(sigma::sqrt(interval), one, value_t(1.43198051533946));

        REQUIRE_THROWS_AS(sigma::sqrt(affine_t(-one, one)), std::domain_error);
    }

    SECTION("exp") {
        affine_t empty;
        REQUIRE_THROWS_AS(sigma::exp(empty), std::domain_error);

        affine_t point(one);
        test_affine(sigma::exp(point), value_t(std::exp(1.0)),
                    value_t(std::exp(1.0)));

        affine_t interval(one, two);
        // Tight value is [e, e^2] = [2.71828182846..., 7.38905609893...], but
        // the affine transformation adds some error.
        test_affine(sigma::exp(interval), value_t(2.14236806757880416),
                    value_t(7.38905609893));
    }

    SECTION("log") {
        affine_t empty;
        REQUIRE_THROWS_AS(sigma::log(empty), std::domain_error);
        REQUIRE_THROWS_AS(sigma::log(affine_t(-one, one)), std::domain_error);

        affine_t point(one);
        test_affine(sigma::log(point), value_t(0.0), value_t(0.0));

        affine_t interval(one, two);
        // Tight value is [0, log(2)] = [0, 0.69314718056...], but the affine
        // transformation adds some error.
        test_affine(sigma::log(interval), value_t(0.0),
                    value_t(0.752807281701));
    }

    SECTION("pow") {
        affine_t empty;
        REQUIRE(sigma::pow(empty, 2) == empty);

        affine_t point_zero(0);
        test_affine(sigma::pow(point_zero, 2), value_t(0.0), value_t(0.0));

        affine_t point(two);
        test_affine(sigma::pow(point, 2), value_t(4.0), value_t(4.0));

        affine_t interval(one, two);
        REQUIRE(sigma::pow(interval, 0) == affine_t(one));
        // Tight value is [1, 4], but the affine transformation adds some error.
        test_affine(sigma::pow(interval, 2), value_t(0.35977815060135754),
                    value_t(4.50692256902870181));

        // Can raise negative values to integer powers
        affine_t negative_interval(-two, -one);
        REQUIRE(sigma::pow(negative_interval, 0) == affine_t(one));
        test_affine(sigma::pow(negative_interval, 2),
                    value_t(0.35977815060135754), value_t(4.50692256902870181));
        // Tight value is [-8, -1], but the affine transformation adds some
        // error.
        test_affine(sigma::pow(negative_interval, 3),
                    value_t(-9.56797749509911455),
                    value_t(1.26471835084614082));

        // Can't raise point zer to a negative power.
        REQUIRE_THROWS_AS(sigma::pow(point_zero, -1), std::domain_error);

        // // Can't raise an affine form containing 0 to a negative power.
        REQUIRE_THROWS_AS(sigma::pow(affine_t(-one, one), -2),
                          std::domain_error);

        // Can't raise negative values to a non-integer power.
        REQUIRE_THROWS_AS(sigma::pow(negative_interval, 0.5),
                          std::domain_error);
    }
}
