#include "catch2/catch_approx.hpp"
#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"
#include <sigma/taylor/operations/series_detail.hpp>

TEMPLATE_TEST_CASE("series_detail", "", float, double) {
    using value_t = TestType;
    using namespace sigma::detail;

    SECTION("factorial") {
        REQUIRE(factorial<value_t>(0) == Catch::Approx(1.0));
        REQUIRE(factorial<value_t>(1) == Catch::Approx(1.0));
        REQUIRE(factorial<value_t>(2) == Catch::Approx(2.0));
        REQUIRE(factorial<value_t>(3) == Catch::Approx(6.0));
        REQUIRE(factorial<value_t>(4) == Catch::Approx(24.0));
        REQUIRE(factorial<value_t>(5) == Catch::Approx(120.0));
    }

    SECTION("half_binomial_coefficient") {
        // C(1/2, k) = prod_{i=0}^{k-1} (1/2 - i) / k!
        REQUIRE(half_binomial_coefficient<value_t>(0) == Catch::Approx(1.0));
        REQUIRE(half_binomial_coefficient<value_t>(1) == Catch::Approx(0.5));
        REQUIRE(half_binomial_coefficient<value_t>(2) == Catch::Approx(-0.125));
        REQUIRE(half_binomial_coefficient<value_t>(3) == Catch::Approx(0.0625));
        REQUIRE(half_binomial_coefficient<value_t>(4) ==
                Catch::Approx(-0.0390625));
    }

    SECTION("half_falling_factorial") {
        // (1/2)(1/2-1)...(1/2-k+1) = half_binomial_coefficient(k) * k!
        REQUIRE(half_falling_factorial<value_t>(0) == Catch::Approx(1.0));
        REQUIRE(half_falling_factorial<value_t>(1) == Catch::Approx(0.5));
        REQUIRE(half_falling_factorial<value_t>(2) == Catch::Approx(-0.25));
        REQUIRE(half_falling_factorial<value_t>(3) == Catch::Approx(0.375));
        REQUIRE(half_falling_factorial<value_t>(4) == Catch::Approx(-0.9375));

        for(std::size_t k = 0; k <= 4; ++k) {
            REQUIRE(half_falling_factorial<value_t>(k) ==
                    Catch::Approx(half_binomial_coefficient<value_t>(k) *
                                  factorial<value_t>(k)));
        }
    }

    // The four outer-series generators below are all evaluated about c = 2,
    // through order 3, matching the reference values a Python re-derivation
    // of the same formulas produced independently.
    value_t c = value_t(2.0);

    SECTION("exp_outer_coeffs") {
        // exp(c)/k!
        auto coeffs = exp_outer_coeffs(c, std::size_t(3));
        REQUIRE(coeffs.size() == 4);
        REQUIRE(coeffs[0] == Catch::Approx(7.38905609893065));
        REQUIRE(coeffs[1] == Catch::Approx(7.38905609893065));
        REQUIRE(coeffs[2] == Catch::Approx(3.694528049465325));
        REQUIRE(coeffs[3] == Catch::Approx(1.231509349821775));
    }

    SECTION("log_outer_coeffs") {
        // log(c) for k=0, (-1)^(k-1) / (k c^k) for k >= 1
        auto coeffs = log_outer_coeffs(c, std::size_t(3));
        REQUIRE(coeffs.size() == 4);
        REQUIRE(coeffs[0] == Catch::Approx(0.6931471805599453));
        REQUIRE(coeffs[1] == Catch::Approx(0.5));
        REQUIRE(coeffs[2] == Catch::Approx(-0.125));
        REQUIRE(coeffs[3] == Catch::Approx(0.041666666666666664));
    }

    SECTION("reciprocal_outer_coeffs") {
        // (-1)^k / c^(k+1)
        auto coeffs = reciprocal_outer_coeffs(c, std::size_t(3));
        REQUIRE(coeffs.size() == 4);
        REQUIRE(coeffs[0] == Catch::Approx(0.5));
        REQUIRE(coeffs[1] == Catch::Approx(-0.25));
        REQUIRE(coeffs[2] == Catch::Approx(0.125));
        REQUIRE(coeffs[3] == Catch::Approx(-0.0625));
    }

    SECTION("sqrt_outer_coeffs") {
        // sqrt(c) * C(1/2, k) / c^k
        auto coeffs = sqrt_outer_coeffs(c, std::size_t(3));
        REQUIRE(coeffs.size() == 4);
        REQUIRE(coeffs[0] == Catch::Approx(1.4142135623730951));
        REQUIRE(coeffs[1] == Catch::Approx(0.3535533905932738));
        REQUIRE(coeffs[2] == Catch::Approx(-0.04419417382415922));
        REQUIRE(coeffs[3] == Catch::Approx(0.011048543456039806));
    }

    SECTION("order 0 is just the constant term") {
        REQUIRE(exp_outer_coeffs(c, std::size_t(0)).size() == 1);
        REQUIRE(exp_outer_coeffs(c, std::size_t(0))[0] ==
                Catch::Approx(7.38905609893065));
        REQUIRE(log_outer_coeffs(c, std::size_t(0))[0] ==
                Catch::Approx(0.6931471805599453));
        REQUIRE(reciprocal_outer_coeffs(c, std::size_t(0))[0] ==
                Catch::Approx(0.5));
        REQUIRE(sqrt_outer_coeffs(c, std::size_t(0))[0] ==
                Catch::Approx(1.4142135623730951));
    }
}
