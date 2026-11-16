#include "testing.hpp"
#include <cmath>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <vector>

using testing::test_invariant;
using testing::test_triplex;

TEMPLATE_TEST_CASE("Triplex", "", TRIPLEX_TYPES) {
    using testing_t  = TestType;
    using value_t    = typename testing_t::value_t;
    using interval_t = typename testing_t::interval_t;

    constexpr auto inf = std::numeric_limits<value_t>::infinity();
    constexpr auto nan = std::numeric_limits<value_t>::quiet_NaN();

    testing_t defaulted;
    testing_t point(value_t{2});
    testing_t x(value_t{1}, value_t{2}, value_t{5});
    testing_t y(value_t{-1}, value_t{1}, value_t{3});

    SECTION("Ctors") {
        SECTION("Default") { REQUIRE(defaulted.empty()); }

        SECTION("Value") {
            REQUIRE_FALSE(point.empty());
            REQUIRE(point.lower() == value_t{2});
            REQUIRE(point.nominal() == value_t{2});
            REQUIRE(point.upper() == value_t{2});

            // A single value converts implicitly
            testing_t converted = value_t{3};
            REQUIRE(converted == testing_t(value_t{3}, value_t{3}, value_t{3}));

            // ...but there is no finite nominal value for a non-finite value
            REQUIRE_THROWS_AS(testing_t(inf), std::domain_error);
            REQUIRE_THROWS_AS(testing_t(-inf), std::domain_error);
            REQUIRE_THROWS_AS(testing_t(nan), std::domain_error);
        }

        SECTION("Bounds and nominal value") {
            REQUIRE(x.lower() == value_t{1});
            REQUIRE(x.nominal() == value_t{2});
            REQUIRE(x.upper() == value_t{5});

            // The nominal value may sit on a closed bound
            testing_t on_lower(value_t{1}, value_t{1}, value_t{5});
            REQUIRE(on_lower.nominal() == value_t{1});
            testing_t on_upper(value_t{1}, value_t{5}, value_t{5});
            REQUIRE(on_upper.nominal() == value_t{5});

            // Unbounded intervals are fine, as long as the nominal value is
            // finite
            testing_t unbounded(-inf, value_t{0}, inf);
            REQUIRE(unbounded.nominal() == value_t{0});

            // Nominal value outside the bounds
            REQUIRE_THROWS_AS(testing_t(value_t{1}, value_t{0}, value_t{5}),
                              std::domain_error);
            REQUIRE_THROWS_AS(testing_t(value_t{1}, value_t{6}, value_t{5}),
                              std::domain_error);

            // Nothing is reordered: Interval would sort (5, 1) into [1, 5]
            REQUIRE_THROWS_AS(testing_t(value_t{5}, value_t{2}, value_t{1}),
                              std::domain_error);

            // Non-finite nominal values
            REQUIRE_THROWS_AS(testing_t(-inf, inf, inf), std::domain_error);
            REQUIRE_THROWS_AS(testing_t(-inf, -inf, inf), std::domain_error);
            REQUIRE_THROWS_AS(testing_t(-inf, nan, inf), std::domain_error);
        }

        SECTION("Interval and nominal value") {
            testing_t from_interval(interval_t(value_t{1}, value_t{5}),
                                    value_t{2});
            REQUIRE(from_interval == x);

            REQUIRE_THROWS_AS(
              testing_t(interval_t(value_t{1}, value_t{5}), value_t{6}),
              std::domain_error);
            REQUIRE_THROWS_AS(testing_t(interval_t(), value_t{0}),
                              std::domain_error);
        }

        SECTION("No construction from an interval alone") {
            STATIC_REQUIRE_FALSE(
              std::is_constructible_v<testing_t, interval_t>);
            STATIC_REQUIRE_FALSE(std::is_convertible_v<interval_t, testing_t>);
            STATIC_REQUIRE(std::is_convertible_v<value_t, testing_t>);
        }

        SECTION("Copy") {
            testing_t copy(x);
            REQUIRE(copy == x);
        }
    }

    SECTION("Accessors") {
        SECTION("width and median are those of the bounds") {
            REQUIRE(x.width() == value_t{4});
            REQUIRE(x.median() == value_t{3});
            // ...and the median is not the nominal value
            REQUIRE(x.median() != x.nominal());
        }

        SECTION("Radii") {
            REQUIRE(x.lower_radius() == value_t{1});
            REQUIRE(x.upper_radius() == value_t{3});
            REQUIRE(x.lower_radius() + x.upper_radius() == x.width());
            REQUIRE(point.lower_radius() == value_t{0});
            REQUIRE(point.upper_radius() == value_t{0});
        }

        SECTION("interval") {
            REQUIRE(x.interval() == interval_t(value_t{1}, value_t{5}));
            REQUIRE(defaulted.interval().empty());
        }

        SECTION("Empty triplex has no values") {
            REQUIRE_THROWS_AS(defaulted.lower(), std::domain_error);
            REQUIRE_THROWS_AS(defaulted.nominal(), std::domain_error);
            REQUIRE_THROWS_AS(defaulted.upper(), std::domain_error);
            REQUIRE_THROWS_AS(defaulted.width(), std::domain_error);
            REQUIRE_THROWS_AS(defaulted.median(), std::domain_error);
            REQUIRE_THROWS_AS(defaulted.lower_radius(), std::domain_error);
            REQUIRE_THROWS_AS(defaulted.upper_radius(), std::domain_error);
        }
    }

    SECTION("contains") {
        SECTION("Value") {
            REQUIRE(x.contains(value_t{1}));
            REQUIRE(x.contains(value_t{4}));
            REQUIRE(x.contains(value_t{5}));
            REQUIRE_FALSE(x.contains(value_t{0}));
            REQUIRE_FALSE(defaulted.contains(value_t{0}));
        }
        SECTION("Triplex") {
            // Only the bounds matter, not the nominal values
            REQUIRE(x.contains(testing_t(value_t{2}, value_t{4}, value_t{5})));
            REQUIRE(x.contains(x));
            REQUIRE(x.contains(defaulted));
            REQUIRE_FALSE(x.contains(y));
            REQUIRE_FALSE(defaulted.contains(x));
        }
    }

    SECTION("Arithmetic") {
        SECTION("Negation") {
            auto r = -x;
            REQUIRE(r == testing_t(value_t{-5}, value_t{-2}, value_t{-1}));
            REQUIRE((-defaulted).empty());
        }

        SECTION("Addition") {
            auto r = x + y;
            REQUIRE(r.interval() == x.interval() + y.interval());
            REQUIRE(r.nominal() == value_t{3});
            REQUIRE(r == testing_t(value_t{0}, value_t{3}, value_t{8}));

            REQUIRE(x + value_t{1} ==
                    testing_t(value_t{2}, value_t{3}, value_t{6}));
            REQUIRE(value_t{1} + x ==
                    testing_t(value_t{2}, value_t{3}, value_t{6}));

            testing_t z(x);
            z += y;
            REQUIRE(z == r);

            // As for intervals, the empty triplex is the additive identity
            REQUIRE(defaulted + x == x);
            REQUIRE(x + defaulted == x);
        }

        SECTION("Subtraction") {
            auto r = x - y;
            REQUIRE(r.interval() == x.interval() - y.interval());
            REQUIRE(r == testing_t(value_t{-2}, value_t{1}, value_t{6}));

            REQUIRE(x - value_t{1} ==
                    testing_t(value_t{0}, value_t{1}, value_t{4}));
            REQUIRE(value_t{1} - x ==
                    testing_t(value_t{-4}, value_t{-1}, value_t{0}));

            testing_t z(x);
            z -= y;
            REQUIRE(z == r);

            REQUIRE(defaulted - x == -x);
            REQUIRE(x - defaulted == x);
        }

        SECTION("Multiplication") {
            auto r = x * y;
            REQUIRE(r.interval() == x.interval() * y.interval());
            REQUIRE(r == testing_t(value_t{-5}, value_t{2}, value_t{15}));

            REQUIRE(x * value_t{2} ==
                    testing_t(value_t{2}, value_t{4}, value_t{10}));
            REQUIRE(value_t{2} * x ==
                    testing_t(value_t{2}, value_t{4}, value_t{10}));

            testing_t z(x);
            z *= y;
            REQUIRE(z == r);

            REQUIRE((defaulted * x).empty());
            REQUIRE((x * defaulted).empty());
        }

        SECTION("Division") {
            testing_t d(value_t{2}, value_t{4}, value_t{8});
            auto r = x / d;
            REQUIRE(r.interval() == x.interval() / d.interval());
            REQUIRE(r.nominal() == value_t{0.5});

            REQUIRE(x / value_t{2} ==
                    testing_t(value_t{0.5}, value_t{1}, value_t{2.5}));
            REQUIRE((value_t{8} / d).nominal() == value_t{2});

            testing_t z(x);
            z /= d;
            REQUIRE(z == r);

            REQUIRE((defaulted / x).empty());
            REQUIRE((x / defaulted).empty());

            // A divisor containing zero is an error, even with a nonzero
            // nominal value
            REQUIRE_THROWS_AS(x / y, std::domain_error);
            REQUIRE_THROWS_AS(x / value_t{0}, std::domain_error);
            // ...and the strong guarantee holds
            testing_t w(x);
            REQUIRE_THROWS_AS(w /= y, std::domain_error);
            REQUIRE(w == x);
        }

        SECTION("Self-assignment forms") {
            testing_t z(y);
            z *= z;
            REQUIRE(z == testing_t(value_t{-3}, value_t{1}, value_t{9}));
        }
    }

    SECTION("The nominal value is not affected by the dependency problem") {
        // From the design page: <-1, 1, 3> * <-1, 1, 3> = <-3, 1, 9>. The
        // median of the interval result, 3, drifts; the nominal value does not.
        auto sq = y * y;
        REQUIRE(sq == testing_t(value_t{-3}, value_t{1}, value_t{9}));
        REQUIRE(sq.median() == value_t{3});

        // X - X has the needlessly wide bounds of the interval case, but its
        // nominal value is exactly zero.
        auto diff = y - y;
        REQUIRE(diff == testing_t(value_t{-4}, value_t{0}, value_t{4}));
    }

    SECTION("Comparability with plain floating point") {
        // The nominal value of a triplex computation must be, bit for bit, the
        // same computation in plain value_t on the nominal values. The
        // reference is written one operation per statement so the compiler
        // cannot contract it into fused multiply-adds, which a triplex, one
        // operation at a time, never does either.
        testing_t a(value_t{0.5}, value_t{0.7}, value_t{0.9});
        testing_t b(value_t{1.1}, value_t{1.3}, value_t{2.0});

        auto r = sigma::exp(a) * sigma::sin(b) / (a + b) - sigma::sqrt(a * b) +
                 sigma::pow(a, 3) + sigma::log(b) -
                 sigma::atan(a) * sigma::cosh(b) + value_t{2} * sigma::tanh(a);

        const value_t am = a.nominal();
        const value_t bm = b.nominal();
        value_t t1       = std::exp(am);
        value_t t2       = std::sin(bm);
        value_t t3       = t1 * t2;
        value_t t4       = am + bm;
        value_t t5       = t3 / t4;
        value_t t6       = am * bm;
        value_t t7       = std::sqrt(t6);
        value_t t8       = t5 - t7;
        value_t t9       = std::pow(am, 3);
        value_t t10      = t8 + t9;
        value_t t11      = std::log(bm);
        value_t t12      = t10 + t11;
        value_t t13      = std::atan(am);
        value_t t14      = std::cosh(bm);
        value_t t15      = t13 * t14;
        value_t t16      = t12 - t15;
        value_t t17      = std::tanh(am);
        value_t t18      = value_t{2} * t17;
        value_t expected = t16 + t18;

        REQUIRE(r.nominal() == expected);
        test_invariant(r);
        // The bounds enclose the nominal value but are not centered on it
        REQUIRE(r.median() != r.nominal());
    }

    SECTION("Iterating on the nominal value iterates plain floating point") {
        // The fixed-point iteration x <- cos(x). A procedure that fed the
        // median back in would drift away from the plain result.
        testing_t t(value_t{0.5}, value_t{0.6}, value_t{1.0});
        value_t plain = value_t{0.6};
        for(int i = 0; i < 50; ++i) {
            t     = sigma::cos(t);
            plain = std::cos(plain);
            REQUIRE(t.nominal() == plain);
            test_invariant(t);
        }
    }

    SECTION("Non-finite nominal values") {
        // Plain floating point overflows here while the interval does not
        // fail. The overflowed nominal value is kept, and is not a member.
        constexpr auto big = std::numeric_limits<value_t>::max();
        testing_t huge(big);
        auto r = huge * value_t{2};
        REQUIRE(std::isinf(r.nominal()));
        REQUIRE(r.nominal() == value_t{2} * big);
        REQUIRE(std::isinf(r.upper()));
        REQUIRE_FALSE(r.contains(r.nominal()));

        // Further arithmetic carries the plain floating point result along
        // without throwing, e.g., inf - inf is NaN
        auto nan_nominal = r - r;
        REQUIRE(std::isnan(nan_nominal.nominal()));
    }

    SECTION("Comparisons") {
        SECTION("Equality") {
            REQUIRE(x == testing_t(value_t{1}, value_t{2}, value_t{5}));
            REQUIRE(defaulted == testing_t());
            // Same bounds, different nominal value
            REQUIRE(x != testing_t(value_t{1}, value_t{3}, value_t{5}));
            // Same nominal value, different bounds
            REQUIRE(x != testing_t(value_t{1}, value_t{2}, value_t{6}));
            REQUIRE(x != defaulted);
            REQUIRE(defaulted != x);
        }

        SECTION("Different value types") {
            using other_t =
              sigma::Triplex<std::conditional_t<std::is_same_v<value_t, float>,
                                                sigma::Interval<double>,
                                                sigma::Interval<float>>>;
            REQUIRE(x != other_t(1, 2, 5));
        }

        SECTION("Ordering is by nominal value") {
            // wide has the larger median but the smaller nominal value
            testing_t wide(value_t{0}, value_t{1}, value_t{10});
            testing_t narrow(value_t{1.5}, value_t{2}, value_t{2.5});
            REQUIRE(wide.median() > narrow.median());
            REQUIRE(wide < narrow);
            REQUIRE(narrow > wide);
            REQUIRE(wide <= narrow);
            REQUIRE(narrow >= wide);
            REQUIRE_FALSE(narrow < wide);
            REQUIRE(wide <= wide);
            REQUIRE(wide >= wide);

            REQUIRE(defaulted < wide);
            REQUIRE_FALSE(wide < defaulted);
            REQUIRE_FALSE(defaulted < defaulted);
        }

        SECTION("Ties in nominal value are broken by the bounds") {
            // Same nominal value, different bounds: one must precede the
            // other, or they would be equivalent without being equal
            testing_t narrow(value_t{-1}, value_t{0}, value_t{1});
            testing_t wide_below(value_t{-2}, value_t{0}, value_t{1});
            testing_t wide_above(value_t{-1}, value_t{0}, value_t{2});
            REQUIRE(wide_below < narrow);
            REQUIRE_FALSE(narrow < wide_below);
            REQUIRE(narrow < wide_above);
            REQUIRE_FALSE(wide_above < narrow);

            // ...but the nominal value still decides first
            testing_t later(value_t{-5}, value_t{0.5}, value_t{1});
            REQUIRE(narrow < later);

            std::set<testing_t> keys{narrow, wide_below, wide_above, narrow};
            REQUIRE(keys.size() == 3);
        }

        SECTION("Equivalent under < exactly when ==") {
            std::vector<testing_t> values{
              defaulted,
              x,
              y,
              testing_t(value_t{-1}, value_t{0}, value_t{1}),
              testing_t(value_t{-2}, value_t{0}, value_t{1}),
              testing_t(value_t{-1}, value_t{0}, value_t{2}),
              testing_t(value_t{0}),
              testing_t(value_t{1}, value_t{3}, value_t{5})};
            for(const auto& a : values) {
                for(const auto& b : values) {
                    const bool equivalent = !(a < b) && !(b < a);
                    REQUIRE(equivalent == (a == b));
                    REQUIRE((a <= b) == (a < b || a == b));
                    REQUIRE((a >= b) == (b < a || a == b));
                }
            }
        }
    }

    SECTION("Hashing") {
        std::hash<testing_t> hasher;
        REQUIRE(hasher(defaulted) == 0);
        REQUIRE(hasher(x) == hasher(testing_t(x)));
        REQUIRE(hasher(x) !=
                hasher(testing_t(value_t{1}, value_t{3}, value_t{5})));

        std::unordered_set<testing_t> set{x, y, x};
        REQUIRE(set.size() == 2);
    }

    SECTION("Printing") {
        std::stringstream ss;
        ss << x;
        REQUIRE(ss.str() == "[1, 2, 5]");

        std::stringstream empty_ss;
        empty_ss << defaulted;
        REQUIRE(empty_ss.str() == "∅");
    }
}

