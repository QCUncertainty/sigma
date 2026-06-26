#include "catch2/catch_test_macros.hpp"
#include "testing.hpp"
#include <sigma/affine/thresholded_affine.hpp>
#include <sstream>

using testing::test_affine;

// Convenience aliases
using TAFloat  = sigma::TAFloat;
using TADouble = sigma::TADouble;

// Helper: verify that a ThresholdedAffine's range contains the equivalent
// Affine range. Note that dropping terms is not conservative in general; this
// holds only when nothing significant is dropped (small thresholds / inputs).
template<typename TA>
void contains_affine_range(const TA& ta,
                           const sigma::Affine<typename TA::value_t>& a) {
    REQUIRE(ta.contains(a.range()));
}

TEMPLATE_TEST_CASE("ThresholdedAffine", "", TAFloat, TADouble) {
    using ta_t        = TestType;
    using value_t     = typename ta_t::value_t;
    using affine_t    = sigma::Affine<value_t>;
    using interval_t  = typename ta_t::interval_t;
    using threshold_t = typename ta_t::Threshold;

    const value_t zero  = value_t(0.0);
    const value_t one   = value_t(1.0);
    const value_t two   = value_t(2.0);
    const value_t three = value_t(3.0);
    const value_t four  = value_t(4.0);

    // A threshold of zero so that no error terms are ever dropped.
    const threshold_t no_drop{value_t(0.0)};
    // A threshold high enough to drop almost everything.
    const threshold_t big_t{value_t(0.5)};
    // An explicit threshold of 1% used by several cases below.
    const threshold_t def_t{value_t(0.01)};

    SECTION("Constructors") {
        SECTION("Default") {
            ta_t empty;
            REQUIRE(empty.empty());
        }

        SECTION("From Center") {
            ta_t value(one);
            test_affine(value, one, one);

            ta_t value2(zero);
            test_affine(value2, zero, zero);
        }

        SECTION("From Lower and Upper") {
            ta_t value(one, two);
            test_affine(value, one, two);

            ta_t point(one, one);
            test_affine(point, one, one);
        }

        SECTION("From Interval") {
            ta_t empty_int(interval_t{});
            REQUIRE(empty_int.empty());

            ta_t value(interval_t(one, two));
            test_affine(value, one, two);

            ta_t value2(interval_t(-two, three));
            test_affine(value2, -two, three);
        }

        SECTION("From Center and Error Terms") {
            auto sym = affine_t::make_error_term();
            ta_t value(one, {{sym, one}});
            test_affine(value, zero, two);
        }

        SECTION("Copy Constructor") {
            ta_t empty;
            ta_t copy_empty(empty);
            REQUIRE(copy_empty.empty());
            REQUIRE(copy_empty == empty);

            ta_t point(one);
            ta_t copy_point(point);
            test_affine(copy_point, one, one);
            REQUIRE(copy_point == point);

            ta_t interval(one, two);
            ta_t copy_interval(interval);
            test_affine(copy_interval, one, two);
            REQUIRE(copy_interval == interval);
        }

        SECTION("Move Constructor") {
            ta_t empty;
            ta_t move_empty(std::move(empty));
            REQUIRE(move_empty.empty());

            ta_t point(one);
            ta_t move_point(std::move(point));
            test_affine(move_point, one, one);
        }
    }

    SECTION("Accessors") {
        ta_t interval(one, two);
        REQUIRE(interval.center() == value_t(1.5));
        REQUIRE(interval.radius() == value_t(0.5));
        REQUIRE_FALSE(interval.empty());
        REQUIRE(interval.threshold() == value_t(0.001));
    }

    SECTION("Threshold") {
        SECTION("No dropping for a single-term form") {
            // A single-term form has relative contribution 1.0, never dropped.
            ta_t x(one, two, def_t);
            // Should have exactly one error term (the interval's epsilon).
            auto terms = x.error_terms();
            // Count non-zero terms
            std::size_t nonzero = 0;
            for(auto&& [sym, coeff] : terms) {
                if(coeff != value_t(0)) ++nonzero;
            }
            REQUIRE(nonzero == 1);
        }

        SECTION("Dropping reduces term count after multiplication") {
            // x in [1, 2], y in [1, 2] — multiply to get nonlinearity term.
            // With a tight threshold the nonlinearity correction (0.25) is
            // small relative to the main terms (~0.5 each), so it is dropped
            // at threshold > ~0.2.
            ta_t x(one, two, big_t);
            ta_t y(one, two, big_t);
            ta_t z = x * y;

            // The corresponding Affine result
            affine_t ax(one, two), ay(one, two);
            affine_t az = ax * ay;

            // With a big threshold the new nonlinearity term is dropped, so z
            // has no more unique symbols than az.
            REQUIRE(z.error_terms().size() <= az.error_terms().size());
        }

        SECTION("Dropping keeps the term count bounded") {
            // Start with [1, 2] and multiply repeatedly; each multiplication
            // adds a nonlinearity term.  With a high threshold, these are
            // dropped, so the total term count stays small.
            ta_t x(one, two, big_t);
            std::size_t prev_count = x.error_terms().size();
            for(int i = 0; i < 5; ++i) {
                ta_t bump(value_t(1.0), value_t(1.01), big_t);
                x *= bump;
                // Symbol count should stay bounded (≤ prev + 1 at most,
                // and often stays the same because terms are dropped).
                REQUIRE(x.error_terms().size() <= prev_count + 2);
                prev_count = x.error_terms().size();
            }
        }
    }

    SECTION("range contains Affine range when nothing significant is dropped") {
        affine_t ax(one, two), ay(two, three);
        ta_t tx(one, two, def_t), ty(two, three, def_t);

        SECTION("Addition") { contains_affine_range(tx + ty, ax + ay); }
        SECTION("Subtraction") { contains_affine_range(tx - ty, ax - ay); }
        SECTION("Multiplication") { contains_affine_range(tx * ty, ax * ay); }
        SECTION("Division") { contains_affine_range(tx / ty, ax / ay); }
        SECTION("sqrt") {
            contains_affine_range(sigma::sqrt(tx), sigma::sqrt(ax));
        }
        SECTION("exp") {
            contains_affine_range(sigma::exp(tx), sigma::exp(ax));
        }
        SECTION("log") {
            contains_affine_range(sigma::log(tx), sigma::log(ax));
        }
        SECTION("abs") {
            ta_t tneg(-two, -one, def_t);
            affine_t aneg(-two, -one);
            contains_affine_range(sigma::abs(tneg), sigma::abs(aneg));
        }
    }

    SECTION("Arithmetic") {
        SECTION("Scalar add") {
            ta_t x(one, two, no_drop);
            x += two;
            test_affine(x, three, four);
        }

        SECTION("Scalar subtract") {
            ta_t x(one, two, no_drop);
            x -= one;
            test_affine(x, zero, one);
        }

        SECTION("Scalar multiply") {
            ta_t x(one, two, no_drop);
            x *= two;
            test_affine(x, two, four);
        }

        SECTION("Scalar divide") {
            ta_t x(two, four, no_drop);
            x /= two;
            test_affine(x, one, two);
        }

        SECTION("Affine add") {
            ta_t x(one, two, no_drop), y(two, three, no_drop);
            test_affine(x + y, three, value_t(5.0));
        }

        SECTION("Unary minus") {
            ta_t x(one, two, no_drop);
            test_affine(-x, -two, -one);
        }

        SECTION("Dependent subtraction: x - x") {
            // With no_drop, no terms are dropped, so x shares its error symbol
            // with its copy and x - x = 0 exactly (same as Affine).
            ta_t x(one, two, no_drop);
            ta_t result = x - x;
            REQUIRE(result.radius() == zero);
        }

        SECTION("Scalar right multiply") {
            ta_t x(one, two, no_drop);
            ta_t result = two * x;
            test_affine(result, two, four);
        }
    }

    SECTION("Comparison") {
        ta_t x(one, two);
        ta_t y = x; // copy shares same error symbols
        ta_t z(two, three);
        REQUIRE(x == y);
        REQUIRE_FALSE(x == z);
        REQUIRE(x != z);
    }

    SECTION("Contains") {
        ta_t x(one, two, no_drop);
        REQUIRE(x.contains(one));
        REQUIRE(x.contains(two));
        REQUIRE(x.contains(value_t(1.5)));
        REQUIRE_FALSE(x.contains(zero));
        REQUIRE_FALSE(x.contains(three));

        REQUIRE(x.contains(interval_t(one, two)));
        REQUIRE(x.contains(interval_t(one, value_t(1.5))));
        REQUIRE_FALSE(x.contains(interval_t(zero, two)));

        ta_t y(one, value_t(1.5), no_drop);
        REQUIRE(x.contains(y));
        ta_t z(zero, two, no_drop);
        REQUIRE_FALSE(x.contains(z));
    }

    SECTION("Empty form behavior") {
        ta_t empty;
        REQUIRE(empty.empty());
        REQUIRE(empty.range().empty());
        REQUIRE_FALSE(empty.contains(zero));
    }

    SECTION("Stream output") {
        ta_t x(one, two, no_drop);
        std::ostringstream os;
        os << x;
        REQUIRE_FALSE(os.str().empty());
    }

    SECTION("Operations") {
        ta_t x(one, two, no_drop);

        SECTION("sqrt") {
            test_affine(sigma::sqrt(x), one, value_t(1.43198051533946));
        }

        SECTION("exp") {
            // Tight value is [e, e^2] = [2.71828182846..., 7.38905609893...],
            // but the affine transformation adds some error.
            test_affine(sigma::exp(x), value_t(2.14236806757880416),
                        value_t(7.38905609893));
        }

        SECTION("log") {
            test_affine(sigma::log(x), value_t(0.0), value_t(0.752807281701));
        }

        SECTION("abs positive") {
            auto result = sigma::abs(x);
            REQUIRE(result == x);
        }

        SECTION("abs negative") {
            ta_t neg(-two, -one, no_drop);
            auto result = sigma::abs(neg);
            test_affine(result, one, two);
        }

        SECTION("fabs") {
            ta_t neg(-two, -one, no_drop);
            auto result = sigma::fabs(neg);
            test_affine(result, one, two);
        }

        SECTION("pow") {
            auto result = sigma::pow(x, value_t(2.0));
            REQUIRE(result.contains(one));
            REQUIRE(result.contains(four));
        }
    }

    SECTION("Dropping behavior") {
        SECTION("Nothing dropped when threshold is zero") {
            // With the no_drop threshold every error term is kept, so x - x
            // cancels exactly (all error terms are shared tracked symbols).
            ta_t x(one, two, no_drop);
            ta_t y    = x;
            ta_t diff = y - x;
            REQUIRE(diff.radius() == zero);
        }

        SECTION("Sub-threshold terms are discarded") {
            // sym2's relative contribution is 0.05/1.05 ≈ 4.8%, below the 10%
            // threshold, so it is dropped on construction. Only sym1 survives
            // and the radius reflects the kept term alone.
            const threshold_t T{value_t(0.1)};
            auto sym1 = affine_t::make_error_term();
            auto sym2 = affine_t::make_error_term();
            ta_t x(
              value_t(2.5),
              typename ta_t::error_terms_t{{sym1, one}, {sym2, value_t(0.05)}},
              T);
            REQUIRE(x.error_terms().size() == 1);
            REQUIRE(x.radius() == one);
        }

        SECTION("Dropping is not conservative") {
            // Because dropped terms are discarded rather than retained in a
            // lump, a ThresholdedAffine range may be SMALLER than the
            // corresponding Affine range — the upper-bound guarantee does not
            // hold for the drop strategy.
            //
            // Trace (threshold = 10%):
            //   a: center=2.5, {sym1: 1.0, sym2: 0.05}; sym2 dropped on
            //      construction (0.05/1.05 ≈ 4.8% < 10%) → {sym1: 1.0}
            //   b = a (copy), scaled by 0.9 → {sym1: 0.9}
            //   c = [-0.03, 0.03]; on b += c the term 0.03/0.93 ≈ 3.2% < 10%
            //      is dropped → b stays {sym1: 0.9}
            //   diff = a - b = {sym1: 0.1} → radius = 0.1
            //
            // Plain Affine (every term tracked):
            //   diff: {sym1: 0.10, sym2: 0.005, ε_c: -0.03} → radius = 0.135
            //
            // 0.1 < 0.135: the discarded uncertainty shrinks the range.

            const value_t R     = value_t(1.0);
            const value_t ds    = value_t(0.05); // dropped: 0.05/1.05 ≈ 4.8%
            const value_t alpha = value_t(0.9);
            const value_t dn    = value_t(0.03); // dropped from b: ≈ 3.2%
            const threshold_t T{value_t(0.1)};

            auto sym1 = affine_t::make_error_term();
            auto sym2 = affine_t::make_error_term();

            // ThresholdedAffine path: sym2 is dropped on construction.
            ta_t a_ta(value_t(2.5),
                      typename ta_t::error_terms_t{{sym1, R}, {sym2, ds}}, T);
            ta_t b_ta = a_ta;
            b_ta *= alpha;
            ta_t c_ta(-dn, dn, T); // single-term interval; not dropped within c
            b_ta += c_ta;          // c's term dropped on combine
            ta_t diff_ta = a_ta - b_ta;

            // Plain Affine path: every term tracked individually, no dropping.
            affine_t a_af(value_t(2.5), {{sym1, R}, {sym2, ds}});
            affine_t b_af = a_af * alpha;
            affine_t c_af(-dn, dn);
            affine_t diff_af = a_af - (b_af + c_af);

            // Dropping discarded uncertainty, so the thresholded radius is the
            // smaller of the two.
            REQUIRE(diff_ta.radius() < diff_af.radius());
        }
    }
}
