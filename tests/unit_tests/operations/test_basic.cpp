#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Basic", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1);
    auto b = testing_t(1.3, 0.1);
    auto c = testing_t(2.0, 0.2);
    auto d = testing_t(3.0, 0.3);

    SECTION("Absolute Value") { REQUIRE(a == sigma::abs(-a)); }
    SECTION("Ceiling") { test_uncertain(sigma::ceil(b), 2.0, 0.0, 0); }
    SECTION("Floor") { test_uncertain(sigma::floor(b), 1.0, 0.0, 0); }
    SECTION("Float modulo") {
        test_uncertain(sigma::fmod(d, c), 1.0, 0.3606, 2);
        test_uncertain(sigma::fmod(d, 2.0), 1.0, 0.3, 1);
        test_uncertain(sigma::fmod(3.0, c), 1.0, 0.2, 1);
    }
}