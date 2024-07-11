#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Arithmetic", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1);
    auto b = testing_t(2.0, 0.2);
    auto c = testing_t(3.0, 0.3);

    SECTION("Negation") {
        test_uncertain(-a, -1.0, 0.1, 1);
        test_uncertain(-(a + b), -3.0, 0.2236, 2);
        test_uncertain(-a + a, 0.0, 0.0, 1);
    }
    SECTION("Addition") { test_uncertain(a + b, 3.0, 0.2236, 2); }
    SECTION("Addition Assignment") {
        auto x = a;
        x += b;
        test_uncertain(x, 3.0, 0.2236, 2);
    }
    SECTION("Subtraction") {
        test_uncertain(a - b, -1.0, 0.2236, 2);
        test_uncertain(a - a, 0.0, 0.0, 1);
    }
    SECTION("Subtraction Assignment") {
        auto x = a;
        x -= b;
        test_uncertain(x, -1.0, 0.2236, 2);
    }
    SECTION("Multiplication") {
        SECTION("By Uncertain") {
            auto x = a * b;
            test_uncertain(a * b, 2.0, 0.2828, 2);
        }
        SECTION("By Certain") {
            int two = 2;
            auto x  = a * two;        // Works with int
            auto y = (double)two * a; // Works with floating point, and reversed
            test_uncertain(x, 2.0, 0.2, 1);
            test_uncertain(y, 2.0, 0.2, 1);
            REQUIRE(x == y);
            REQUIRE(x == (a + a));
        }
    }
    SECTION("Multiplication Assignment") {
        SECTION("By Uncertain") {
            auto x = a;
            x *= b;
            test_uncertain(x, 2.0, 0.2828, 2);
        }
        SECTION("By Certain") {
            int two = 2;
            auto x  = a;
            auto y  = a;
            x *= two;
            y *= (double)two;
            test_uncertain(x, 2.0, 0.2, 1);
            test_uncertain(y, 2.0, 0.2, 1);
            REQUIRE(x == y);
            REQUIRE(x == (a + a));
        }
    }
    SECTION("Division") {
        SECTION("By Uncertain") {
            test_uncertain(a / b, 0.5, 0.0707, 2);
            test_uncertain(a / a, 1.0, 0.0, 1);
        }
        SECTION("By Certain") {
            int two = 2;
            auto x  = a / two;        // Works with int
            auto y = (double)two / a; // Works with floating point, and reversed
            test_uncertain(x, 0.5, 0.05, 1);
            test_uncertain(y, 2.0, 0.2, 1);
        }
    }
    SECTION("Division Assignment") {
        SECTION("By Uncertain") {
            auto x = a;
            auto y = a;
            x /= b;
            y /= a;
            test_uncertain(x, 0.5, 0.0707, 2);
            test_uncertain(y, 1.0, 0.0, 1);
        }
        SECTION("By Certain") {
            int two = 2;
            auto x  = a / two;        // Works with int
            auto y = (double)two / a; // Works with floating point, and reversed
            test_uncertain(x, 0.5, 0.05, 1);
            test_uncertain(y, 2.0, 0.2, 1);
        }
    }
}