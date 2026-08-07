#include "catch2/catch_test_macros.hpp"
#include "testing.hpp"
#include <sstream>

using testing::test_taylor;

TEMPLATE_TEST_CASE("Taylor", "", float, double) {
    using value_t    = TestType;
    using taylor_t   = sigma::Taylor<TestType>;
    using interval_t = typename taylor_t::interval_t;
    using coeffs_t   = typename taylor_t::coeffs_t;
    using order_t    = typename taylor_t::Order;

    value_t zero  = 0.0;
    value_t one   = 1.0;
    value_t two   = 2.0;
    value_t three = 3.0;
    value_t four  = 4.0;
    value_t five  = 5.0;

    SECTION("Constructors") {
        SECTION("Default") {
            taylor_t empty;
            REQUIRE(empty.empty());
            REQUIRE(empty.order() == 2); // default_order()
        }

        SECTION("From Center") {
            taylor_t value(one);
            test_taylor(value, one, one);
            REQUIRE(value.constant() == one);
            REQUIRE(value.coefficients().empty());
        }

        SECTION("From Lower and Upper") {
            taylor_t value(one, three);
            test_taylor(value, one, three);
            REQUIRE(value.constant() == two);
            REQUIRE(value.coefficients().size() == 1);
        }

        SECTION("From Lower and Upper, Order 0 Drops the Term") {
            taylor_t value(one, three, order_t(0));
            REQUIRE(value.order() == 0);
            REQUIRE(value.coefficients().empty());
            test_taylor(value, two, two); // collapses to the median
        }

        SECTION("From Interval") {
            taylor_t empty(interval_t{});
            REQUIRE(empty.empty());

            taylor_t value(interval_t(one, three));
            test_taylor(value, one, three);
        }

        SECTION("From Constant and Coefficients") {
            auto v = taylor_t::make_deviation();
            coeffs_t coeffs{{typename taylor_t::monomial_t(v, 1), two}};
            taylor_t value(three, coeffs, order_t(2));
            REQUIRE(value.constant() == three);
            REQUIRE(value.coefficients().size() == 1);
        }

        SECTION("From Constant and Coefficients, Above-Order Terms Dropped") {
            auto v = taylor_t::make_deviation();
            coeffs_t coeffs{{typename taylor_t::monomial_t(v, 3), two}};
            taylor_t value(three, coeffs, order_t(2));
            REQUIRE(value.coefficients().empty());
        }

        SECTION("Copy Constructor") {
            taylor_t point(one);
            taylor_t copy(point);
            REQUIRE(copy == point);
        }

        SECTION("Move Constructor") {
            taylor_t point(one);
            taylor_t moved(std::move(point));
            test_taylor(moved, one, one);
        }

        SECTION("Copy Assignment") {
            taylor_t point(one);
            taylor_t copy;
            copy = point;
            REQUIRE(copy == point);
        }

        SECTION("Move Assignment") {
            taylor_t point(one);
            taylor_t moved;
            moved = std::move(point);
            test_taylor(moved, one, one);
        }
    }

    SECTION("order/constant/coefficients/n_terms") {
        taylor_t empty;
        REQUIRE(empty.n_terms() == 0);

        taylor_t value(one, three);
        REQUIRE(value.order() == 2);
        REQUIRE(value.constant() == two);
        REQUIRE(value.coefficients().size() == 1);
        REQUIRE(value.n_terms() == 2);
    }

    SECTION("empty") {
        taylor_t empty;
        REQUIRE(empty.empty());
        REQUIRE_THROWS_AS(empty.constant(), std::domain_error);
        REQUIRE_THROWS_AS(empty.bound(), std::domain_error);
        REQUIRE_THROWS_AS(empty.range(), std::domain_error);
        REQUIRE_FALSE(empty.contains(zero));

        REQUIRE_FALSE(taylor_t(one, three).empty());
    }

    SECTION("range/bound/contains") {
        taylor_t value(one, three);
        test_taylor(value, one, three);

        REQUIRE(value.contains(two));
        REQUIRE_FALSE(value.contains(value_t(0.5)));
        REQUIRE(value.contains(interval_t(value_t(1.5), value_t(2.5))));
        REQUIRE_FALSE(value.contains(interval_t(zero, five)));
        REQUIRE(value.contains(taylor_t(value_t(1.5), two)));
    }

    SECTION("bound, quadratic fast bounder branches") {
        // Each case builds p = lin*v + quad*v^2 (constant 0, no other
        // terms), so bound() should equal the diagonal extremum exactly --
        // these exercise the branches the "Order 3" cubic test above
        // doesn't reach.
        using monomial_t = typename taylor_t::monomial_t;

        SECTION("convex, vertex outside the domain") {
            // f(d) = d^2 + 10d; f'(d) = 2d + 10 > 0 on [-1,1], so f is
            // monotonic increasing there: min = f(-1) = -9, max = f(1) = 11.
            auto v = taylor_t::make_deviation();
            coeffs_t coeffs{{monomial_t(v, 1), value_t(10.0)},
                            {monomial_t(v, 2), one}};
            taylor_t p(zero, coeffs, order_t(2));
            test_taylor(p, value_t(-9.0), value_t(11.0));
        }

        SECTION("concave, vertex inside the domain") {
            // f(d) = -2d^2 + d; vertex at d* = 0.25 (in [-1,1]) is the
            // maximum, f(d*) = 0.125. f(-1) = -3, f(1) = -1, so the minimum
            // is -3.
            auto v = taylor_t::make_deviation();
            coeffs_t coeffs{{monomial_t(v, 1), one},
                            {monomial_t(v, 2), value_t(-2.0)}};
            taylor_t p(zero, coeffs, order_t(2));
            test_taylor(p, value_t(-3.0), value_t(0.125));
        }

        SECTION("concave, vertex outside the domain") {
            // f(d) = -d^2 + 10d; f'(d) = -2d + 10 > 0 on [-1,1], so f is
            // monotonic increasing there: min = f(-1) = -11, max = f(1) = 9.
            auto v = taylor_t::make_deviation();
            coeffs_t coeffs{{monomial_t(v, 1), value_t(10.0)},
                            {monomial_t(v, 2), -one}};
            taylor_t p(zero, coeffs, order_t(2));
            test_taylor(p, value_t(-11.0), value_t(9.0));
        }
    }

    SECTION("Arithmetic") {
        SECTION("operator-() unary negation") {
            taylor_t empty;
            REQUIRE(-empty == empty);

            taylor_t value(one, three);
            auto neg = -value;
            test_taylor(neg, -three, -one);
        }

        SECTION("operator+=(value_t) on empty becomes the value") {
            taylor_t value;
            value += five;
            test_taylor(value, five, five);
            REQUIRE(value.order() == 2);
        }

        SECTION("operator+=(value_t) shifts the constant") {
            taylor_t value(one, three);
            value += one;
            test_taylor(value, two, four);
        }

        SECTION("operator+(Taylor), disjoint variables sum ranges exactly") {
            taylor_t x(one, three); // range [1, 3]
            taylor_t y(two, four);  // range [2, 4]
            auto z = x + y;
            test_taylor(z, three, value_t(7.0));
        }

        SECTION("operator-=(value_t)") {
            taylor_t value(one, three);
            value -= one;
            test_taylor(value, zero, two);
        }

        SECTION("operator-(Taylor), same operand cancels exactly") {
            taylor_t x(one, three);
            auto z = x - x;
            // x and -x share the same deviation variable (operator-()
            // negates coefficients in place, it does not mint a new
            // variable), so the coefficient cancels exactly, the same way
            // Affine's correlation tracking makes x - x collapse to 0.
            test_taylor(z, zero, zero);
        }

        SECTION("operator*=(value_t) on empty is a no-op") {
            taylor_t value;
            value *= two;
            REQUIRE(value.empty());
        }

        SECTION("operator*=(value_t) scales constant and coefficients") {
            taylor_t value(one, three);
            value *= two;
            test_taylor(value, two, value_t(6.0));
        }

        SECTION("operator*(Taylor), disjoint variables") {
            taylor_t x(one, three, order_t(2)); // const 2, coeff on vx^1 = 1
            taylor_t y(two, four, order_t(2));  // const 3, coeff on vy^1 = 1
            auto z = x * y;
            REQUIRE(z.order() == 2);
            REQUIRE(z.constant() == value_t(6.0));
            REQUIRE(z.coefficients().size() == 3); // vx, vy, vx*vy
            test_taylor(z, zero, value_t(12.0));
        }

        SECTION("operator*(Taylor), mixed orders drop the cross term") {
            taylor_t x(one, three, order_t(2));
            taylor_t y(two, four, order_t(1));
            auto z = x * y;
            REQUIRE(z.order() == 1);
            REQUIRE(z.constant() == value_t(6.0));
            REQUIRE(z.coefficients().size() == 2); // cross term dropped
            test_taylor(z, one, value_t(11.0));
        }

        SECTION("scalar * Taylor") {
            taylor_t value(one, three);
            auto z = two * value;
            test_taylor(z, two, value_t(6.0));
        }
    }

    SECTION("derivative") {
        auto v           = taylor_t::make_deviation();
        using monomial_t = typename taylor_t::monomial_t;
        // 5 + 2*v + 3*v^2, order 2
        coeffs_t coeffs{{monomial_t(v, 1), two}, {monomial_t(v, 2), three}};
        taylor_t p(five, coeffs, order_t(2));

        auto d = p.derivative(v);
        // d/dv (5 + 2v + 3v^2) = 2 + 6v
        REQUIRE(d.order() == 2);
        REQUIRE(d.constant() == two);
        REQUIRE(d.coefficients().size() == 1);
        REQUIRE(d.coefficients().at(monomial_t(v, 1)) == value_t(6.0));
    }

    SECTION("truncate") {
        auto v           = taylor_t::make_deviation();
        using monomial_t = typename taylor_t::monomial_t;
        coeffs_t coeffs{{monomial_t(v, 1), two}, {monomial_t(v, 2), three}};
        taylor_t p(five, coeffs, order_t(2));

        auto t = p.truncate(1);
        REQUIRE(t.order() == 1);
        REQUIRE(t.coefficients().size() == 1);
        REQUIRE(t.coefficients().at(monomial_t(v, 1)) == two);

        auto same = p.truncate(5); // raising the order drops nothing
        REQUIRE(same.order() == 5);
        REQUIRE(same.coefficients().size() == 2);
    }

    SECTION("Order 3") {
        // Every other section above tops out at order 2 (the default),
        // where multiplication's cross-term degree (1 + 1 = 2) never
        // exceeds the order, so a genuine order-3 term never actually
        // survives an operation. This section exercises order 3
        // specifically, so cubic terms are exercised end-to-end instead of
        // always landing exactly on the truncation boundary.
        using monomial_t = typename taylor_t::monomial_t;

        SECTION("Construction preserves order 3") {
            taylor_t value(one, three, order_t(3));
            REQUIRE(value.order() == 3);
            test_taylor(value, one, three);
        }

        SECTION("operator*(Taylor) retains a genuine cubic cross term") {
            // x = 2 + v + 0.5*v^2 (order 3)
            auto vx = taylor_t::make_deviation();
            coeffs_t x_coeffs{{monomial_t(vx, 1), one},
                              {monomial_t(vx, 2), value_t(0.5)}};
            taylor_t x(two, x_coeffs, order_t(3));

            // y = 3 + w (order 3)
            auto vy = taylor_t::make_deviation();
            coeffs_t y_coeffs{{monomial_t(vy, 1), one}};
            taylor_t y(three, y_coeffs, order_t(3));

            auto z = x * y;
            REQUIRE(z.order() == 3);
            REQUIRE(z.constant() == value_t(6.0));
            // vx, vx^2, vy, vx*vy, vx^2*vy: the last is the degree-3 cross
            // term (deg(vx^2) + deg(vy) == 3) that a lower order would drop.
            REQUIRE(z.coefficients().size() == 5);

            monomial_t cubic_cross = monomial_t(vx, 2) * monomial_t(vy, 1);
            REQUIRE(cubic_cross.degree() == 3);
            REQUIRE(z.coefficients().at(cubic_cross) == value_t(0.5));
        }

        SECTION("operator*(Taylor), order 3 vs order 2 drops the cubic "
                "cross term") {
            auto vx = taylor_t::make_deviation();
            coeffs_t x_coeffs{{monomial_t(vx, 1), one},
                              {monomial_t(vx, 2), value_t(0.5)}};
            taylor_t x(two, x_coeffs, order_t(3));
            taylor_t y(two, four, order_t(2));

            auto z = x * y;
            REQUIRE(z.order() == 2);
            // The vx^2 term of x is itself kept (degree 2 <= 2), but
            // vx^2 * vy (degree 3) is not.
            REQUIRE(z.coefficients().size() == 4);
        }

        SECTION("derivative, truncate, and bound of a cubic polynomial") {
            auto v = taylor_t::make_deviation();
            // p = 5 + 2v + 3v^2 + 4v^3, order 3
            coeffs_t coeffs{{monomial_t(v, 1), two},
                            {monomial_t(v, 2), three},
                            {monomial_t(v, 3), four}};
            taylor_t p(five, coeffs, order_t(3));

            SECTION("derivative") {
                auto d = p.derivative(v);
                // d/dv (5 + 2v + 3v^2 + 4v^3) = 2 + 6v + 12v^2
                REQUIRE(d.order() == 3);
                REQUIRE(d.constant() == two);
                REQUIRE(d.coefficients().size() == 2);
                REQUIRE(d.coefficients().at(monomial_t(v, 1)) == value_t(6.0));
                REQUIRE(d.coefficients().at(monomial_t(v, 2)) == value_t(12.0));
            }

            SECTION("truncate") {
                auto t2 = p.truncate(2);
                REQUIRE(t2.order() == 2);
                REQUIRE(t2.coefficients().size() == 2);
                REQUIRE(t2.coefficients().count(monomial_t(v, 3)) == 0);

                auto t3 = p.truncate(3); // no-op, nothing exceeds order 3
                REQUIRE(t3.order() == 3);
                REQUIRE(t3.coefficients().size() == 3);

                auto t4 = p.truncate(4); // raising the order drops nothing
                REQUIRE(t4.order() == 4);
                REQUIRE(t4.coefficients().size() == 3);
            }

            SECTION("bound, quadratic fast bounder") {
                // v and v^2 are v's diagonal linear/quadratic coefficients
                // (lin = 2, quad = 3), so QFB bounds 2v + 3v^2 exactly by
                // completing the square: convex (quad > 0), vertex at
                // v* = -lin/(2*quad) = -1/3 (inside [-1,1]), so the minimum
                // is f(v*) = -lin^2/(4*quad) = -1/3, and the maximum is at
                // the endpoint v=1: f(1) = quad + lin = 5.
                // v^3 (degree 3, not captured by QFB's diagonal) falls back
                // to the naive per-term bound: 4*[-1,1] = [-4,4].
                // Total: 5 + [-1/3, 5] + [-4, 4] = [5 - 1/3 - 4, 5 + 5 + 4]
                //   = [2/3, 14]
                test_taylor(p, value_t(2.0 / 3.0), value_t(14.0));
            }

            SECTION("bound, QFB is tighter than the naive baseline") {
                // The naive per-term baseline (bound each of v, v^2, v^3
                // separately) gives 5 +/- 2*[-1,1] +/- 3*[0,1] +/- 4*[-1,1]
                //   = [5 - 2 + 0 - 4, 5 + 2 + 3 + 4] = [-1, 14].
                // QFB tightens the lower bound to 2/3 by evaluating v's
                // linear and quadratic contribution together instead of
                // separately -- see docs/taylor.md's "Dominated and fast
                // bounders" section.
                REQUIRE(p.bound().lower() > value_t(-1.0));
            }
        }
    }

    SECTION("Comparison Operators") {
        taylor_t x(one, three);
        taylor_t same_x(one, three);
        taylor_t copy_of_x(x);

        REQUIRE(x == copy_of_x);
        REQUIRE_FALSE(x != copy_of_x);

        // Equality is structural (same coefficients on the same monomials),
        // not value-based: same_x mints its own deviation variable, so it
        // is not == x even though the two ranges match.
        REQUIRE(x != same_x);

        taylor_t different_order(one, three, order_t(3));
        REQUIRE(x != different_order);

        taylor_t empty1, empty2;
        REQUIRE(empty1 == empty2);
    }

    SECTION("make_deviation returns unique ids") {
        auto v0 = taylor_t::make_deviation();
        auto v1 = taylor_t::make_deviation();
        REQUIRE(v0 != v1);
    }

    SECTION("Stream Insertion / Printing") {
        taylor_t value(one, three);

        std::stringstream ss;
        ss << value;
        REQUIRE_FALSE(ss.str().empty());

        REQUIRE_FALSE(value.print_polynomial_form().empty());
        REQUIRE_FALSE(value.print_interval_form().empty());

        taylor_t empty;
        REQUIRE(empty.print_polynomial_form() == "∅");
    }
}
