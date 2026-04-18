#include "testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_interval;

TEMPLATE_TEST_CASE("Generalized Interval", "", float, double) {
    using value_t     = TestType;
    using interval_t  = sigma::Interval<value_t>;
    using ginterval_t = sigma::GeneralInterval<interval_t>;

    SECTION("Constructors") {
        SECTION("Default") {
            ginterval_t value;
            test_interval(value.as_interval(), 0.0, 0.0);
            REQUIRE(value.dep().size() == 0);
            REQUIRE(value.gradient().size() == 0);
        }
        SECTION("From Interval") {
            interval_t interval(1.0, 2.0);
            ginterval_t value(interval);
            test_interval(value.as_interval(), 1.0, 2.0);
            REQUIRE(value.dep().size() == 1);
            REQUIRE(value.gradient().size() == 1);
            for(auto&& [radius, weight] : value.dep()) {
                REQUIRE(value.derivative(radius) == interval_t(1.0));
            }
        }
    }

    SECTION("operator+=") {
        SECTION("Independent") {
            ginterval_t a(interval_t(1.0, 2.0));
            ginterval_t b(interval_t(3.0, 4.0));
            a += b;
            test_interval(a.as_interval(), 4.0, 6.0);
            REQUIRE(a.dep().size() == 2);
            REQUIRE(a.gradient().size() == 2);
            for(auto&& [radius, weight] : a.dep()) {
                REQUIRE(a.derivative(radius) == interval_t(1.0));
            }
        }
        SECTION("Dependent") {
            ginterval_t a(interval_t(1.0, 2.0));
            ginterval_t b(a);
            a += b;
            test_interval(a.as_interval(), 2.0, 4.0);
            REQUIRE(a.dep().size() == 1);
            REQUIRE(a.gradient().size() == 1);
            for(auto&& [radius, weight] : a.dep()) {
                REQUIRE(a.derivative(radius) == interval_t(2.0));
            }
        }
    }

    SECTION("operator-=") {
        SECTION("Independent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a = a.dep().begin()->first;
            ginterval_t b(interval_t(1.0, 2.0));
            auto pradius_b = b.dep().begin()->first;
            a -= b;
            test_interval(a.as_interval(), -1.0, 1.0);
            REQUIRE(a.dep().size() == 2);
            REQUIRE(a.gradient().size() == 2);
            REQUIRE(a.derivative(pradius_a) == interval_t(1.0));
            REQUIRE(a.derivative(pradius_b) == interval_t(-1.0));
        }

        SECTION("Dependent") {
            ginterval_t a(interval_t(1.0, 2.0));
            ginterval_t b(a);
            a -= b;
            test_interval(a.as_interval(), 0.0, 0.0);
            REQUIRE(a.dep().size() == 1);
            REQUIRE(a.gradient().size() == 1);
            REQUIRE(a.derivative(a.dep().begin()->first) == interval_t(0.0));
        }
    }

    SECTION("operator*= (GeneralInterval)") {
        SECTION("Independent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a  = a.dep().begin()->first;
            auto a_interval = a.as_interval();

            ginterval_t b(interval_t(3.0, 4.0));
            auto pradius_b = b.dep().begin()->first;
            a *= b;
            // The paper says 2.5, 8, but I think their [1.5,1.5] term
            // should be [1.0, 2.0] (which then gives the answer
            // [2.25, 8.25])
            test_interval(a.as_interval(), 2.25, 8.25);
            REQUIRE(a.dep().size() == 2);
            REQUIRE(a.gradient().size() == 2);
            REQUIRE(a.derivative(pradius_a) == b.as_interval());
            REQUIRE(a.derivative(pradius_b) == a_interval);
        }
        SECTION("Dependent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a = a.dep().begin()->first;
            auto a_weight  = a.dep().begin()->second;

            ginterval_t b(a);
            auto b_weight = b.dep().begin()->second;
            a *= b;
            test_interval(a.as_interval(), 0.75, 4.0);
            REQUIRE(a.dep().size() == 1);
            REQUIRE(a.gradient().size() == 1);
            REQUIRE(a.derivative(pradius_a) == b.as_interval() * value_t(2.0));
        }
    }

    SECTION("operator/= (GeneralInterval)") {
        SECTION("Independent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a  = a.dep().begin()->first;
            auto a_interval = a.as_interval();

            ginterval_t b(interval_t(3.0, 4.0));
            auto pradius_b  = b.dep().begin()->first;
            auto b_interval = b.as_interval();

            a /= b;
            test_interval(a.as_interval(), 0.190476, 0.666667);
            REQUIRE(a.dep().size() == 2);
            REQUIRE(a.gradient().size() == 2);
            REQUIRE(a.derivative(pradius_a) == interval_t(1.0) / b_interval);
            REQUIRE(a.derivative(pradius_b) ==
                    -a_interval / (b_interval * b_interval));
        }

        SECTION("Dependent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a  = a.dep().begin()->first;
            auto a_interval = a.as_interval();

            auto a_interval_squared = a_interval * a_interval;
            auto term0              = interval_t(1.0) / a_interval;
            auto term1              = a_interval / a_interval_squared;

            ginterval_t b(a);

            a /= b;
            test_interval(a.as_interval(), 1.0, 1.0);
            REQUIRE(a.dep().size() == 1);
            REQUIRE(a.gradient().size() == 1);
            REQUIRE(a.derivative(pradius_a) == term0 - term1);
        }
    }

    SECTION("operator*= (value_t)") {
        ginterval_t a(interval_t(1.0, 2.0));
        a *= 2.0;
        test_interval(a.as_interval(), 2.0, 4.0);
        REQUIRE(a.dep().size() == 1);
        REQUIRE(a.gradient().size() == 1);
        REQUIRE(a.derivative(a.dep().begin()->first) == interval_t(2.0));
    }

    SECTION("Example 8.1 from paper") {
        SECTION("Independent") {
            // I'm not sure how many of the x_1 intervals are treated as
            // independent in the paper, but we'll assume one per power
            ginterval_t x_1_0(interval_t(1.0, 2.0));
            ginterval_t x_1_1(interval_t(1.0, 2.0));
            ginterval_t x_1_2(interval_t(1.0, 2.0));
            ginterval_t x_2(interval_t(3.0, 4.0));

            auto x5 = x_1_0 * x_1_0 * x_1_0 * x_1_0 * x_1_0;
            auto x3 = x_1_1 * x_1_1 * x_1_1;

            auto result = x5 - x3 * 25.2 + 24 * x_1_2 - 6 * x_2;
            // The paper gives -200.600000, 36.800000, but our result has a
            // slightly larger range (they say something about only keeping
            // terms linear in the radius, but we keep the quadratic terms too)
            test_interval(result.as_interval(), -209.506250, 65.150000);
        }

        SECTION("Dependent") {
            ginterval_t x_1(interval_t(1.0, 2.0));
            ginterval_t x_2(interval_t(3.0, 4.0));
            auto x5     = x_1 * x_1 * x_1 * x_1 * x_1;
            auto x4     = x_1 * x_1 * x_1 * x_1;
            auto x3     = x_1 * x_1 * x_1;
            auto x2     = x_1 * x_1;
            auto result = x5 - x3 * 25.2 + 24 * x_1 - 6 * x_2;
            test_interval(result.as_interval(), -157.350000, 12.993750);

            auto deriv_1 = 5 * x4 - 3 * 25.2 * x2 + 24;
            interval_t deriv_2(-6);

            REQUIRE(result.gradient().size() == 2);
            REQUIRE(result.derivative(x_1.dep().begin()->first) ==
                    deriv_1.as_interval());
            REQUIRE(result.derivative(x_2.dep().begin()->first) == deriv_2);
        }
    }
}
