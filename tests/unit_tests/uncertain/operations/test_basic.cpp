#include "../testing.hpp"

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Basic", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1, 0.001);
    auto b = testing_t(1.3, 0.1, 0.002);
    auto c = testing_t(2.0, 0.2, 0.003);
    auto d = testing_t(3.0, 0.3, 0.004);

    SECTION("Copy sign") {
        REQUIRE(a == sigma::copysign(a, b));
        REQUIRE(-a == sigma::copysign(a, -b));
        REQUIRE(-a == sigma::copysign(a, -1.0));
        REQUIRE(-1.0 == sigma::copysign(1.0, -b));
        test_uncertain(a + sigma::copysign(a, -1.0), 0.0, 0.0, 0, 0.001);
    }
    SECTION("Absolute Value") {
        REQUIRE(a == sigma::abs(-a));
        REQUIRE(a == sigma::fabs(-a));
    }
    SECTION("Absolute Value Squared") { REQUIRE((c * c) == sigma::abs2(-c)); }
    SECTION("Ceiling") { test_uncertain(sigma::ceil(b), 2.0, 0.0, 0, 0.002); }
    SECTION("Floor") { test_uncertain(sigma::floor(b), 1.0, 0.0, 0, 0.002); }
    SECTION("Float modulo") {
        test_uncertain(sigma::fmod(d, c), 1.0, 0.3606, 2, 0.004);
        test_uncertain(sigma::fmod(d, 2.0), 1.0, 0.3, 1, 0.004);
        test_uncertain(sigma::fmod(3.0, c), 1.0, 0.2, 1, 0.003);
    }
    SECTION("Truncation") {
        test_uncertain(sigma::trunc(b), 1.0, 0.0, 0, 0.002);
        test_uncertain(sigma::trunc(-b), -1.0, 0.0, 0, 0.002);
    }
    SECTION("Round") {
        test_uncertain(sigma::round(b), 1.0, 0.0, 0, 0.002);
        test_uncertain(sigma::round(-b), -1.0, 0.0, 0, 0.002);
        test_uncertain(sigma::round(testing_t{0.5}), 1.0, 0.0, 0);
    }
}
