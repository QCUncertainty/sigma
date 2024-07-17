#include "sigma/detail_/independent_variable.hpp"
#include <catch2/catch.hpp>
#include <sstream>

TEMPLATE_TEST_CASE("IndependentVariable", "", float, double) {
    using value_t   = TestType;
    using testing_t = sigma::IndependentVariable<value_t>;

    value_t mean = 1.0, sd = 0.1;
    auto value = testing_t(mean, sd);

    SECTION("Constructors") {
        REQUIRE(value.mean() == mean);
        REQUIRE(value.sd() == sd);
    }
    SECTION("operator<<(std::ostream, IndependentVariable)") {
        std::stringstream ss, corr;
        ss << value;
        corr << mean << "+/-" << sd;
        REQUIRE(ss.str() == corr.str());
    }
    SECTION("Comparisons") {
        auto a = testing_t(1.0, 0.1);
        auto b = testing_t(1.0, 0.1);
        auto c = testing_t(2.0, 0.1);
        auto d = testing_t(1.0, 0.2);
        SECTION("Same") {
            REQUIRE(a == b);
            REQUIRE_FALSE(a != b);
        }
        SECTION("Different Mean") { REQUIRE_FALSE(a == c); }
        SECTION("Different Standard Deviation") { REQUIRE_FALSE(a == d); }
    }
}