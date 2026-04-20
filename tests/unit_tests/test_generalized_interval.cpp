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
            REQUIRE(value.weights().size() == 0);
            REQUIRE(value.gradient().size() == 0);
        }
        SECTION("From Interval") {
            interval_t interval(1.0, 2.0);
            ginterval_t value(interval);
            test_interval(value.as_interval(), 1.0, 2.0);
            REQUIRE(value.weights().size() == 1);
            REQUIRE(value.gradient().size() == 1);
            for(auto&& [radius, weight] : value.weights()) {
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
            REQUIRE(a.weights().size() == 2);
            REQUIRE(a.gradient().size() == 2);
            for(auto&& [radius, weight] : a.weights()) {
                REQUIRE(a.derivative(radius) == interval_t(1.0));
            }
        }
        SECTION("Dependent") {
            ginterval_t a(interval_t(1.0, 2.0));
            ginterval_t b(a);
            a += b;
            test_interval(a.as_interval(), 2.0, 4.0);
            REQUIRE(a.weights().size() == 1);
            REQUIRE(a.gradient().size() == 1);
            for(auto&& [radius, weight] : a.weights()) {
                REQUIRE(a.derivative(radius) == interval_t(2.0));
            }
        }
    }

    SECTION("operator-=") {
        SECTION("Independent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a = a.weights().begin()->first;
            ginterval_t b(interval_t(1.0, 2.0));
            auto pradius_b = b.weights().begin()->first;
            a -= b;
            test_interval(a.as_interval(), -1.0, 1.0);
            REQUIRE(a.weights().size() == 2);
            REQUIRE(a.gradient().size() == 2);
            REQUIRE(a.derivative(pradius_a) == interval_t(1.0));
            REQUIRE(a.derivative(pradius_b) == interval_t(-1.0));
        }

        SECTION("Dependent") {
            ginterval_t a(interval_t(1.0, 2.0));
            ginterval_t b(a);
            a -= b;
            test_interval(a.as_interval(), 0.0, 0.0);
            REQUIRE(a.weights().size() == 1);
            REQUIRE(a.gradient().size() == 1);
            REQUIRE(a.derivative(a.weights().begin()->first) ==
                    interval_t(0.0));
        }
    }

    SECTION("operator*= (GeneralInterval)") {
        SECTION("Independent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a  = a.weights().begin()->first;
            auto a_interval = a.as_interval();

            ginterval_t b(interval_t(3.0, 4.0));
            auto pradius_b = b.weights().begin()->first;
            a *= b;
            // The paper says 2.5, 8, but I think their [1.5,1.5] term
            // should be [1.0, 2.0] (which then gives the answer
            // [2.25, 8.25])
            test_interval(a.as_interval(), 2.25, 8.25);
            REQUIRE(a.weights().size() == 2);
            REQUIRE(a.gradient().size() == 2);
            REQUIRE(a.derivative(pradius_a) == b.as_interval());
            REQUIRE(a.derivative(pradius_b) == a_interval);
        }
        SECTION("Dependent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a = a.weights().begin()->first;
            auto a_weight  = a.weights().begin()->second;

            ginterval_t b(a);
            auto b_weight = b.weights().begin()->second;
            a *= b;
            test_interval(a.as_interval(), 0.75, 4.0);
            REQUIRE(a.weights().size() == 1);
            REQUIRE(a.gradient().size() == 1);
            REQUIRE(a.derivative(pradius_a) == b.as_interval() * value_t(2.0));
        }
    }

    SECTION("operator/= (GeneralInterval)") {
        SECTION("Independent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a  = a.weights().begin()->first;
            auto a_interval = a.as_interval();

            ginterval_t b(interval_t(3.0, 4.0));
            auto pradius_b  = b.weights().begin()->first;
            auto b_interval = b.as_interval();

            a /= b;
            test_interval(a.as_interval(), 0.190476, 0.666667);
            REQUIRE(a.weights().size() == 2);
            REQUIRE(a.gradient().size() == 2);
            REQUIRE(a.derivative(pradius_a) == interval_t(1.0) / b_interval);
            REQUIRE(a.derivative(pradius_b) ==
                    -a_interval / (b_interval * b_interval));
        }

        SECTION("Dependent") {
            ginterval_t a(interval_t(1.0, 2.0));
            auto pradius_a  = a.weights().begin()->first;
            auto a_interval = a.as_interval();

            auto a_interval_squared = a_interval * a_interval;
            auto term0              = interval_t(1.0) / a_interval;
            auto term1              = a_interval / a_interval_squared;

            ginterval_t b(a);

            a /= b;
            test_interval(a.as_interval(), 1.0, 1.0);
            REQUIRE(a.weights().size() == 1);
            REQUIRE(a.gradient().size() == 1);
            REQUIRE(a.derivative(pradius_a) == term0 - term1);
        }
    }

    SECTION("operator*= (value_t)") {
        ginterval_t a(interval_t(1.0, 2.0));
        a *= 2.0;
        test_interval(a.as_interval(), 2.0, 4.0);
        REQUIRE(a.weights().size() == 1);
        REQUIRE(a.gradient().size() == 1);
        REQUIRE(a.derivative(a.weights().begin()->first) == interval_t(2.0));
    }

    SECTION("square()") {
        ginterval_t a(interval_t(-0.2, 0.3));
        auto pradius_a  = a.weights().begin()->first;
        auto a_interval = a.as_interval();

        auto a_squared = a.square();
        test_interval(a_squared.as_interval(), -0.0225, 0.09);
        REQUIRE(a_squared.weights().size() == 1);
        REQUIRE(a_squared.gradient().size() == 1);
        REQUIRE(a_squared.derivative(pradius_a) == value_t(2.0) * a_interval);
    }

    // SECTION("power()") {
    //     SECTION("power == 0") {
    //         ginterval_t a(interval_t(1.0, 2.0));
    //         auto a_powered = a.power(0);
    //         test_interval(a_powered.as_interval(), 1.0, 1.0);
    //         REQUIRE(a_powered.weights().size() == 0);
    //         REQUIRE(a_powered.gradient().size() == 0);
    //     }
    //     SECTION("power == 1") {
    //         ginterval_t a(interval_t(1.0, 2.0));
    //         auto pradius_a = a.weights().begin()->first;
    //         auto a_powered = a.power(1);
    //         test_interval(a_powered.as_interval(), 1.0, 2.0);
    //         REQUIRE(a_powered.weights().size() == 1);
    //         REQUIRE(a_powered.gradient().size() == 1);
    //         REQUIRE(a_powered.derivative(pradius_a) == interval_t(1.0));
    //     }
    //     SECTION("power == 2") {
    //         ginterval_t a(interval_t(1.0, 2.0));
    //         auto pradius_a = a.weights().begin()->first;
    //         auto a_squared = a.power(2);
    //         test_interval(a_squared.as_interval(), 0.75, 4.0);
    //         REQUIRE(a_squared.weights().size() == 1);
    //         REQUIRE(a_squared.gradient().size() == 1);
    //         REQUIRE(a_squared.derivative(pradius_a) ==
    //                 interval_t(2.0) * a.as_interval());
    //     }
    //     SECTION("power == 3") {
    //         ginterval_t a(interval_t(1.0, 2.0));
    //         auto pradius_a = a.weights().begin()->first;
    //         auto a_powered = a.power(3);
    //         test_interval(a_powered.as_interval(), -0.125, 8.0);
    //         REQUIRE(a_powered.weights().size() == 1);
    //         REQUIRE(a_powered.gradient().size() == 1);
    //         auto corr = interval_t(3.0) * a.square().as_interval();
    //         REQUIRE(a_powered.derivative(pradius_a) == corr);
    //     }
    //     SECTION("power == -1") {
    //         ginterval_t a(interval_t(1.0, 2.0));
    //         auto pradius_a  = a.weights().begin()->first;
    //         auto a_interval = a.as_interval();

    //         auto a_powered = a.power(-1);
    //         test_interval(a_powered.as_interval(), 0.333333, 1.0);
    //         REQUIRE(a_powered.weights().size() == 1);
    //         REQUIRE(a_powered.gradient().size() == 1);
    //         auto corr = interval_t(-1.0) / (a_interval * a_interval);
    //         REQUIRE(a_powered.derivative(pradius_a) == corr);
    //     }
    //     SECTION("power == -2") {
    //         ginterval_t a(interval_t(1.0, 2.0));
    //         auto pradius_a  = a.weights().begin()->first;
    //         auto a_interval = a.as_interval();

    //         auto a_powered = a.power(-2);
    //         test_interval(a_powered.as_interval(), -0.488889, 1.333333);
    //         REQUIRE(a_powered.weights().size() == 1);
    //         REQUIRE(a_powered.gradient().size() == 1);
    //         auto corr =
    //           interval_t(-2.0) / (a_interval * a_interval * a_interval);
    //         REQUIRE(a_powered.derivative(pradius_a) == corr);
    //     }
    // }

    SECTION("(x + y) / (x - y)") {
        // Taken from Example 6.2 in the paper
        ginterval_t x(interval_t(5.0, 10.0));
        auto pradius_x = x.weights().begin()->first;
        ginterval_t y(interval_t(1.0, 2.0));
        auto pradius_y = y.weights().begin()->first;

        auto result = (x + y) / (x - y);
        test_interval(result.as_interval(), 0.66666667, 2.33333333);
        REQUIRE(result.weights().size() == 2);

        REQUIRE(result.gradient().size() == 2);
        // I'm not getting the same value as the paper here...
        test_interval(result.derivative(pradius_x), -1.8708292246,
                      0.2078699172);
        test_interval(result.derivative(pradius_y), 0.18518519, 1.66666667);
    }

    //     SECTION("Example 8.1 from paper") {
    //         SECTION("Independent") {
    //             // I'm not sure how many of the x_1 intervals are treated as
    //             // independent in the paper, but we'll assume one per power
    //             ginterval_t x_1_0(interval_t(1.0, 2.0));
    //             ginterval_t x_1_1(interval_t(1.0, 2.0));
    //             ginterval_t x_1_2(interval_t(1.0, 2.0));
    //             ginterval_t x_2(interval_t(3.0, 4.0));

    //             auto x5 = x_1_0.power(5);
    //             auto x3 = x_1_1.power(3);

    //             auto result = x5 - x3 * 25.2 + 24 * x_1_2 - 6 * x_2;
    //             // The paper gives -200.600000, 36.800000, but our result has
    //             a
    //             // slightly different answer. I'm assuming it's because we do
    //             things
    //             // in a different order, but have not confirmed this.
    //             test_interval(result.as_interval(), -201.725006, 41.150000);
    //         }

    //         SECTION("Dependent") {
    //             ginterval_t x_1(interval_t(1.0, 2.0));
    //             ginterval_t x_2(interval_t(3.0, 4.0));
    //             auto x5     = x_1.power(5);
    //             auto x4     = x_1.power(4);
    //             auto x3     = x_1.power(3);
    //             auto x2     = x_1.power(2);
    //             auto result = x5 - x3 * 25.2 + 24 * x_1 - 6 * x_2;
    //             test_interval(result.as_interval(), -157.350000, 12.993750);

    //             auto deriv_1 = 5 * x4 - 3 * 25.2 * x2 + 24;
    //             interval_t deriv_2(-6);

    //             REQUIRE(result.gradient().size() == 2);
    //             REQUIRE(result.derivative(x_1.weights().begin()->first) ==
    //                     deriv_1.as_interval());
    //             REQUIRE(result.derivative(x_2.weights().begin()->first) ==
    //             deriv_2);
    //         }
    //     }
}
