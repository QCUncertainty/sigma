#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>
#include <sstream>

template<typename UncertainType>
void test_uncertain(UncertainType x, double m, double s, std::size_t n) {
    REQUIRE(x.mean() == Approx(m).margin(1.0e-4));
    REQUIRE(x.std() == Approx(s).margin(1.0e-4));
    REQUIRE(x.deps().size() == n);
}

TEMPLATE_TEST_CASE("Operations", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1);
    auto b = testing_t(2.0, 0.2);
    auto c = testing_t(2.0, 0.2); // Same value, different variable
    auto d = testing_t(3.0, 0.3);
    auto e = testing_t(4.0, 0.4);
    auto f = testing_t(5.0, 0.5);

    SECTION("Negation") {
        auto x = -a;
        auto y = -(a + b);
        test_uncertain(x, -1.0, 0.1, 1);
        test_uncertain(y, -3.0, 0.2236, 2);
    }
    SECTION("Addition") {
        auto x = a + b;
        auto y = x + d;
        test_uncertain(x, 3.0, 0.2236, 2);
        test_uncertain(y, 6.0, 0.3742, 3);
    }
    SECTION("Subtraction") {
        auto x = a - b;
        auto y = x - d;
        test_uncertain(x, -1.0, 0.2236, 2);
        test_uncertain(y, -4.0, 0.3742, 3);
    }
    SECTION("Multiplication") {
        auto x = a * b;
        auto y = x * d;
        test_uncertain(x, 2.0, 0.2828, 2);
        test_uncertain(y, 6.0, 1.0392, 3);
    }
    SECTION("Scale") {
        int two = 2;
        auto x  = a * two;         // Works with int
        auto y  = (double)two * a; // Works with floating point, and reversed
        test_uncertain(x, 2.0, 0.2, 1);
        test_uncertain(y, 2.0, 0.2, 1);
        REQUIRE(x == y);
        REQUIRE(x == (a + a));
    }
    SECTION("Division") {
        auto x = a / b;
        auto y = x / d;
        test_uncertain(x, 0.5, 0.0707, 2);
        test_uncertain(y, 0.1667, 0.0289, 3);
    }
    SECTION("Exponentiation") {
        auto x = pow(a, 2);
        auto y = pow((a + b), -1);
        auto z = pow((a + b + a * b), 0.5);
        test_uncertain(x, 1.0, 0.2, 1);
        test_uncertain(y, 0.3333, 0.0248, 2);
        test_uncertain(z, 2.2361, 0.1118, 2);
    }
    SECTION("Square Root") {
        auto x = sigma::sqrt(a);
        auto y = sigma::sqrt(e);
        auto z = sigma::sqrt(e + f);
        test_uncertain(x, 1.0, 0.05, 1);
        test_uncertain(y, 2.0, 0.1, 1);
        test_uncertain(z, 3.0, 0.1067, 2);
    }
}