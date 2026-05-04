#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_interval;
using testing::test_uncertain;

TEMPLATE_TEST_CASE("Exponents (Interval)", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;

    testing_t empty;
    testing_t one_two(1.0, 2.0);
    testing_t one_two_left_open(1.0, 2.0, true, false);
    testing_t one_two_right_open(1.0, 2.0, false, true);
    testing_t one_two_open(1.0, 2.0, true, true);

    SECTION("Square Root") {
        auto low  = std::sqrt(one_two.lower());
        auto high = std::sqrt(one_two.upper());
        testing_t result(low, high);
        testing_t result_left_open(low, high, true, false);
        testing_t result_right_open(low, high, false, true);
        testing_t result_open(low, high, true, true);

        REQUIRE(sigma::sqrt(empty) == empty);
        REQUIRE(sigma::sqrt(one_two) == result);
        REQUIRE(sigma::sqrt(one_two_left_open) == result_left_open);
        REQUIRE(sigma::sqrt(one_two_right_open) == result_right_open);
        REQUIRE(sigma::sqrt(one_two_open) == result_open);
    }
    SECTION("Exponential Function") {
        auto low  = std::exp(one_two.lower());
        auto high = std::exp(one_two.upper());
        testing_t result(low, high);
        testing_t result_left_open(low, high, true, false);
        testing_t result_right_open(low, high, false, true);
        testing_t result_open(low, high, true, true);

        REQUIRE(sigma::exp(empty) == empty);
        REQUIRE(sigma::exp(one_two) == result);
        REQUIRE(sigma::exp(one_two_left_open) == result_left_open);
        REQUIRE(sigma::exp(one_two_right_open) == result_right_open);
        REQUIRE(sigma::exp(one_two_open) == result_open);
    }
    SECTION("Natural Logarithm") {
        auto low  = std::log(one_two.lower());
        auto high = std::log(one_two.upper());
        testing_t result(low, high);
        testing_t result_left_open(low, high, true, false);
        testing_t result_right_open(low, high, false, true);
        testing_t result_open(low, high, true, true);

        REQUIRE(sigma::log(empty) == empty);
        REQUIRE(sigma::log(one_two) == result);
        REQUIRE(sigma::log(one_two_left_open) == result_left_open);
        REQUIRE(sigma::log(one_two_right_open) == result_right_open);
        REQUIRE(sigma::log(one_two_open) == result_open);
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

TEMPLATE_TEST_CASE("Exponents (Affine)", "", float, double) {
    using affine_t = sigma::Affine<TestType>;
    using value_t  = typename affine_t::value_t;

    SECTION("Square Root") {
        auto a = affine_t(value_t{1}, value_t{2});
        // TODO: Establish tight range
        test_interval(sigma::sqrt(a).range(), 1.0, 1.431980515);
    }
}

TEMPLATE_TEST_CASE("Exponents (PartitionedAffine)", "", float, double) {
    using affine_t = sigma::PartitionedAffine<TestType>;
    using value_t  = typename affine_t::value_t;

    SECTION("Square Root") {
        auto a = affine_t(value_t{1}, value_t{2});
        // TODO: Establish tight range
        test_interval(sigma::sqrt(a).range(), 1.0, 1.41432835267698676);
    }
}
