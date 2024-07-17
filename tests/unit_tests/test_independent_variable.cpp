#include "sigma/detail_/independent_variable.hpp"
#include <catch2/catch.hpp>
#include <sstream>

TEMPLATE_TEST_CASE("IndependentVariable", "", float, double) {
    TestType sd{0.1};
    sigma::IndependentVariable<TestType> value{sd};
    REQUIRE(value.sd() == sd);
}