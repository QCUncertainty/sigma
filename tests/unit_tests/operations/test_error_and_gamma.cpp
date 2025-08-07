#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Error Function and Gamma", "", sigma::UFloat,
                   sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1);
    SECTION("Error Function") {
        test_uncertain(sigma::erf(a), 0.8427, 0.0415, 1);
    }
    SECTION("Complementary Error Function") {
        test_uncertain(sigma::erfc(a), 0.1573, 0.0415, 1);
        test_uncertain(sigma::erf(a) + sigma::erfc(a), 1.0, 0.0, 0);
    }
    SECTION("Gamma Function") {
        test_uncertain(sigma::tgamma(a), 1.0, 0.0577, 1);
    }
    SECTION("Gamma Function Natural Logarithm") {
        test_uncertain(sigma::lgamma(a), 0.0, 0.0577, 1);
    }
}
