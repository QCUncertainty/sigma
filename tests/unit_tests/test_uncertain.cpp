#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>
#include <sstream>

template<typename UncertainType>
void test_uncertain(UncertainType x, double m, double s, std::size_t n) {
    REQUIRE(x.mean() == Approx(m).margin(1.0e-4));
    REQUIRE(x.std() == Approx(s).margin(1.0e-4));
    REQUIRE(x.deps().size() == n);
}

TEMPLATE_TEST_CASE("Uncertain", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    SECTION("Constructors") {
        SECTION("Default") {
            auto value = testing_t();
            test_uncertain(value, 0.0, 0.0, 0);
        }
        SECTION("With Values") {
            auto value = testing_t(1.0, 0.1);
            test_uncertain(value, 1.0, 0.1, 1);
        }
        SECTION("Copy") {
            auto first = testing_t(1.0, 0.1);
            testing_t value(first);
            test_uncertain(value, 1.0, 0.1, 1);
            test_uncertain(first, 1.0, 0.1, 1);
        }
        SECTION("Move") {
            auto first = testing_t(1.0, 0.1);
            testing_t value(std::move(first));
            test_uncertain(value, 1.0, 0.1, 1);
            test_uncertain(first, 1.0, 0.1, 0);
        }
        SECTION("Copy Assignment") {
            auto first = testing_t(1.0, 0.1);
            auto value = first;
            test_uncertain(value, 1.0, 0.1, 1);
            test_uncertain(first, 1.0, 0.1, 1);
        }
        SECTION("Move Assignment") {
            auto first = testing_t(1.0, 0.1);
            auto value = std::move(first);
            test_uncertain(value, 1.0, 0.1, 1);
            test_uncertain(first, 1.0, 0.1, 0);
        }
    }
    SECTION("Operations") {
        auto a = testing_t(1.0, 0.1);
        auto b = testing_t(2.0, 0.2);
        auto c = testing_t(2.0, 0.2); // Same value, different variable
        auto d = testing_t(3.0, 0.3);

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
        SECTION("Division") {
            auto x = a / b;
            auto y = x / d;
            test_uncertain(x, 0.5, 0.0707, 2);
            test_uncertain(y, 0.1667, 0.0289, 3);
        }
        SECTION("Scale") {
            int two = 2;
            auto x  = a * two;         // Works with int
            auto y  = (double)two * a; // Works with floating point
            auto z  = a + a;
            test_uncertain(x, 2.0, 0.2, 1);
            test_uncertain(y, 2.0, 0.2, 1);
            test_uncertain(z, 2.0, 0.2, 1);
            REQUIRE(x == y);
            REQUIRE(x == z);
        }
        SECTION("Exponentiation") {
            auto x = a.pow(2);
            auto y = (a + b).pow(-1);
            auto z = (a + b + a * b).pow(0.5);
            test_uncertain(x, 1.0, 0.2, 1);
            test_uncertain(y, 0.3333, 0.0248, 2);
            test_uncertain(z, 2.2361, 0.1118, 2);
        }
        SECTION("Mixing Operations") {
            auto x = 2.0 * a + b * 2.0 + (a * b);
            auto y = (d * b - a) / b;
            auto z = (d * b - a) / c; // Same mean as y, different std and deps
            test_uncertain(x, 8.0, 0.7211, 2);
            test_uncertain(y, 2.5, 0.3082, 3);
            test_uncertain(z, 2.5, 0.4950, 4);
        }
    }
    SECTION("operator<<(std::ostream, IndependentVariable)") {
        value_t mean = 1.0, std = 0.1;
        auto value = testing_t(mean, std);
        std::stringstream ss, corr;
        ss << value;
        corr << mean << "+/-" << std;
        REQUIRE(ss.str() == corr.str());
    }
    SECTION("Comparisons") {
        SECTION("Same") {
            auto first  = testing_t(1.0, 0.1);
            auto second = first;
            REQUIRE(first == second);
            REQUIRE_FALSE(first != second);
        }
        SECTION("Different Mean") {
            auto first  = testing_t(1.0, 0.1);
            auto second = testing_t(1.1, 0.1);
            REQUIRE_FALSE(first == second);
        }
        SECTION("Different Standard Deviation") {
            auto first  = testing_t(1.0, 0.1);
            auto second = testing_t(1.0, 0.2);
            REQUIRE_FALSE(first == second);
        }
        SECTION("Different Mean") {
            auto first  = testing_t(1.0, 0.1);
            auto second = testing_t(1.0, 0.1);
            REQUIRE_FALSE(first == second);
        }
    }
}