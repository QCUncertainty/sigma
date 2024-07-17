#include "testing.hpp"
#include <sigma/sigma.hpp>
#include <sstream>

using testing::test_uncertain;

template<typename T>
struct test_traits;

template<>
struct test_traits<sigma::UFloat> {
    using other_t = sigma::UDouble;
};

template<>
struct test_traits<sigma::UDouble> {
    using other_t = sigma::UFloat;
};

TEMPLATE_TEST_CASE("Uncertain", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;
    using other_t   = typename test_traits<TestType>::other_t;

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
        auto first = testing_t(1.0, 0.1);
        SECTION("Same") {
            auto second = first;
            REQUIRE(first == second);
            REQUIRE_FALSE(first != second);
        }
        SECTION("Different Value Type") {
            auto second = other_t(1.1, 0.1);
            REQUIRE_FALSE(first == second);
        }
        SECTION("Different Mean") {
            auto second = testing_t(1.1, 0.1);
            REQUIRE_FALSE(first == second);
        }
        SECTION("Different Standard Deviation") {
            auto second = testing_t(1.0, 0.2);
            REQUIRE_FALSE(first == second);
        }
        SECTION("Different Dependencies") {
            auto second = testing_t(1.0, 0.1);
            REQUIRE_FALSE(first == second);
        }
        SECTION("Relative") {
            auto second = testing_t(2.0, 0.2);
            auto third  = testing_t(2.0, 0.2);
            auto fourth = second;
            SECTION("Less than") {
                REQUIRE(first < second);
                REQUIRE_FALSE(second < first);
                REQUIRE_FALSE(third < second);
            }
            SECTION("Greater than") {
                REQUIRE(second > first);
                REQUIRE_FALSE(first > second);
                REQUIRE_FALSE(third > second);
            }
            SECTION("Less than or equal") {
                REQUIRE(first <= second);
                REQUIRE(fourth <= second);
                REQUIRE_FALSE(second <= first);
                REQUIRE_FALSE(third <= second);
            }
            SECTION("Greater than or equal") {
                REQUIRE(second >= first);
                REQUIRE(fourth >= second);
                REQUIRE_FALSE(first >= second);
                REQUIRE_FALSE(third >= second);
            }
        }
    }
}
