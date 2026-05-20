#include "../testing.hpp"

using testing::test_interval;

TEMPLATE_TEST_CASE("Basic", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    SECTION("Absolute Value") {
        SECTION("All positive") {
            auto a = testing_t(value_t{1}, value_t{3});
            test_interval(sigma::abs(a), 1.0, 3.0);
            test_interval(sigma::fabs(a), 1.0, 3.0);
        }
        SECTION("All negative") {
            auto a = testing_t(value_t{-3}, value_t{-1});
            test_interval(sigma::abs(a), 1.0, 3.0);
            test_interval(sigma::fabs(a), 1.0, 3.0);
        }
        SECTION("Straddles zero") {
            auto a = testing_t(value_t{-2}, value_t{3});
            test_interval(sigma::abs(a), 0.0, 3.0);
            test_interval(sigma::fabs(a), 0.0, 3.0);
        }
    }
}
