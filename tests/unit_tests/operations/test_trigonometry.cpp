#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Trigonometry", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(0.785398, 0.1);

    SECTION("Sine") { test_uncertain(sigma::sin(a), 0.7071, 0.0707, 1); }
    SECTION("Cosine") { test_uncertain(sigma::cos(a), 0.7071, 0.0707, 1); }
    SECTION("Tangent") { test_uncertain(sigma::tan(a), 1.0000, 0.2000, 1); }
    SECTION("Arcsine") { test_uncertain(sigma::asin(a), 0.9033, 0.1616, 1); }
    SECTION("Arccosine") { test_uncertain(sigma::acos(a), 0.6675, 0.1616, 1); }
    SECTION("Arctangent") { test_uncertain(sigma::atan(a), 0.6658, 0.0618, 1); }
}