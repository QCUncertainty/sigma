#include "../testing.hpp"
#include <sigma/triplex/detail_/apply_unary.hpp>
#include <stdexcept>

TEMPLATE_TEST_CASE("apply_unary_", "", TRIPLEX_TYPES) {
    using testing_t  = TestType;
    using value_t    = typename testing_t::value_t;
    using interval_t = typename testing_t::interval_t;
    using sigma::detail_::apply_unary_;

    testing_t a(value_t{1}, value_t{2}, value_t{4});

    // Doubles the bounds and squares the nominal value, so it is visible which
    // callable produced which part of the result
    auto twice  = [](const interval_t& x) { return x * value_t{2}; };
    auto square = [](value_t m) { return m * m; };

    SECTION("Bounds and nominal value come from their own callables") {
        auto r = apply_unary_(a, twice, square);
        REQUIRE(r.interval() == interval_t(value_t{2}, value_t{8}));
        REQUIRE(r.nominal() == value_t{4});
    }

    SECTION("The widening rule is applied") {
        // A nominal value of 16 misses the bounds [2, 8], which are widened
        auto r =
          apply_unary_(a, twice, [](value_t m) { return m * m * m * m; });
        REQUIRE(r.nominal() == value_t{16});
        REQUIRE(r.lower() == value_t{2});
        REQUIRE(r.upper() == value_t{16});
    }

    SECTION("The empty triplex is returned without calling either callable") {
        bool called = false;
        auto r      = apply_unary_(
          testing_t(),
          [&called](const interval_t& x) {
              called = true;
              return x;
          },
          [&called](value_t m) {
              called = true;
              return m;
          });
        REQUIRE(r.empty());
        REQUIRE_FALSE(called);
    }

    SECTION("An exception from the bounds callable propagates") {
        auto reject = [](const interval_t&) -> interval_t {
            throw std::domain_error("out of domain");
        };
        testing_t copy(a);
        REQUIRE_THROWS_AS(apply_unary_(a, reject, square), std::domain_error);
        REQUIRE(a == copy);
    }
}
