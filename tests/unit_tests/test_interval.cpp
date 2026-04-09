#include "testing.hpp"
#include <sigma/sigma.hpp>
#include <sstream>

using testing::test_interval;

TEMPLATE_TEST_CASE("Interval", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;
    using other_t   = typename testing::test_traits<TestType>::other_t;

    SECTION("Constructors") {
        SECTION("With Lower and Upper") {
            auto value = testing_t(1.0, 2.0);
            test_interval(value, 1.0, 2.0);
        }
        SECTION("Copy") {
            auto first = testing_t(1.0, 2.0);
            testing_t value(first);
            test_interval(value, 1.0, 2.0);
            test_interval(first, 1.0, 2.0);
        }
        SECTION("Move") {
            auto first = testing_t(1.0, 2.0);
            testing_t value(std::move(first));
            test_interval(value, 1.0, 2.0);
        }
        SECTION("Copy Assignment") {
            auto first = testing_t(1.0, 2.0);
            auto value = first;
            test_interval(value, 1.0, 2.0);
            test_interval(first, 1.0, 2.0);
        }
        SECTION("Move Assignment") {
            auto first = testing_t(1.0, 2.0);
            auto value = std::move(first);
            test_interval(value, 1.0, 2.0);
        }
    }
    SECTION("operator<<(std::ostream, Interval)") {
        value_t lo = 1.0, hi = 2.0;
        auto value = testing_t(lo, hi);
        std::stringstream ss, corr;
        ss << value;
        corr << value.median() << "+/-" << value.radius();
        REQUIRE(ss.str() == corr.str());
    }
    SECTION("Comparisons") {
        auto first = testing_t(1.0, 2.0);
        SECTION("Same") {
            auto second = first;
            REQUIRE(first == second);
            REQUIRE_FALSE(first != second);
        }
        SECTION("Different Value Type") {
            auto second = other_t(1.0, 2.0);
            REQUIRE_FALSE(first == second);
        }
        SECTION("Different Lower") {
            auto second = testing_t(0.5, 2.0);
            REQUIRE_FALSE(first == second);
            REQUIRE(first != second);
        }
        SECTION("Different Upper") {
            auto second = testing_t(1.0, 3.0);
            REQUIRE_FALSE(first == second);
            REQUIRE(first != second);
        }
        SECTION("Relative") {
            // [1,2] is certainly less than [3,4]
            auto second = testing_t(3.0, 4.0);
            // [1.5, 2.5] overlaps with [1,2]
            auto third = testing_t(1.5, 2.5);
            SECTION("Less than") {
                REQUIRE(first < second);
                REQUIRE(third < second);
                REQUIRE_FALSE(second < first);
                REQUIRE_FALSE(first < third);
            }
            SECTION("Greater than") {
                REQUIRE(second > first);
                REQUIRE(second > third);
                REQUIRE_FALSE(first > second);
                REQUIRE_FALSE(third > first);
            }
            SECTION("Less than or equal") {
                REQUIRE(first <= second);
                REQUIRE(third <= second);
                REQUIRE_FALSE(second <= first);
                REQUIRE_FALSE(first <= third);
            }
            SECTION("Greater than or equal") {
                REQUIRE(second >= first);
                REQUIRE(second >= third);
                REQUIRE_FALSE(first >= second);
                REQUIRE_FALSE(third >= first);
            }
        }
    }
    SECTION("contains") {
        auto interval = testing_t(value_t{1}, value_t{3});

        SECTION("interior") { REQUIRE(interval.contains(value_t{2})); }

        SECTION("endpoints are included") {
            REQUIRE(interval.contains(value_t{1}));
            REQUIRE(interval.contains(value_t{3}));
        }

        SECTION("outside") {
            REQUIRE_FALSE(interval.contains(value_t{0.99}));
            REQUIRE_FALSE(interval.contains(value_t{3.01}));
        }

        SECTION("degenerate interval (single point)") {
            auto point = testing_t(value_t{5}, value_t{5});
            REQUIRE(point.contains(value_t{5}));
            REQUIRE_FALSE(point.contains(value_t{4.99}));
            REQUIRE_FALSE(point.contains(value_t{5.01}));
        }

        SECTION("default constructed [0, 0]") {
            testing_t zero{};
            REQUIRE(zero.contains(value_t{0}));
            REQUIRE_FALSE(zero.contains(value_t{-0.01}));
            REQUIRE_FALSE(zero.contains(value_t{0.01}));
        }
    }
    SECTION("Arithmetic") {
        // a = [1, 3], b = [2, 4]
        auto a = testing_t(value_t{1}, value_t{3});
        auto b = testing_t(value_t{2}, value_t{4});

        SECTION("Negation") { test_interval(-a, -3.0, -1.0); }
        SECTION("Addition") {
            SECTION("With Interval") { test_interval(a + b, 3.0, 7.0); }
            SECTION("With Scalar") {
                test_interval(a + value_t{1}, 2.0, 4.0);
                test_interval(value_t{1} + a, 2.0, 4.0);
            }
        }
        SECTION("Addition Assignment") {
            SECTION("With Interval") {
                auto x = a;
                x += b;
                test_interval(x, 3.0, 7.0);
            }
            SECTION("With Scalar") {
                auto x = a;
                x += value_t{1};
                test_interval(x, 2.0, 4.0);
            }
        }
        SECTION("Subtraction") {
            SECTION("With Interval") { test_interval(a - b, -3.0, 1.0); }
            SECTION("With Scalar") {
                test_interval(a - value_t{1}, 0.0, 2.0);
                test_interval(value_t{1} - a, -2.0, 0.0);
            }
        }
        SECTION("Subtraction Assignment") {
            SECTION("With Interval") {
                auto x = a;
                x -= b;
                test_interval(x, -3.0, 1.0);
            }
            SECTION("With Scalar") {
                auto x = a;
                x -= value_t{1};
                test_interval(x, 0.0, 2.0);
            }
        }
        SECTION("Multiplication") {
            SECTION("By Interval") { test_interval(a * b, 2.0, 12.0); }
            SECTION("By Scalar") {
                test_interval(a * value_t{2}, 2.0, 6.0);
                test_interval(value_t{2} * a, 2.0, 6.0);
            }
        }
        SECTION("Multiplication Assignment") {
            SECTION("By Interval") {
                auto x = a;
                x *= b;
                test_interval(x, 2.0, 12.0);
            }
            SECTION("By Scalar") {
                auto x = a;
                x *= value_t{2};
                test_interval(x, 2.0, 6.0);
            }
        }
        SECTION("Division") {
            SECTION("By Interval") { test_interval(a / b, 0.25, 1.5); }
            SECTION("By Scalar") {
                test_interval(a / value_t{2}, 0.5, 1.5);
                test_interval(value_t{2} / a, 0.6667, 2.0);
            }
        }
        SECTION("Division Assignment") {
            SECTION("By Interval") {
                auto x = a;
                x /= b;
                test_interval(x, 0.25, 1.5);
            }
            SECTION("By Scalar") {
                auto x = a;
                x /= value_t{2};
                test_interval(x, 0.5, 1.5);
            }
        }
    }
}
