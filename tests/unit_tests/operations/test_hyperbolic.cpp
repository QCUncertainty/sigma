#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Hyperbolic", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(0.785398, 0.1);
    auto b = testing_t(1.0, 0.1);
    auto c = testing_t(2.0, 0.2);

    SECTION("Sine") { test_uncertain(sigma::sinh(a), 0.8687, 0.1325, 1); }
    SECTION("Cosine") { test_uncertain(sigma::cosh(a), 1.3246, 0.0869, 1); }
    SECTION("Tangent") { test_uncertain(sigma::tanh(a), 0.6558, 0.0570, 1); }
    SECTION("Arcsine") { test_uncertain(sigma::asinh(b), 0.8814, 0.0707, 1); }
    SECTION("Arccosine") { test_uncertain(sigma::acosh(c), 1.3170, 0.1155, 1); }
    SECTION("Arctangent") {
        test_uncertain(sigma::atanh(a), 1.0593, 0.2610, 1);
    }
}
