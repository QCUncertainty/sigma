#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_interval;
using testing::test_uncertain;

TEMPLATE_TEST_CASE("Exponents (Interval)", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    SECTION("Square Root") {
        // sqrt([1, 4]) = [1, 2]
        auto a = testing_t(value_t{1}, value_t{4});
        test_interval(sigma::sqrt(a), 1.0, 2.0);
        // sqrt([4, 9]) = [2, 3]
        auto b = testing_t(value_t{4}, value_t{9});
        test_interval(sigma::sqrt(b), 2.0, 3.0);
    }
    SECTION("Exponential Function") {
        // exp([0, 1]) = [1, e]
        auto a = testing_t(value_t{0}, value_t{1});
        test_interval(sigma::exp(a), 1.0, 2.7183);
        // exp([1, 2]) = [e, e^2]
        auto b = testing_t(value_t{1}, value_t{2});
        test_interval(sigma::exp(b), 2.7183, 7.3891);
    }
    SECTION("Natural Logarithm") {
        // log([1, e]) = [0, 1]
        auto a = testing_t(value_t{1}, value_t{2.7183});
        test_interval(sigma::log(a), 0.0, 1.0);
        // log([1, 1]) = [0, 0]
        auto b = testing_t(value_t{1}, value_t{1});
        test_interval(sigma::log(b), 0.0, 0.0);
    }
}

TEMPLATE_TEST_CASE("Exponents", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1);
    auto b = testing_t(2.0, 0.2);
    auto c = testing_t(4.0, 0.4);
    auto d = testing_t(5.0, 0.5);
    auto e = testing_t(8.0, 0.8);

    SECTION("Exponentiation") {
        SECTION("Certain exponent") {
            test_uncertain(sigma::pow(a, 2), 1.0, 0.2, 1);
            test_uncertain(sigma::pow((a + b), -1), 0.3333, 0.0248, 2);
            test_uncertain(sigma::pow((a + b + a * b), 0.5), 2.2361, 0.1118, 2);
            test_uncertain(sigma::pow((a + b + a * b), 0.0), 1.0, 0.0, 0);
        }
        SECTION("Uncertain exponent") {
            test_uncertain(sigma::pow(a, b), 1.0, 0.2, 1);
            test_uncertain(sigma::pow(c, -b), 0.0625, 0.0214, 2);
            test_uncertain(sigma::pow(c, a * 0.5), 2.0, 0.1709, 2);
        }
    }
    SECTION("Square Root") {
        test_uncertain(sigma::sqrt(a), 1.0, 0.05, 1);
        test_uncertain(sigma::sqrt(c), 2.0, 0.1, 1);
        test_uncertain(sigma::sqrt(c + d), 3.0, 0.1067, 2);
    }
    SECTION("Cube Root") { test_uncertain(sigma::cbrt(e), 2.0, 0.0667, 1); }
    SECTION("Exponential Function") {
        SECTION("Natural Exponential") {
            test_uncertain(sigma::exp(a), 2.7183, 0.2718, 1);
        }
        SECTION("Exponential Base 2 Function") {
            test_uncertain(sigma::exp2(a), 2.0, 0.1386, 1);
        }
        SECTION("Exponential Minus 1 Function") {
            test_uncertain(sigma::expm1(a), 1.7183, 0.2718, 1);
        }
    }
    SECTION("Logarithms") {
        SECTION("Natural Log") {
            test_uncertain(sigma::log(a), 0.0, 0.1, 1);
            test_uncertain(sigma::log(b), 0.6931, 0.1, 1);
        }
        SECTION("Log Base 10") {
            test_uncertain(sigma::log10(a), 0.0, 0.0434, 1);
            test_uncertain(sigma::log10(b), 0.3010, 0.0434, 1);
        }
        SECTION("Log Base 2") {
            test_uncertain(sigma::log2(a), 0.0, 0.1443, 1);
            test_uncertain(sigma::log2(b), 1.0, 0.1443, 1);
        }
        SECTION("Natural Log Plus 1") {
            test_uncertain(sigma::log1p(a), 0.6931, 0.05, 1);
            test_uncertain(sigma::log1p(b), 1.0986, 0.0667, 1);
        }
    }
    SECTION("Hypotenuse") {
        SECTION("Two Uncertain Variables") {
            test_uncertain(sigma::hypot(a, b), 2.2361, 0.1844, 2);
        }
        SECTION("One Uncertain Variable") {
            test_uncertain(sigma::hypot(a, 2.0), 2.2361, 0.0447, 1);
            test_uncertain(sigma::hypot(2.0, a), 2.2361, 0.0447, 1);
        }
    }
}
