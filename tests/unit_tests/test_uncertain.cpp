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