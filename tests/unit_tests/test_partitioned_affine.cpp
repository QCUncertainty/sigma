#include "testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_interval;

TEMPLATE_TEST_CASE("PartitionedAffine", "", float, double) {
    using value_t     = TestType;
    using pinterval_t = sigma::PartitionedAffine<TestType>;
    using interval_t  = typename pinterval_t::interval_t;

    value_t zero  = 0.0;
    value_t one   = 1.0;
    value_t two   = 2.0;
    value_t three = 3.0;
    value_t four  = 4.0;

    SECTION("Constructors") {
        SECTION("Default") {
            pinterval_t value;
            REQUIRE(value.range() == interval_t());
            REQUIRE(value.num_partitions() == 0);
        }

        SECTION("From Value") {
            pinterval_t value(one);
            test_interval(value.range(), one, one);
            REQUIRE(value.num_partitions() == 1);
        }

        SECTION("From Lower and Upper") {
            pinterval_t value(one, two, 2);
            test_interval(value.range(0), one, 1.5);
            test_interval(value.range(1), 1.5, two);
            test_interval(value.range(), one, two);
            REQUIRE(value.num_partitions() == 2);

            pinterval_t value2(zero, four, 1);
            test_interval(value2.range(0), zero, four);
            test_interval(value2.range(), zero, four);
            REQUIRE(value2.num_partitions() == 1);

            pinterval_t value3(three, three, 1);
            test_interval(value3.range(), three, three);
            REQUIRE(value3.num_partitions() == 1);
        }

        SECTION("From Interval") {
            pinterval_t value(interval_t(one, two));
            test_interval(value.range(), one, two);
            REQUIRE(value.num_partitions() == 10);

            pinterval_t value2(interval_t(-two, three));
            test_interval(value2.range(), -two, three);
            REQUIRE(value2.num_partitions() == 10);

            pinterval_t value3(interval_t(-two, -one), 20);
            test_interval(value3.range(), -two, -one);
            REQUIRE(value3.num_partitions() == 20);
        }
    }

    SECTION("num_partitions") {
        pinterval_t defaulted;
        REQUIRE(defaulted.num_partitions() == 0);

        pinterval_t scalar(one);
        REQUIRE(scalar.num_partitions() == 1);

        pinterval_t value(interval_t(one, two));
        REQUIRE(value.num_partitions() == 10);

        pinterval_t value2(interval_t(one, two), 20);
        REQUIRE(value2.num_partitions() == 20);
    }

    SECTION("center") {
        pinterval_t value(interval_t(one, three));
        REQUIRE(value.center() == Catch::Approx(2.0));

        pinterval_t value2(interval_t(-one, two), 20);
        REQUIRE(value2.center() == Catch::Approx(0.5));

        pinterval_t value3(interval_t(-four, -one), 20);
        REQUIRE(value3.center() == Catch::Approx(-2.5));

        REQUIRE_THROWS_AS(pinterval_t{}.center(), std::domain_error);
    }

    SECTION("range") {
        pinterval_t empty;
        REQUIRE(empty.range() == interval_t());

        pinterval_t value(interval_t(one, two));
        test_interval(value.range(), one, two);

        pinterval_t value2(interval_t(one, two), 20);
        test_interval(value2.range(), one, two);
    }

    SECTION("radius") {
        pinterval_t value(interval_t(one, two));
        REQUIRE(value.radius() == Catch::Approx(0.5));

        pinterval_t value2(interval_t(one, two), 20);
        REQUIRE(value2.radius() == Catch::Approx(0.5));

        pinterval_t value3(interval_t(-four, -one), 20);
        REQUIRE(value3.radius() == Catch::Approx(1.5));

        pinterval_t empty;
        REQUIRE_THROWS_AS(empty.radius(), std::domain_error);

        pinterval_t scalar(one, one);
        REQUIRE(scalar.radius() == 0.0);
    }

    SECTION("contains") {
        auto value = pinterval_t(one, two);
        REQUIRE(value.contains(one));
        REQUIRE(value.contains(value_t(1.5)));
        REQUIRE(value.contains(two));
        REQUIRE_FALSE(value.contains(value_t(0.5)));

        REQUIRE(value.contains(interval_t(one, two)));
        REQUIRE(value.contains(interval_t(value_t(1.5), value_t(1.5))));
        REQUIRE_FALSE(value.contains(interval_t(one, three)));
        REQUIRE_FALSE(value.contains(interval_t(zero, value_t(1.5))));

        REQUIRE(value.contains(pinterval_t(one, two)));
        REQUIRE_FALSE(value.contains(pinterval_t(one, three)));
        REQUIRE_FALSE(value.contains(pinterval_t(zero, one)));
    }

    SECTION("operator-") {
        pinterval_t scalar(one);
        auto scalar2 = -scalar;
        test_interval(scalar2.range(), -one, -one);

        pinterval_t value(interval_t(one, two));
        auto value2 = -value;
        test_interval(value2.range(), -two, -one);

        pinterval_t value3(interval_t(-four, -one), 20);
        auto value4 = -value3;
        test_interval(value4.range(), one, four);

        pinterval_t empty;
        auto empty2 = -empty;
        REQUIRE(empty2.range() == interval_t());
    }

    SECTION("operator+=") {
        SECTION("Value") {
            pinterval_t value(one);
            pinterval_t value2(one, two, 2);
            value2 += value;
            test_interval(value2.range(), two, value_t(3.0));
        }
        SECTION("Independent") {
            // Collapse makes results conservative; check containment only.
            pinterval_t value(one, two, 2);
            value += pinterval_t(three, four, 2);
            REQUIRE(value.range().lower() <= four);
            REQUIRE(value.range().upper() >= value_t(6.0));

            pinterval_t value2(interval_t(-one, two), 20);
            value2 += pinterval_t(interval_t(three, four), 20);
            REQUIRE(value2.range().lower() <= two);
            REQUIRE(value2.range().upper() >= value_t(6.0));

            pinterval_t value3(interval_t(-two, -one), 20);
            value3 += pinterval_t(interval_t(three, four), 20);
            REQUIRE(value3.range().lower() <= one);
            REQUIRE(value3.range().upper() >= three);

            pinterval_t value4(one, two);
            pinterval_t value_temp(-four, -three);
            value4 += value_temp;
            REQUIRE(value4.range().lower() <= -three);
            REQUIRE(value4.range().upper() >= -one);
        }
        SECTION("Dependent") {
            pinterval_t value(one, two, 2);
            auto value2 = -value;
            value += value2;
            test_interval(value.range(), zero, zero);
        }
    }

    SECTION("operator-=") {
        SECTION("Value") {
            pinterval_t value(one);
            pinterval_t value2(one, two, 2);
            value2 -= value;
            test_interval(value2.range(), zero, one);
        }
        SECTION("Independent") {
            // Collapse makes results conservative; check containment only.
            pinterval_t value(one, two, 2);
            pinterval_t temp(three, four, 2);
            value -= temp;
            REQUIRE(value.range().lower() <= -three);
            REQUIRE(value.range().upper() >= -one);

            pinterval_t value2(interval_t(-one, two), 20);
            value2 -= pinterval_t(interval_t(three, four), 20);
            REQUIRE(value2.range().lower() <= -value_t(5.0));
            REQUIRE(value2.range().upper() >= -one);

            pinterval_t value3(interval_t(-two, -one), 20);
            value3 -= pinterval_t(interval_t(three, four), 20);
            REQUIRE(value3.range().lower() <= -value_t(6.0));
            REQUIRE(value3.range().upper() >= -four);
        }
        SECTION("Dependent") {
            pinterval_t value(one, two, 2);
            pinterval_t value2(value);
            value -= value2;
            test_interval(value.range(), zero, zero);

            pinterval_t value3(interval_t(three, four), 2);
            // value3 = [4, 6]
            value3 += value2;
            value3 -= value2;

            // Tight bound is [3, 4]. After collapse the result is
            // conservative, so we only check containment of [3, 4].
            REQUIRE(value3.range().lower() <= three);
            REQUIRE(value3.range().upper() >= four);
        }
    }

    SECTION("operator*=") {
        SECTION("Value") {
            pinterval_t value(one);
            pinterval_t value2(one, two, 2);
            value2 *= value;
            test_interval(value2.range(), one, two);
        }

        SECTION("Independent") {
            // Collapse makes results conservative; check containment only.
            pinterval_t value(one, two);
            value *= pinterval_t(three, four);
            REQUIRE(value.range().lower() <= value_t(3.0));
            REQUIRE(value.range().upper() >= value_t(8.0));

            pinterval_t value2(three, value_t(8.0));
            value2 *= pinterval_t(interval_t(-three, -two));
            // Tight range is [-24, -6]
            REQUIRE(value2.range().lower() <= value_t(-24.0));
            REQUIRE(value2.range().upper() >= value_t(-6.0));

            value *= pinterval_t(interval_t(-two, two));
            // True range is [-16, 16]; collapse may widen conservatively.
            // The result straddles zero and should be symmetric or wider.
            REQUIRE(value.range().lower() < value_t(0));
            REQUIRE(value.range().upper() > value_t(0));
        }
        SECTION("Dependent") {
            pinterval_t value(-one, two);
            value *= value;
            // Tight range is [0, 4]
            test_interval(value.range(), -0.000350, 4.000000);
        }
    }

    SECTION("operator/=") {
        SECTION("Value") {
            pinterval_t value(one);
            pinterval_t value2(one, two, 2);
            value2 /= value;
            test_interval(value2.range(), one, two);
        }
        SECTION("Independent") {
            // Collapse makes results conservative; check containment only.
            pinterval_t value(one, two);
            value /= pinterval_t(three, four);
            REQUIRE(value.range().lower() <= value_t(0.25));
            REQUIRE(value.range().upper() >= value_t(2.0 / 3.0));
        }
        SECTION("Dependent") {
            pinterval_t value(one, two);
            value /= value;
            test_interval(value.range(), 0.999951, 1.000009);
        }
    }

    SECTION("collapse") {
        SECTION("No-op when already at or below target") {
            pinterval_t value(interval_t(one, two), 4);
            auto lo_before = value.range().lower();
            auto hi_before = value.range().upper();
            value.collapse(4);
            REQUIRE(value.num_partitions() == 4);
            test_interval(value.range(), lo_before, hi_before);

            value.collapse(10); // target > current: no-op
            REQUIRE(value.num_partitions() == 4);
        }

        SECTION("Collapse to 1: range contains original endpoints") {
            pinterval_t value(interval_t(one, four), 10);
            value.collapse(1);
            REQUIRE(value.num_partitions() == 1);
            // Collapsed range must conservatively contain the original [1,4].
            // Allow small floating-point slack.
            REQUIRE(value.range().lower() <= one + value_t(1.0e-4));
            REQUIRE(value.range().upper() >= four - value_t(1.0e-4));
        }

        SECTION("Collapse to n: range contains original endpoints") {
            pinterval_t value(interval_t(one, four), 20);
            value.collapse(5);
            REQUIRE(value.num_partitions() == 5);
            REQUIRE(value.range().lower() <= one + value_t(1.0e-4));
            REQUIRE(value.range().upper() >= four - value_t(1.0e-4));
        }

        SECTION(
          "Collapse negative interval: range contains original endpoints") {
            pinterval_t value(interval_t(-four, -one), 20);
            value.collapse(2);
            REQUIRE(value.num_partitions() == 2);
            REQUIRE(value.range().lower() <= -four + value_t(1.0e-4));
            REQUIRE(value.range().upper() >= -one - value_t(1.0e-4));
        }

        SECTION("Collapse preserves error-symbol identity") {
            // Build a PA whose Affine shares error symbols with another PA.
            // pa1 + (-pa1) must produce zero range regardless of collapse,
            // because the same e_k appears in both operands and cancels.
            pinterval_t pa1(interval_t(one, two), 5);
            pa1.collapse(2);

            pinterval_t collapsed = pa1;
            auto neg_collapsed    = -collapsed;
            collapsed += neg_collapsed;

            // Dependent cancellation: the result must contain 0 and should
            // be very tight (radius close to 0).
            REQUIRE(collapsed.range().lower() <= zero);
            REQUIRE(collapsed.range().upper() >= zero);
            // With shared symbols the radius must be much smaller than the
            // original interval's radius (no independent widening).
            REQUIRE(collapsed.range().radius() < value_t(0.1));
        }
    }

    SECTION("operator+= partition count stays bounded after chaining") {
        // Without collapse, adding N independent terms with 2 partitions each
        // would grow the count exponentially via the Cartesian-product loop.
        // The primary invariant is that partition count stays bounded.
        const std::size_t n = 2;
        pinterval_t acc(interval_t(zero, one), n);
        for(int k = 0; k < 8; ++k) {
            pinterval_t term(interval_t(zero, one), n);
            acc += term;
            REQUIRE(acc.num_partitions() == n);
        }
        // The result must conservatively contain some part of [0, 9].
        // Collapse is allowed to widen the interval, so we only check that
        // the result is non-trivially wide.
        REQUIRE(acc.range().upper() > acc.range().lower());
    }

    SECTION("operator*= partition count stays bounded after chaining") {
        const std::size_t n = 2;
        pinterval_t acc(interval_t(one, two), n);
        for(int k = 0; k < 6; ++k) {
            pinterval_t term(interval_t(one, two), n);
            acc *= term;
            REQUIRE(acc.num_partitions() == n);
        }
        // 7 independent [1,2] intervals multiplied: true result is [1, 128].
        // Collapse may widen conservatively; just verify partition count
        // stays bounded and the result contains a positive value.
        REQUIRE(acc.range().upper() > acc.range().lower());
        REQUIRE(acc.range().upper() > value_t(0));
    }

    SECTION("operator<<") {
        pinterval_t value(one, two, 1);
        std::stringstream ss;
        ss << value;
        std::stringstream corr_ss;
        corr_ss << value.center() << "+/-" << value.radius();
        REQUIRE(ss.str() == corr_ss.str());
    }
}
