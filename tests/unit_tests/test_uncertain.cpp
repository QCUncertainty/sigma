#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>
#include <sstream>

void test_udouble(sigma::UDouble x, double mean, double std, int n_deps) {
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
    }
    SECTION("Operations") {
        auto a = testing_t(1.0, 0.1);
        auto b = testing_t(2.0, 0.2);
        auto c = testing_t(2.0, 0.2); // Same value, different variable
        auto d = testing_t(3.0, 0.3);

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
        SECTION("Mixing Operations") {
            auto x = a + b + (a * b);
            auto y = (d * b - a) / b;
            auto z = (d * b - a) / c; // Same mean as y, different std and deps
            test_udouble(x, 5.0, 0.5000, 2);
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
}