#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>
#include <sstream>

void test_udouble(sigma::UDouble x, double mean, double std,
                  std::size_t n_deps) {
    REQUIRE(x.mean() == Approx(mean).margin(1.0e-4));
    REQUIRE(x.std() == Approx(std).margin(1.0e-4));
    REQUIRE(x.deps().size() == n_deps);
}

TEST_CASE("Uncertain") {
    using testing_t = sigma::UDouble;
    SECTION("Constructors") {
        SECTION("Default") {
            auto value = testing_t();
            test_udouble(value, 0.0, 0.0, 0);
        }
        SECTION("With Values") {
            auto value = testing_t(1.0, 0.1);
            test_udouble(value, 1.0, 0.1, 1);
        }
        SECTION("Copy") {
            auto first = testing_t(1.0, 0.1);
            testing_t value(first);
            test_udouble(value, 1.0, 0.1, 1);
            test_udouble(first, 1.0, 0.1, 1);
        }
        SECTION("Move") {
            auto first = testing_t(1.0, 0.1);
            testing_t value(std::move(first));
            test_udouble(value, 1.0, 0.1, 1);
            test_udouble(first, 1.0, 0.1, 0);
        }
        SECTION("Copy Assignment") {
            auto first = testing_t(1.0, 0.1);
            auto value = first;
            test_udouble(value, 1.0, 0.1, 1);
            test_udouble(first, 1.0, 0.1, 1);
        }
        SECTION("Move Assignment") {
            auto first = testing_t(1.0, 0.1);
            auto value = std::move(first);
            test_udouble(value, 1.0, 0.1, 1);
            test_udouble(first, 1.0, 0.1, 0);
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
            test_udouble(x, -1.0, 0.1, 1);
            test_udouble(y, -3.0, 0.2236, 2);
        }
        SECTION("Addition") {
            auto x = a + b;
            auto y = x + d;
            test_udouble(x, 3.0, 0.2236, 2);
            test_udouble(y, 6.0, 0.3742, 3);
        }
        SECTION("Subtraction") {
            auto x = a - b;
            auto y = x - d;
            test_udouble(x, -1.0, 0.2236, 2);
            test_udouble(y, -4.0, 0.3742, 3);
        }
        SECTION("Multiplication") {
            auto x = a * b;
            auto y = x * d;
            test_udouble(x, 2.0, 0.2828, 2);
            test_udouble(y, 6.0, 1.0392, 3);
        }
        SECTION("Division") {
            auto x = a / b;
            auto y = x / d;
            test_udouble(x, 0.5, 0.0707, 2);
            test_udouble(y, 0.1667, 0.0289, 3);
        }
        SECTION("Scale") {
            int two = 2;
            auto x  = a * two;         // Works with int
            auto y  = (double)two * a; // Works with float
            auto z  = a + a;
            test_udouble(x, 2.0, 0.2, 1);
            test_udouble(y, 2.0, 0.2, 1);
            test_udouble(z, 2.0, 0.2, 1);
            REQUIRE(x == y);
            REQUIRE(x == z);
        }
        SECTION("Exponentiation") {
            auto x = a.pow(2);
            auto y = (a + b).pow(-1);
            auto z = (a + b + a * b).pow(0.5);
            test_udouble(x, 1.0, 0.2, 1);
            test_udouble(y, 0.3333, 0.0248, 2);
            test_udouble(z, 2.2361, 0.1118, 2);
        }
        SECTION("Mixing Operations") {
            auto x = 2.0 * a + b * 2.0 + (a * b);
            auto y = (d * b - a) / b;
            auto z = (d * b - a) / c; // Same mean as y, different std and deps
            test_udouble(x, 8.0, 0.7211, 2);
            test_udouble(y, 2.5, 0.3082, 3);
            test_udouble(z, 2.5, 0.4950, 4);
        }
    }
    SECTION("operator<<(std::ostream, IndependentVariable)") {
        double mean = 1.0, std = 0.1;
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