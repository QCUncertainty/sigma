#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>
#include <sstream>

TEST_CASE("IndependentVariable") {
    using testing_t = sigma::IndependentVariable<double>;
    SECTION("Constructors") {
        SECTION("With Values") {
            auto value = testing_t(1.0, 0.1);
            REQUIRE(value.mean() == 1.0);
            REQUIRE(value.std() == 0.1);
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