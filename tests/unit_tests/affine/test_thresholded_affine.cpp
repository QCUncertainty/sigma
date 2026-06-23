#include "catch2/catch_test_macros.hpp"
#include "testing.hpp"
#include <sigma/affine/thresholded_affine.hpp>
#include <sstream>

using testing::test_affine;

// Convenience aliases
using TAFloat  = sigma::TAFloat;
using TADouble = sigma::TADouble;

// Helper: verify that a ThresholdedAffine's range contains the equivalent
// Affine range. Used to assert conservatism.
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

    // A threshold low enough that single-term forms never get lumped.
    const threshold_t no_lump{value_t(0.0)};
    // A threshold high enough to lump almost everything.
    const threshold_t big_t{value_t(0.5)};
    // The default threshold (1%).
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
        REQUIRE(interval.threshold() == value_t(0.01));
        REQUIRE(interval.lump_radius() == value_t(0));
    }

    SECTION("Threshold") {
        SECTION("No lumping for a single-term form") {
            // A single-term form has relative contribution 1.0, never lumped.
            ta_t x(one, two, def_t);
            // Should have exactly one error term (the interval's epsilon).
            // The lump term should NOT be present (or have zero coefficient).
            auto terms = x.error_terms();
            // Count non-zero terms
            std::size_t nonzero = 0;
            for(auto&& [sym, coeff] : terms) {
                if(coeff != value_t(0)) ++nonzero;
            }
            REQUIRE(nonzero == 1);
        }

        SECTION("Lumping reduces term count after multiplication") {
            // x in [1, 2], y in [1, 2] — multiply to get nonlinearity term.
            // With a tight threshold the nonlinearity correction (0.25) is
            // small relative to the main terms (~0.5 each), so it may be
            // lumped at threshold > ~0.2.
            ta_t x(one, two, big_t);
            ta_t y(one, two, big_t);
            ta_t z = x * y;

            // The corresponding Affine result
            affine_t ax(one, two), ay(one, two);
            affine_t az = ax * ay;

            // ThresholdedAffine result must contain the Affine result.
            contains_affine_range(z, az);

            // With big threshold, the new nonlinearity term is likely lumped.
            // Verify that z has fewer unique symbols than az (or at most
            // equal).
            REQUIRE(z.error_terms().size() <= az.error_terms().size());
        }

        SECTION("Lumped lump term grows monotonically") {
            // Start with [1, 2] and multiply repeatedly; each multiplication
            // adds a nonlinearity term.  With a high threshold, these get
            // absorbed into the lump, so the total term count stays small.
            ta_t x(one, two, big_t);
            std::size_t prev_count = x.error_terms().size();
            for(int i = 0; i < 5; ++i) {
                ta_t bump(value_t(1.0), value_t(1.01), big_t);
                x *= bump;
                // Symbol count should stay bounded (≤ prev + 1 at most,
                // and often stays the same due to lumping).
                REQUIRE(x.error_terms().size() <= prev_count + 2);
                prev_count = x.error_terms().size();
            }
        }
    }

    SECTION("Conservatism: range always contains Affine range") {
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
            ta_t x(one, two, no_lump);
            x += two;
            test_affine(x, three, four);
        }

        SECTION("Scalar subtract") {
            ta_t x(one, two, no_lump);
            x -= one;
            test_affine(x, zero, one);
        }

        SECTION("Scalar multiply") {
            ta_t x(one, two, no_lump);
            x *= two;
            test_affine(x, two, four);
        }

        SECTION("Scalar divide") {
            ta_t x(two, four, no_lump);
            x /= two;
            test_affine(x, one, two);
        }

        SECTION("Affine add") {
            ta_t x(one, two, no_lump), y(two, three, no_lump);
            test_affine(x + y, three, value_t(5.0));
        }

        SECTION("Unary minus") {
            ta_t x(one, two, no_lump);
            test_affine(-x, -two, -one);
        }

        SECTION("Dependent subtraction: x - x") {
            // With no_lump, x shares its error symbol with its copy, so
            // x - x = 0 exactly (same as Affine).
            ta_t x(one, two, no_lump);
            ta_t result = x - x;
            REQUIRE(result.radius() == zero);
        }

        SECTION("Scalar right multiply") {
            ta_t x(one, two, no_lump);
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
        ta_t x(one, two, no_lump);
        REQUIRE(x.contains(one));
        REQUIRE(x.contains(two));
        REQUIRE(x.contains(value_t(1.5)));
        REQUIRE_FALSE(x.contains(zero));
        REQUIRE_FALSE(x.contains(three));

        REQUIRE(x.contains(interval_t(one, two)));
        REQUIRE(x.contains(interval_t(one, value_t(1.5))));
        REQUIRE_FALSE(x.contains(interval_t(zero, two)));

        ta_t y(one, value_t(1.5), no_lump);
        REQUIRE(x.contains(y));
        ta_t z(zero, two, no_lump);
        REQUIRE_FALSE(x.contains(z));
    }

    SECTION("Empty form behavior") {
        ta_t empty;
        REQUIRE(empty.empty());
        REQUIRE(empty.range().empty());
        REQUIRE_FALSE(empty.contains(zero));
    }

    SECTION("Stream output") {
        ta_t x(one, two, no_lump);
        std::ostringstream os;
        os << x;
        REQUIRE_FALSE(os.str().empty());
    }

    SECTION("Operations") {
        ta_t x(one, two, no_lump);

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
            ta_t neg(-two, -one, no_lump);
            auto result = sigma::abs(neg);
            test_affine(result, one, two);
        }

        SECTION("fabs") {
            ta_t neg(-two, -one, no_lump);
            auto result = sigma::fabs(neg);
            test_affine(result, one, two);
        }

        SECTION("pow") {
            auto result = sigma::pow(x, value_t(2.0));
            REQUIRE(result.contains(one));
            REQUIRE(result.contains(four));
        }
    }

    SECTION("Lump radius behavior") {
        SECTION("No lumping when threshold is zero") {
            // With no_lump threshold the lump radius stays zero and x - x
            // cancels exactly (all error terms are tracked symbols).
            ta_t x(one, two, no_lump);
            ta_t y    = x;
            ta_t diff = y - x;
            REQUIRE(diff.radius() == zero);
            REQUIRE(diff.lump_radius() == zero);
        }

        SECTION("Lump radius accumulates after lumping") {
            // With big_t, small nonlinearity terms are absorbed into the
            // unsigned lump radius.
            ta_t x(one, two, big_t);
            ta_t bump(one, value_t(1.01), big_t);
            x *= bump;
            REQUIRE(x.lump_radius() > zero);
        }

        SECTION("Lump radii add in subtraction (no cancellation)") {
            // With big_t, both x and y have non-zero lump radii after
            // multiplication. Their difference has lump ≥ x.lump + y.lump
            // because unsigned lumps always add.
            ta_t x(one, two, big_t);
            ta_t bump(one, value_t(1.01), big_t);
            x *= bump;
            ta_t y    = x; // copy, same lump radius value
            ta_t diff = x - y;
            REQUIRE(diff.lump_radius() >= x.lump_radius() + y.lump_radius());
        }

        SECTION("Signed lump can produce interval smaller than true range") {
            // This test proved that the first implementation using signed lump
            // violates the upper-bound guarantee. The root cause:
            //
            //   The copy constructor shares m_lump_term_ between a and its
            //   copy b.  When an independent error is absorbed into the shared
            //   lump of b (because it falls below the relative threshold), and
            //   then a - b is computed, that absorbed error SUBTRACTS from the
            //   shared lump contribution instead of ADDING to the radius.
            //
            // Concrete trace:
            //   a: center=2.5, {sym1: R=1.0, L_a: ds=0.05}
            //      (sym2 with coeff ds was lumped on construction because
            //       ds/(R+ds) = 0.048 < threshold=0.10)
            //   b = a (copy, shares L_a), then scaled by alpha=0.9:
            //      b: {sym1: 0.9, L_a: 0.045}
            //   c = [-dn, dn] = [-0.03, 0.03] independent error; its single
            //      term has relative contribution dn/0.975 ≈ 0.031 < 0.10 →
            //      absorbed into L_a when b += c:
            //      b: {sym1: 0.9, L_a: 0.075}
            //   diff = a - b:
            //      L_a coeff: 0.05 - 0.075 = -0.025 → |contrib| = 0.025
            //      sym1 coeff: 1.0 - 0.9   =  0.100 → |contrib| = 0.100
            //      ThresholdedAffine radius = 0.125
            //
            // Plain Affine (sym2 and ε_c tracked separately):
            //   diff: {sym1: 0.10, sym2: 0.005, ε_c: -0.03} → radius = 0.135
            //
            // The independent error dn=0.03 cancelled (net -0.025) instead of
            // contributing +0.03, giving 0.125 < 0.135.  The true extremal
            // value (sym2 and ε_c extremised in opposite directions) lies
            // outside the ThresholdedAffine interval.

            const value_t R  = value_t(1.0);
            const value_t ds = value_t(0.05); // lumped: 0.05/1.05 ≈ 4.8% < 10%
            const value_t alpha = value_t(0.9);
            const value_t dn =
              value_t(0.03); // lumped into b: 0.03/0.975 ≈ 3.1% < 10%
            const threshold_t T{value_t(0.1)};

            auto sym1 = affine_t::make_error_term();
            auto sym2 = affine_t::make_error_term();

            // ThresholdedAffine path: sym2 is lumped into L_a on construction.
            ta_t a_ta(value_t(2.5),
                      typename ta_t::error_terms_t{{sym1, R}, {sym2, ds}}, T);
            ta_t b_ta = a_ta;      // copy — shares lump symbol L_a
            b_ta *= alpha;         // {sym1: 0.9, L_a: 0.045}
            ta_t c_ta(-dn, dn, T); // single-term interval; not lumped within c
            b_ta += c_ta;          // c's term absorbed into shared L_a → 0.075
            ta_t diff_ta = a_ta - b_ta;

            // Plain Affine path: every term tracked individually, no lumping.
            affine_t a_af(value_t(2.5), {{sym1, R}, {sym2, ds}});
            affine_t b_af = a_af * alpha;
            affine_t c_af(-dn, dn);
            affine_t diff_af = a_af - (b_af + c_af);

            // After the fix: the unsigned lump never cancels, so the
            // ThresholdedAffine radius is at least as large as the plain Affine
            // radius (it may be larger because the lump from a and the lump
            // from b both add, even though the sym2 part is correlated).
            REQUIRE(diff_ta.radius() >= diff_af.radius());

            // The ThresholdedAffine interval contains the correct range,
            // satisfying the upper-bound guarantee.
            REQUIRE(diff_ta.contains(diff_af.range()));
        }
    }
}
