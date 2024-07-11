#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Basic", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1);
    auto b = testing_t(2.0, 0.2);
    auto c = testing_t(3.0, 0.3);

    SECTION("Absolute Value") { REQUIRE(a == sigma::abs(-a)); }
    SECTION("Float modulo") {
        test_uncertain(sigma::fmod(c, b), 1.0, 0.3606, 2);
        test_uncertain(sigma::fmod(c, 2.0), 1.0, 0.3, 1);
        test_uncertain(sigma::fmod(3.0, b), 1.0, 0.2, 1);
    }
}