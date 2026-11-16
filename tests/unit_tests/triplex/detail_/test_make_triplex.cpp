#include "../testing.hpp"
#include <limits>
#include <sigma/triplex/detail_/make_triplex.hpp>

TEMPLATE_TEST_CASE("make_triplex_", "", TRIPLEX_TYPES) {
    using testing_t  = TestType;
    using value_t    = typename testing_t::value_t;
    using interval_t = typename testing_t::interval_t;
    using sigma::detail_::make_triplex_;

    interval_t bounds(value_t{1}, value_t{2});

    SECTION("Nominal value inside is left alone") {
        auto r = make_triplex_(bounds, value_t{1.5});
        REQUIRE(r == testing_t(bounds, value_t{1.5}));
    }

    SECTION("Nominal value outside widens the bounds, not the nominal value") {
        // As when a libm result is a few ulps off from the interval's
        auto r = make_triplex_(bounds, value_t{3});
        REQUIRE(r.nominal() == value_t{3});
        REQUIRE(r.lower() == value_t{1});
        REQUIRE(r.upper() == value_t{3});
    }

    SECTION("Non-finite nominal value is kept and nothing is widened") {
        constexpr auto inf = std::numeric_limits<value_t>::infinity();
        auto r             = make_triplex_(bounds, inf);
        REQUIRE(r.nominal() == inf);
        REQUIRE(r.interval() == bounds);
    }
}
