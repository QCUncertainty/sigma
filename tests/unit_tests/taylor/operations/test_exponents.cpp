#include "../testing.hpp"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include <cmath>
#include <sstream>
#include <utility>

using testing::test_taylor;

TEMPLATE_TEST_CASE("Taylor Exponents", "", sigma::TFloat, sigma::TDouble) {
    using taylor_t   = TestType;
    using value_t    = typename taylor_t::value_t;
    using coeffs_t   = typename taylor_t::coeffs_t;
    using monomial_t = typename taylor_t::monomial_t;
    using order_t    = typename taylor_t::Order;

    value_t one = 1.0;
    value_t two = 2.0;

    // x = 2 + v (order 2), the same shape used throughout test_taylor.cpp's
    // "Division" and "compose_" sections, so the elementary functions below
    // can be checked against hand-derived outer-series coefficients: for an
    // outer function g composed about c_f = 2, compose_'s result is exactly
    // g(2) + g'(2)*v + (g''(2)/2)*v^2, since bar = x - 2 = v is exact to
    // every power at order 2.
    auto make_x = []() {
        auto v = taylor_t::make_deviation();
        coeffs_t coeffs{{monomial_t(v, 1), value_t(1.0)}};
        return std::pair{taylor_t(value_t(2.0), coeffs, order_t(2)), v};
    };

    SECTION("sqrt") {
        taylor_t empty;
        REQUIRE(sigma::sqrt(empty).empty());

        REQUIRE_THROWS_AS(sigma::sqrt(taylor_t(-one, one)), std::domain_error);

        taylor_t point(two);
        test_taylor(sigma::sqrt(point), value_t(std::sqrt(2.0)),
                    value_t(std::sqrt(2.0)));

        auto [x, v] = make_x();
        auto s      = sigma::sqrt(x);
        REQUIRE(s.constant() == Catch::Approx(1.4142135623730951));
        REQUIRE(s.coefficients().at(monomial_t(v, 1)) ==
                Catch::Approx(0.3535533905932738));
        REQUIRE(s.coefficients().at(monomial_t(v, 2)) ==
                Catch::Approx(-0.04419417382415922));
    }

    SECTION("exp") {
        taylor_t empty;
        REQUIRE(sigma::exp(empty).empty());

        taylor_t point(one);
        test_taylor(sigma::exp(point), value_t(std::exp(1.0)),
                    value_t(std::exp(1.0)));

        auto [x, v] = make_x();
        auto e      = sigma::exp(x);
        REQUIRE(e.constant() == Catch::Approx(7.38905609893065));
        REQUIRE(e.coefficients().at(monomial_t(v, 1)) ==
                Catch::Approx(7.38905609893065));
        REQUIRE(e.coefficients().at(monomial_t(v, 2)) ==
                Catch::Approx(3.694528049465325));
    }

    SECTION("log") {
        taylor_t empty;
        REQUIRE(sigma::log(empty).empty());
        REQUIRE_THROWS_AS(sigma::log(taylor_t(-one, one)), std::domain_error);

        taylor_t point(one);
        test_taylor(sigma::log(point), value_t(0.0), value_t(0.0));

        auto [x, v] = make_x();
        auto l      = sigma::log(x);
        REQUIRE(l.constant() == Catch::Approx(0.6931471805599453));
        REQUIRE(l.coefficients().at(monomial_t(v, 1)) == Catch::Approx(0.5));
        REQUIRE(l.coefficients().at(monomial_t(v, 2)) == Catch::Approx(-0.125));
    }

    SECTION("pow") {
        taylor_t empty;
        REQUIRE(sigma::pow(empty, 2).empty());

        taylor_t point_zero(0);
        test_taylor(sigma::pow(point_zero, 2), value_t(0.0), value_t(0.0));
        REQUIRE_THROWS_AS(sigma::pow(point_zero, -1), std::domain_error);
        REQUIRE_THROWS_AS(sigma::pow(taylor_t(-one, one), -2),
                          std::domain_error);

        auto [x, v] = make_x();
        REQUIRE(sigma::pow(x, 0) == taylor_t(one, order_t(2)));

        // (2 + v)^2 = 4 + 4v + v^2 exactly. pow is computed via
        // exp(2*log(x)) rather than repeated multiplication, but for a
        // purely linear x at order 2 the round trip is exact.
        auto p2 = sigma::pow(x, 2);
        REQUIRE(p2.constant() == Catch::Approx(4.0));
        REQUIRE(p2.coefficients().at(monomial_t(v, 1)) == Catch::Approx(4.0));
        REQUIRE(p2.coefficients().at(monomial_t(v, 2)) == Catch::Approx(1.0));

        // Negative base, even integer exponent: (-2 - v)^2 = 4 + 4v + v^2,
        // same as the positive case since squaring erases the sign.
        auto vy = taylor_t::make_deviation();
        coeffs_t y_coeffs{{monomial_t(vy, 1), one}};
        taylor_t y(value_t(-2.0), y_coeffs, order_t(2)); // -2 + v
        auto py2 = sigma::pow(y, 2);
        REQUIRE(py2.constant() == Catch::Approx(4.0));
        REQUIRE(py2.coefficients().at(monomial_t(vy, 1)) ==
                Catch::Approx(-4.0));
        REQUIRE(py2.coefficients().at(monomial_t(vy, 2)) == Catch::Approx(1.0));

        // Can't raise a Taylor polynomial containing 0 to a negative power.
        auto vz = taylor_t::make_deviation();
        coeffs_t z_coeffs{{monomial_t(vz, 1), one}};
        taylor_t z(value_t(0.0), z_coeffs, order_t(2)); // 0 + v, bound [-1,1]
        REQUIRE_THROWS_AS(sigma::pow(z, -2), std::domain_error);

        // Can't raise negative values to a non-integer power.
        REQUIRE_THROWS_AS(sigma::pow(y, 0.5), std::domain_error);

        SECTION("Bound straddling 0 (not the exact point 0), positive "
                "integer exponent") {
            // z = 0 + v, bound [-1, 1] -- contains 0 without being exactly
            // the point 0. z^2 is computed by repeated multiplication
            // (log/exp is not valid over a zero-containing bound), giving
            // z*z = v^2 exactly (the constant and linear terms vanish
            // because z has none), whose QFB bound is the exact true range
            // [0, 1] of x^2 over [-1, 1].
            auto squared = sigma::pow(z, 2);
            REQUIRE(squared.constant() == Catch::Approx(0.0));
            // operator*= doesn't prune exact-zero entries, so the v^1 term
            // is still present in the map, just with coefficient 0.
            REQUIRE(squared.coefficients().at(monomial_t(vz, 1)) ==
                    Catch::Approx(0.0));
            REQUIRE(squared.coefficients().at(monomial_t(vz, 2)) ==
                    Catch::Approx(1.0));
            test_taylor(squared, value_t(0.0), value_t(1.0));
        }

        SECTION("Bound straddling 0, non-integer exponent throws") {
            REQUIRE_THROWS_AS(sigma::pow(z, 0.5), std::domain_error);
        }
    }
}
