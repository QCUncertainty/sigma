#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>
#include <sstream>

TEST_CASE("IndependentVariable") {
    using testing_t = sigma::IndependentVariable<double>;
    SECTION("Constructors") {
        SECTION("Default") {
            auto value = testing_t();
            REQUIRE(value.mean() == 0.0);
            REQUIRE(value.std() == 0.0);
        }
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
}