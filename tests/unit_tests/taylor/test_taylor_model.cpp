#include "catch2/catch_test_macros.hpp"
#include "testing.hpp"
#include <cmath>
#include <sstream>

using testing::test_taylor;

TEMPLATE_TEST_CASE("TaylorModel", "", float, double) {
    using value_t        = TestType;
    using taylor_model_t = sigma::TaylorModel<TestType>;
    using taylor_t       = typename taylor_model_t::taylor_t;
    using interval_t     = typename taylor_model_t::interval_t;
    using coeffs_t       = typename taylor_model_t::coeffs_t;
    using monomial_t     = typename taylor_model_t::monomial_t;
    using order_t        = typename taylor_model_t::Order;

    value_t zero  = 0.0;
    value_t one   = 1.0;
    value_t two   = 2.0;
    value_t three = 3.0;
    value_t four  = 4.0;
    value_t five  = 5.0;

    SECTION("Constructors") {
        SECTION("Default") {
            taylor_model_t empty;
            REQUIRE(empty.empty());
            REQUIRE(empty.max_order() == 2); // default_max_order()
        }

        SECTION("From Center") {
            taylor_model_t value(one);
            test_taylor(value, one, one);
            REQUIRE(value.constant() == one);
            REQUIRE(value.coefficients().empty());
            REQUIRE(value.remainder() == interval_t(zero, zero));
        }

        SECTION("From Lower and Upper, Order >= 1 is Exact") {
            taylor_model_t value(one, three);
            test_taylor(value, one, three);
            REQUIRE(value.constant() == two);
            REQUIRE(value.coefficients().size() == 1);
            REQUIRE(value.remainder() == interval_t(zero, zero));
        }

        SECTION("From Lower and Upper, Order 0 Puts the Radius in the "
                "Remainder") {
            taylor_model_t value(one, three, order_t(0));
            REQUIRE(value.max_order() == 0);
            REQUIRE(value.coefficients().empty());
            REQUIRE(value.constant() == two);
            REQUIRE(value.remainder() == interval_t(-one, one));
            // The polynomial alone collapses to the median, but the
            // remainder restores the exact original interval.
            test_taylor(value, one, three);
        }

        SECTION("From Interval") {
            taylor_model_t empty(interval_t{});
            REQUIRE(empty.empty());

            taylor_model_t value(interval_t(one, three));
            test_taylor(value, one, three);
        }

        SECTION("Re-wrapping a Polynomial and Remainder") {
            taylor_t poly(two, three);       // range [2, 3] with order 2
            interval_t remainder(-one, one); // pretend this came from a sweep
            taylor_model_t value(poly, remainder);
            REQUIRE(value.polynomial() == poly);
            REQUIRE(value.remainder() == remainder);
            test_taylor(value, one, four); // [2, 3] + [-1, 1] = [1, 4]
        }

        SECTION("Copy Constructor") {
            taylor_model_t point(one);
            taylor_model_t copy(point);
            REQUIRE(copy == point);
        }

        SECTION("Move Constructor") {
            taylor_model_t point(one);
            taylor_model_t moved(std::move(point));
            test_taylor(moved, one, one);
        }

        SECTION("Copy Assignment") {
            taylor_model_t point(one);
            taylor_model_t copy;
            copy = point;
            REQUIRE(copy == point);
        }

        SECTION("Move Assignment") {
            taylor_model_t point(one);
            taylor_model_t moved;
            moved = std::move(point);
            test_taylor(moved, one, one);
        }
    }

    SECTION("order/constant/coefficients/n_terms") {
        taylor_model_t empty;
        REQUIRE(empty.n_terms() == 0);

        taylor_model_t value(one, three);
        REQUIRE(value.max_order() == 2);
        REQUIRE(value.constant() == two);
        REQUIRE(value.coefficients().size() == 1);
        REQUIRE(value.n_terms() == 2);
    }

    SECTION("empty") {
        taylor_model_t empty;
        REQUIRE(empty.empty());
        REQUIRE_THROWS_AS(empty.constant(), std::domain_error);
        REQUIRE_THROWS_AS(empty.range(), std::domain_error);
        REQUIRE_FALSE(empty.contains(zero));

        REQUIRE_FALSE(taylor_model_t(one, three).empty());
    }

    SECTION("range/contains") {
        taylor_model_t value(one, three);
        test_taylor(value, one, three);

        REQUIRE(value.contains(two));
        REQUIRE_FALSE(value.contains(value_t(0.5)));
        REQUIRE(value.contains(interval_t(value_t(1.5), value_t(2.5))));
        REQUIRE_FALSE(value.contains(interval_t(zero, five)));
        REQUIRE(value.contains(taylor_model_t(value_t(1.5), two)));

        SECTION("contains an empty model vacuously") {
            REQUIRE(value.contains(taylor_model_t{}));
        }
    }

    SECTION("Arithmetic") {
        SECTION("operator-() unary negation") {
            taylor_model_t empty;
            REQUIRE(-empty == empty);

            taylor_model_t value(one, three);
            auto neg = -value;
            test_taylor(neg, -three, -one);
        }

        SECTION("operator+=(value_t) on empty becomes the value") {
            taylor_model_t value;
            value += five;
            test_taylor(value, five, five);
            REQUIRE(value.max_order() == 2);
            REQUIRE(value.remainder() == interval_t(zero, zero));
        }

        SECTION("operator+=(value_t) shifts the constant, remainder "
                "unaffected") {
            taylor_model_t value(one, three, order_t(0)); // remainder [-1,1]
            value += one;
            REQUIRE(value.remainder() == interval_t(-one, one));
            test_taylor(value, two, four); // [1, 3] shifted by 1 -> [2, 4]
        }

        SECTION("operator+(TaylorModel), disjoint variables, no "
                "truncation needed") {
            taylor_model_t x(one, three); // range [1, 3], order 2
            taylor_model_t y(two, four);  // range [2, 4], order 2
            auto z = x + y;
            test_taylor(z, three, value_t(7.0));
            REQUIRE(z.remainder() == interval_t(zero, zero));
        }

        SECTION("operator-=(value_t)") {
            taylor_model_t value(one, three);
            value -= one;
            test_taylor(value, zero, two);
        }

        SECTION("operator-(TaylorModel), same operand cancels exactly") {
            taylor_model_t x(one, three);
            auto z = x - x;
            test_taylor(z, zero, zero);
            REQUIRE(z.remainder() == interval_t(zero, zero));
        }

        SECTION("operator*=(value_t) on empty is a no-op") {
            taylor_model_t value;
            value *= two;
            REQUIRE(value.empty());
        }

        SECTION("operator*=(value_t) scales polynomial and remainder") {
            taylor_model_t value(one, three, order_t(0)); // remainder [-1,1]
            value *= two;
            REQUIRE(value.remainder() == interval_t(-two, two));
            test_taylor(value, two, value_t(6.0));
        }

        SECTION("operator*(TaylorModel), no truncation: remainder stays "
                "empty") {
            // Neither operand has a term above order 2, so multiplying two
            // order-2 models never has anything to sweep: the cross term
            // vx*vy has degree 2, which still fits.
            taylor_model_t x(one, three); // 2 + vx, order 2
            taylor_model_t y(two, four);  // 3 + vy, order 2
            auto z = x * y;
            REQUIRE(z.max_order() == 2);
            REQUIRE(z.remainder() == interval_t(zero, zero));
            REQUIRE(z.constant() == value_t(6.0));
            REQUIRE(z.coefficients().size() == 3); // vx, vy, vx*vy
            // Same naive-fallback looseness as Taylor::bound() alone gets
            // for this case (neither variable has its own quadratic term),
            // but it still safely contains the true range [2, 12].
            test_taylor(z, zero, value_t(12.0));
        }

        SECTION("operator*(TaylorModel), truncation puts the cross term "
                "in the remainder") {
            // Order 1 this time: the vx*vy cross term (degree 2) no longer
            // fits and must be bounded into the remainder instead of kept.
            taylor_model_t x(one, three, order_t(1)); // 2 + vx
            taylor_model_t y(two, four, order_t(1));  // 3 + vy
            auto z = x * y;
            REQUIRE(z.max_order() == 1);
            // vx*vy bounds to [-1,1] * [-1,1] = [-1,1] (naive fallback).
            REQUIRE(z.remainder() == interval_t(-one, one));
            REQUIRE(z.coefficients().size() == 2); // vx, vy only
            // polynomial().bound() = 6 +/- 2 +/- 3 = [1, 11];
            // range() = [1, 11] + [-1, 1] = [0, 12], which still safely
            // contains the true range [2, 12].
            test_taylor(z, zero, value_t(12.0));
            REQUIRE(z.contains(two));
            REQUIRE(z.contains(value_t(12.0)));
        }

        SECTION("scalar * TaylorModel") {
            taylor_model_t value(one, three);
            auto z = two * value;
            test_taylor(z, two, value_t(6.0));
        }
    }

    SECTION("Division") {
        SECTION("operator/=(value_t) and operator/(value_t)") {
            taylor_model_t empty;
            auto pempty = &(empty /= one);
            REQUIRE(pempty == &empty);
            REQUIRE(empty.empty());

            taylor_model_t value(one, three);
            auto pvalue = &(value /= two);
            REQUIRE(pvalue == &value);
            test_taylor(value, value_t(0.5), value_t(1.5));

            REQUIRE_THROWS_AS(value /= zero, std::domain_error);

            taylor_model_t other(one, three);
            auto quot = other / two;
            test_taylor(quot, value_t(0.5), value_t(1.5));
            REQUIRE_THROWS_AS(other / zero, std::domain_error);
        }

        SECTION("multiplicative_inverse") {
            taylor_model_t empty;
            REQUIRE_THROWS_AS(empty.multiplicative_inverse(),
                              std::domain_error);

            taylor_model_t containing_zero(-one, one);
            REQUIRE_THROWS_AS(containing_zero.multiplicative_inverse(),
                              std::domain_error);

            // range() = [1, 3], order 2, empty remainder: sample the true
            // 1/x at points spanning that exact range and require the
            // result to enclose every one of them, per Eq.
            // \eqref{eq:tm-inclusion}.
            taylor_model_t x(one, three);
            auto inv = x.multiplicative_inverse();
            for(int i = 0; i <= 8; ++i) {
                value_t t  = value_t(i) / value_t(8);
                value_t xv = one + t * two; // samples [1, 3]
                REQUIRE(inv.contains(one / xv));
            }
        }

        SECTION("operator/=(TaylorModel) and operator/(TaylorModel)") {
            taylor_model_t x(one, three); // [1, 3]
            taylor_model_t y(two, four);  // [2, 4]
            auto z = x / y;
            for(int i = 0; i <= 4; ++i) {
                value_t s = one + value_t(i) / value_t(4) * two; // [1, 3]
                for(int j = 0; j <= 4; ++j) {
                    value_t t = two + value_t(j) / value_t(4) * two; // [2,4]
                    REQUIRE(z.contains(s / t));
                }
            }

            taylor_model_t empty;
            REQUIRE((empty / y).empty());
        }
    }

    SECTION("compose_") {
        taylor_model_t x(one, three); // [1, 3], order 2, empty remainder

        // A g whose Taylor coefficients about c_f are exactly {1, 2, 3} is
        // itself a degree-2 polynomial, so a (3rd-)derivative of
        // identically 0 is the correct Lagrange remainder term: the
        // composed model's remainder should stay [0, 0] and its polynomial
        // should match Taylor::compose_'s exactly.
        auto composed = x.compose_({one, two, three}, [=](const interval_t&) {
            return interval_t(zero, zero);
        });
        REQUIRE(composed.polynomial() ==
                x.polynomial().compose_({one, two, three}));
        REQUIRE(composed.remainder() == interval_t(zero, zero));

        taylor_model_t empty;
        REQUIRE_THROWS_AS(
          empty.compose_(
            {one}, [=](const interval_t&) { return interval_t(zero, zero); }),
          std::domain_error);
    }

    SECTION("sweep_to_order") {
        auto v = taylor_model_t::make_deviation();
        // p = 5 + 2v + 3v^2, order 2, no pre-existing remainder.
        coeffs_t coeffs{{monomial_t(v, 1), two}, {monomial_t(v, 2), three}};
        taylor_t p(five, coeffs, order_t(2));
        taylor_model_t model(p, interval_t(zero, zero));

        auto swept = model.sweep_to_order(1);
        REQUIRE(swept.max_order() == 1);
        REQUIRE(swept.coefficients().size() == 1);
        REQUIRE(swept.coefficients().count(monomial_t(v, 2)) == 0);
        // The dropped 3v^2 term bounds to [0, 3] (min at v=0, max at
        // v=+/-1), which becomes the new remainder exactly.
        REQUIRE(swept.remainder() == interval_t(zero, three));

        // True range of 5 + 2v + 3v^2 over v in [-1,1] is about [4.67, 10]
        // (min near v = -1/3); the swept model must still contain it, even
        // though it is not the tightest possible enclosure.
        REQUIRE(swept.contains(value_t(10.0)));
        REQUIRE(swept.contains(value_t(6.0)));

        auto same = model.sweep_to_order(5); // raising the order sweeps
                                             // nothing
        REQUIRE(same.max_order() == 5);
        REQUIRE(same.coefficients().size() == 2);
        REQUIRE(same.remainder() == interval_t(zero, zero));
    }

    SECTION("sweep_small") {
        auto v = taylor_model_t::make_deviation();
        auto w = taylor_model_t::make_deviation();
        // p = 0.01*v + 5*w, order 2, no pre-existing remainder. v's
        // contribution (0.01) is small relative to the model's range
        // (radius ~5.01), w's is not.
        coeffs_t coeffs{{monomial_t(v, 1), value_t(0.01)},
                        {monomial_t(w, 1), five}};
        taylor_t p(zero, coeffs, order_t(2));
        taylor_model_t model(p, interval_t(zero, zero));

        auto swept = model.sweep_small(value_t(0.01));
        REQUIRE(swept.coefficients().size() == 1);
        REQUIRE(swept.coefficients().count(monomial_t(w, 1)) == 1);
        REQUIRE(swept.coefficients().count(monomial_t(v, 1)) == 0);
        REQUIRE(swept.remainder().lower() < zero);
        REQUIRE(swept.remainder().upper() > zero);

        // The original model's range must still be contained.
        REQUIRE(swept.contains(model.range()));
    }

    SECTION("Elementary Functions") {
        // Samples n+1 evenly-spaced points across [lo, hi] and requires
        // model to contain f() evaluated at each -- the model's range() is
        // exactly [lo, hi] (order >= 1, empty remainder) for every fixture
        // below, so this exercises Eq. \eqref{eq:tm-inclusion} at more than
        // just the endpoints.
        auto check_contains = [](auto&& model, value_t lo, value_t hi,
                                 auto&& f) {
            for(int i = 0; i <= 8; ++i) {
                value_t t = value_t(i) / value_t(8);
                value_t x = lo + t * (hi - lo);
                REQUIRE(model.contains(f(x)));
            }
        };

        SECTION("sqrt") {
            taylor_model_t empty;
            REQUIRE(sigma::sqrt(empty).empty());
            REQUIRE_THROWS_AS(sigma::sqrt(taylor_model_t(-one, one)),
                              std::domain_error);

            taylor_model_t x(one, three);
            check_contains(sigma::sqrt(x), one, three,
                           [](value_t v) { return std::sqrt(v); });
        }

        SECTION("exp") {
            taylor_model_t empty;
            REQUIRE(sigma::exp(empty).empty());

            taylor_model_t x(one, three);
            check_contains(sigma::exp(x), one, three,
                           [](value_t v) { return std::exp(v); });
        }

        SECTION("log") {
            taylor_model_t empty;
            REQUIRE(sigma::log(empty).empty());
            REQUIRE_THROWS_AS(sigma::log(taylor_model_t(-one, one)),
                              std::domain_error);

            taylor_model_t x(one, three);
            check_contains(sigma::log(x), one, three,
                           [](value_t v) { return std::log(v); });
        }

        SECTION("pow") {
            taylor_model_t empty;
            REQUIRE(sigma::pow(empty, 2).empty());
            REQUIRE_THROWS_AS(sigma::pow(taylor_model_t(-one, one), -2),
                              std::domain_error);

            taylor_model_t x(one, three);
            check_contains(sigma::pow(x, 2), one, three,
                           [](value_t v) { return v * v; });

            taylor_model_t negative(-three, -one);
            check_contains(sigma::pow(negative, 3), -three, -one,
                           [](value_t v) { return v * v * v; });

            // range() = [-1, 1] straddles 0 without being exactly the
            // point 0: log/exp is not valid here, so pow must fall back to
            // repeated multiplication (Eq. \eqref{eq:tm-mul}) and still
            // rigorously enclose the true range [0, 1] of x^2 -- not
            // collapse to the single point 0, which is what this branch
            // used to (incorrectly) return.
            taylor_model_t straddling(-one, one);
            auto squared = sigma::pow(straddling, 2);
            REQUIRE_FALSE(squared.range() == interval_t(zero, zero));
            check_contains(squared, -one, one, [](value_t v) { return v * v; });

            REQUIRE_THROWS_AS(sigma::pow(straddling, 0.5), std::domain_error);
        }

        SECTION("abs") {
            taylor_model_t empty;
            REQUIRE(sigma::abs(empty).empty());

            taylor_model_t positive(one, two);
            REQUIRE(sigma::abs(positive) == positive);

            taylor_model_t negative(-two, -one);
            test_taylor(sigma::abs(negative), one, two);

            // Straddles 0 asymmetrically: [0, max(|-2|, 1)] = [0, 2].
            taylor_model_t straddling(-two, one);
            auto abs_straddling = sigma::abs(straddling);
            test_taylor(abs_straddling, zero, two);
            check_contains(abs_straddling, -two, one,
                           [](value_t v) { return std::fabs(v); });
        }

        SECTION("fabs is an alias for abs") {
            // Equality is structural (see "Comparison Operators" above), and
            // the straddling-0 case of abs mints a fresh deviation variable
            // per call, so fabs(x) and abs(x) are equal in value but not
            // structurally == to each other; compare ranges instead.
            taylor_model_t x(-two, one);
            REQUIRE(sigma::fabs(x).range() == sigma::abs(x).range());
        }

        SECTION("Remainder does not grow with increasing order") {
            // Order-(n+1) scaling (docs/taylor.md's "Order scaling"): for a
            // fixed nonlinear function and domain, a higher truncation
            // order must not produce a looser enclosure.
            taylor_model_t low_order(value_t(0.9), value_t(1.1), order_t(1));
            taylor_model_t high_order(value_t(0.9), value_t(1.1), order_t(5));
            auto low  = sigma::log(low_order);
            auto high = sigma::log(high_order);
            REQUIRE(high.remainder().width() <= low.remainder().width());
        }
    }

    SECTION("Comparison Operators") {
        taylor_model_t x(one, three);
        taylor_model_t same_x(one, three);
        taylor_model_t copy_of_x(x);

        REQUIRE(x == copy_of_x);
        REQUIRE_FALSE(x != copy_of_x);

        // Equality is structural: same_x mints its own deviation variable,
        // so it is not == x even though the two ranges match.
        REQUIRE(x != same_x);

        taylor_model_t different_order(one, three, order_t(3));
        REQUIRE(x != different_order);

        taylor_model_t empty1, empty2;
        REQUIRE(empty1 == empty2);
    }

    SECTION("make_deviation returns unique ids") {
        auto v0 = taylor_model_t::make_deviation();
        auto v1 = taylor_model_t::make_deviation();
        REQUIRE(v0 != v1);
    }

    SECTION("Stream Insertion / Printing") {
        taylor_model_t value(one, three);

        std::stringstream ss;
        ss << value;
        REQUIRE_FALSE(ss.str().empty());

        REQUIRE_FALSE(value.print_polynomial_form().empty());
        REQUIRE_FALSE(value.print_interval_form().empty());

        taylor_model_t empty;
        REQUIRE(empty.print_polynomial_form() == "∅");

        // Streaming an empty TaylorModel must not throw, even though
        // range() does -- operator<< special-cases it, matching Interval's
        // own operator<<.
        std::stringstream empty_ss;
        REQUIRE_NOTHROW(empty_ss << empty);
        REQUIRE(empty_ss.str() == "∅");
    }
}
