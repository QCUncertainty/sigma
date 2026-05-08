#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Trigonometry", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(0.785398, 0.1, 0.001);
    auto b = testing_t(45.0, 0.1, 0.002);
    auto c = testing_t(1.0, 0.1, 0.003);
    auto d = testing_t(2.0, 0.2, 0.004);

    SECTION("Degrees") {
        test_uncertain(sigma::degrees(a), 45.0000, 5.7296, 1, 0.001);
    }
    SECTION("Radians") {
        test_uncertain(sigma::radians(b), 0.7854, 0.0017, 1, 0.002);
    }
    SECTION("Sine") { test_uncertain(sigma::sin(a), 0.7071, 0.0707, 1, 0.001); }
    SECTION("Cosine") {
        test_uncertain(sigma::cos(a), 0.7071, 0.0707, 1, 0.001);
    }
    SECTION("Tangent") {
        test_uncertain(sigma::tan(a), 1.0000, 0.2000, 1, 0.001);
    }
    SECTION("Arcsine") {
        test_uncertain(sigma::asin(a), 0.9033, 0.1616, 1, 0.001);
    }
    SECTION("Arccosine") {
        test_uncertain(sigma::acos(a), 0.6675, 0.1616, 1, 0.001);
    }
    SECTION("Arctangent") {
        test_uncertain(sigma::atan(a), 0.6658, 0.0618, 1, 0.001);
    }
    SECTION("Two Argument Arctangent") {
        SECTION("Two Uncertain Variables") {
            test_uncertain(sigma::atan2(c, d), 0.4636, 0.0566, 2, 0.003);
        }
        SECTION("One Uncertain Variables") {
            test_uncertain(sigma::atan2(c, 2.0), 0.4636, 0.0400, 1, 0.003);
            test_uncertain(sigma::atan2(1.0, d), 0.4636, 0.0400, 1, 0.004);
        }
    }
}
