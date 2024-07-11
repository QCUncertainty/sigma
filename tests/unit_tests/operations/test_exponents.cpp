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
            auto x  = pow(a, 2);
            auto y  = pow((a + b), -1);
            auto z  = pow((a + b + a * b), 0.5);
            auto zz = pow((a + b + a * b), 0.0);
            test_uncertain(x, 1.0, 0.2, 1);
            test_uncertain(y, 0.3333, 0.0248, 2);
            test_uncertain(z, 2.2361, 0.1118, 2);
            test_uncertain(zz, 1.0, 0.0, 2);
        }
        SECTION("Uncertain exponent") {
            auto x = pow(a, b);
            auto y = pow(c, -b);
            auto z = pow(c, a * 0.5);
            test_uncertain(x, 1.0, 0.2, 2);
            test_uncertain(y, 0.0625, 0.0214, 2);
            test_uncertain(z, 2.0, 0.1709, 2);
        }
    }
    SECTION("Square Root") {
        auto x = sigma::sqrt(a);
        auto y = sigma::sqrt(c);
        auto z = sigma::sqrt(c + d);
        test_uncertain(x, 1.0, 0.05, 1);
        test_uncertain(y, 2.0, 0.1, 1);
        test_uncertain(z, 3.0, 0.1067, 2);
    }
    SECTION("Exponential Function") {
        auto x = sigma::exp(a);
        test_uncertain(x, 2.7183, 0.2718, 1);
    }
    SECTION("Logarithms") {
        SECTION("Natural Log") {
            auto x = sigma::log(a);
            auto y = sigma::log(b);
            test_uncertain(x, 0.0, 0.1, 1);
            test_uncertain(y, 0.6931, 0.1, 1);
        }
        SECTION("Log Base 10") {
            auto x = sigma::log10(a);
            auto y = sigma::log10(b);
            test_uncertain(x, 0.0, 0.0434, 1);
            test_uncertain(y, 0.3010, 0.0434, 1);
        }
    }
}