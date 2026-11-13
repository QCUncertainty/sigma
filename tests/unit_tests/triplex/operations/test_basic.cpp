#include "../testing.hpp"
#include <cmath>

using testing::test_result;

TEMPLATE_TEST_CASE("Basic", "", TRIPLEX_TYPES) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    testing_t pos(value_t{1}, value_t{2}, value_t{3});
    testing_t neg(value_t{-3}, value_t{-2}, value_t{-1});
    testing_t mixed(value_t{-3}, value_t{-1}, value_t{2});

    SECTION("Absolute value") {
        for(const auto& a : {pos, neg, mixed}) {
            test_result(sigma::abs(a), sigma::abs(a.interval()),
                        std::abs(a.nominal()));
            test_result(sigma::fabs(a), sigma::fabs(a.interval()),
                        std::fabs(a.nominal()));
        }
        // |<-3, -1, 2>| = <0, 1, 3>: the nominal value is |m|, not inferred
        // from the bounds
        REQUIRE(sigma::abs(mixed) ==
                testing_t(value_t{0}, value_t{1}, value_t{3}));
        REQUIRE(sigma::abs(testing_t()).empty());
    }
}
