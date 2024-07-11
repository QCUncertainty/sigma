#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Exponents", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1);
    auto b = testing_t(2.0, 0.2);
    auto c = testing_t(4.0, 0.4);
    auto d = testing_t(5.0, 0.5);

    SECTION("Exponentiation") {
        SECTION("Certain exponent") {
            test_uncertain(sigma::pow(a, 2), 1.0, 0.2, 1);
            test_uncertain(sigma::pow((a + b), -1), 0.3333, 0.0248, 2);
            test_uncertain(sigma::pow((a + b + a * b), 0.5), 2.2361, 0.1118, 2);
            test_uncertain(sigma::pow((a + b + a * b), 0.0), 1.0, 0.0, 2);
        }
        SECTION("Uncertain exponent") {
            test_uncertain(sigma::pow(a, b), 1.0, 0.2, 2);
            test_uncertain(sigma::pow(c, -b), 0.0625, 0.0214, 2);
            test_uncertain(sigma::pow(c, a * 0.5), 2.0, 0.1709, 2);
        }
    }
    SECTION("Square Root") {
        test_uncertain(sigma::sqrt(a), 1.0, 0.05, 1);
        test_uncertain(sigma::sqrt(c), 2.0, 0.1, 1);
        test_uncertain(sigma::sqrt(c + d), 3.0, 0.1067, 2);
    }
    SECTION("Exponential Function") {
        test_uncertain(sigma::exp(a), 2.7183, 0.2718, 1);
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
    }
}