TEMPLATE_TEST_CASE("Triplex with possibly open bounds", "", sigma::GIFloat,
                   sigma::GIDouble) {
    using interval_t = TestType;
    using testing_t  = sigma::Triplex<interval_t>;
    using value_t    = typename testing_t::value_t;

    constexpr auto inf = std::numeric_limits<value_t>::infinity();

    testing_t half_open(value_t{0}, value_t{1}, value_t{4}, true);
    testing_t open(value_t{0}, value_t{1}, value_t{4}, true, true);

    SECTION("Ctors") {
        REQUIRE(half_open.left_open());
        REQUIRE_FALSE(half_open.left_closed());
        REQUIRE(half_open.right_closed());
        REQUIRE_FALSE(half_open.right_open());
        REQUIRE(open.left_open());
        REQUIRE(open.right_open());

        // The three-value form has closed bounds
        testing_t closed(value_t{0}, value_t{1}, value_t{4});
        REQUIRE(closed.left_closed());
        REQUIRE(closed.right_closed());

        // The nominal value may not sit on an open bound
        REQUIRE_THROWS_AS(testing_t(value_t{0}, value_t{0}, value_t{4}, true),
                          std::domain_error);
        REQUIRE_THROWS_AS(
          testing_t(value_t{0}, value_t{4}, value_t{4}, false, true),
          std::domain_error);
        // ...nor in an interval that is empty because a point is open
        REQUIRE_THROWS_AS(testing_t(value_t{1}, value_t{1}, value_t{1}, true),
                          std::domain_error);

        // An infinite bound is open
        testing_t unbounded(-inf, value_t{0}, inf);
        REQUIRE(unbounded.left_open());
        REQUIRE(unbounded.right_open());
    }

    SECTION("Openness flags exist only for general intervals") {
        STATIC_REQUIRE(std::is_constructible_v<testing_t, value_t, value_t,
                                               value_t, bool, bool>);
        STATIC_REQUIRE_FALSE(
          std::is_constructible_v<sigma::Triplex<sigma::Interval<value_t>>,
                                  value_t, value_t, value_t, bool, bool>);
    }

    SECTION("Openness participates in equality and hashing") {
        testing_t closed(value_t{0}, value_t{1}, value_t{4});
        REQUIRE(half_open != closed);
        REQUIRE(half_open != open);
        REQUIRE(half_open == testing_t(interval_t(value_t{0}, value_t{4}, true),
                                       value_t{1}));
        std::hash<testing_t> hasher;
        REQUIRE(hasher(half_open) != hasher(closed));
    }

    SECTION("Openness breaks the last ties in the ordering") {
        // Same three values; a closed bound sorts before an open one
        testing_t closed(value_t{0}, value_t{1}, value_t{4});
        testing_t right_open(value_t{0}, value_t{1}, value_t{4}, false, true);
        REQUIRE(closed < right_open);
        REQUIRE(right_open < half_open);
        REQUIRE(half_open < open);
        REQUIRE_FALSE(open < half_open);

        std::vector<testing_t> values{closed, right_open, half_open, open};
        for(const auto& a : values) {
            for(const auto& b : values) {
                REQUIRE((!(a < b) && !(b < a)) == (a == b));
            }
        }
    }

    SECTION("Arithmetic keeps openness from the interval") {
        auto r = half_open + value_t{1};
        REQUIRE(r.left_open());
        REQUIRE(r.right_closed());
        REQUIRE(r.nominal() == value_t{2});

        auto n = -half_open;
        REQUIRE(n.left_closed());
        REQUIRE(n.right_open());
    }

    SECTION("Rounding can land the nominal value on an open bound") {
        // (0, 1] + [1, 2] = (1, 3]: the lower bound 0 + 1 is exact and open.
        // The nominal value is 2^-100 + 1, which rounds to exactly 1, the open
        // bound. The bound is closed to keep the nominal value a member; the
        // nominal value stays put.
        const value_t tiny = std::ldexp(value_t{1}, -100);
        testing_t a(value_t{0}, tiny, value_t{1}, true);
        testing_t b(value_t{1}, value_t{1}, value_t{2});
        REQUIRE((a.interval() + b.interval()).left_open());

        auto r = a + b;
        REQUIRE(r.nominal() == value_t{1});
        REQUIRE(r.lower() == value_t{1});
        REQUIRE(r.left_closed());
        REQUIRE(r.upper() == value_t{3});
        REQUIRE(r.right_closed());
    }

    SECTION(
      "Elementary functions accept an open bound the nominal value avoids") {
        // log is defined on (0, 4] even though it is not on [0, 4]
        auto r = sigma::log(half_open);
        REQUIRE(r.nominal() == std::log(value_t{1}));
        REQUIRE(std::isinf(r.lower()));
        test_invariant(r);
    }

    SECTION("Printing") {
        std::stringstream ss;
        ss << half_open;
        REQUIRE(ss.str() == "(0, 1, 4]");

        std::stringstream open_ss;
        open_ss << open;
        REQUIRE(open_ss.str() == "(0, 1, 4)");
    }
}
