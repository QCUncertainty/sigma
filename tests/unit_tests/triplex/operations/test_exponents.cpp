#include "../testing.hpp"
#include <cmath>
#include <stdexcept>

using testing::test_invariant;
using testing::test_result;
using testing::test_triplex;

TEMPLATE_TEST_CASE("Exponents", "", TRIPLEX_TYPES) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    testing_t a(value_t{0.5}, value_t{0.8}, value_t{2});
    testing_t empty;

    SECTION("Square Root") {
        test_result(sigma::sqrt(a), sigma::sqrt(a.interval()),
                    std::sqrt(a.nominal()));
        REQUIRE(sigma::sqrt(empty).empty());
        testing_t negative(value_t{-1}, value_t{0.5}, value_t{1});
        REQUIRE_THROWS_AS(sigma::sqrt(negative), std::domain_error);
    }

    SECTION("Exponential Function") {
        test_result(sigma::exp(a), sigma::exp(a.interval()),
                    std::exp(a.nominal()));
        REQUIRE(sigma::exp(empty).empty());

        // From the design page: exp(<-1, 0, 1>) has nominal value e^0 = 1,
        // nowhere near the median of [1/e, e].
        testing_t sym(value_t{-1}, value_t{0}, value_t{1});
        auto r = sigma::exp(sym);
        test_triplex(r, std::exp(-1.0), 1.0, std::exp(1.0));
        REQUIRE(r.nominal() == value_t{1});
        REQUIRE(r.median() == Catch::Approx(std::cosh(1.0)).margin(1.0e-4));
    }

    SECTION("Natural Logarithm") {
        test_result(sigma::log(a), sigma::log(a.interval()),
                    std::log(a.nominal()));
        REQUIRE(sigma::log(empty).empty());
        testing_t zero(value_t{0}, value_t{0.5}, value_t{1});
        REQUIRE_THROWS_AS(sigma::log(zero), std::domain_error);
    }

    SECTION("Power with a scalar exponent") {
        test_result(sigma::pow(a, 2), sigma::pow(a.interval(), 2),
                    value_t(std::pow(a.nominal(), 2)));
        test_result(sigma::pow(a, -1), sigma::pow(a.interval(), -1),
                    value_t(std::pow(a.nominal(), -1)));
        test_result(sigma::pow(a, 0.5), sigma::pow(a.interval(), 0.5),
                    value_t(std::pow(a.nominal(), 0.5)));
        test_result(sigma::pow(a, 0), sigma::pow(a.interval(), 0), value_t{1});
        REQUIRE(sigma::pow(empty, 2).empty());

        // An even power of an interval straddling zero
        testing_t straddle(value_t{-2}, value_t{-1}, value_t{3});
        auto sq = sigma::pow(straddle, 2);
        REQUIRE(sq == testing_t(value_t{0}, value_t{1}, value_t{9}));

        REQUIRE_THROWS_AS(sigma::pow(straddle, -1), std::domain_error);
        REQUIRE_THROWS_AS(sigma::pow(straddle, 0.5), std::domain_error);
    }

    SECTION("Power with a triplex exponent") {
        testing_t e(value_t{1}, value_t{1.5}, value_t{2});
        test_result(sigma::pow(a, e), sigma::pow(a.interval(), e.interval()),
                    std::pow(a.nominal(), e.nominal()));
        test_invariant(sigma::pow(a, e));

        // A point exponent goes through the scalar overload, so a negative
        // base is fine
        testing_t negative(value_t{-2}, value_t{-1.5}, value_t{-1});
        testing_t two(value_t{2});
        REQUIRE(sigma::pow(negative, two) ==
                testing_t(value_t{1}, value_t{2.25}, value_t{4}));

        REQUIRE(sigma::pow(empty, e).empty());
        REQUIRE(sigma::pow(a, empty).empty());
        REQUIRE_THROWS_AS(sigma::pow(negative, e), std::domain_error);
    }
}
