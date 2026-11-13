#include "../testing.hpp"
#include <cmath>
#include <stdexcept>

using testing::test_result;

TEMPLATE_TEST_CASE("Hyperbolic", "", TRIPLEX_TYPES) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    testing_t a(value_t{0.2}, value_t{0.3}, value_t{0.7});
    testing_t empty;

    SECTION("Hyperbolic sine") {
        test_result(sigma::sinh(a), sigma::sinh(a.interval()),
                    std::sinh(a.nominal()));
        REQUIRE(sigma::sinh(empty).empty());
    }

    SECTION("Hyperbolic cosine") {
        test_result(sigma::cosh(a), sigma::cosh(a.interval()),
                    std::cosh(a.nominal()));
        REQUIRE(sigma::cosh(empty).empty());
    }

    SECTION("Hyperbolic tangent") {
        test_result(sigma::tanh(a), sigma::tanh(a.interval()),
                    std::tanh(a.nominal()));
        REQUIRE(sigma::tanh(empty).empty());
    }

    SECTION("Inverse hyperbolic sine") {
        test_result(sigma::asinh(a), sigma::asinh(a.interval()),
                    std::asinh(a.nominal()));
        REQUIRE(sigma::asinh(empty).empty());
    }

    SECTION("Inverse hyperbolic cosine") {
        testing_t b(value_t{1.5}, value_t{2}, value_t{4});
        test_result(sigma::acosh(b), sigma::acosh(b.interval()),
                    std::acosh(b.nominal()));
        REQUIRE(sigma::acosh(empty).empty());
        REQUIRE_THROWS_AS(sigma::acosh(a), std::domain_error);
    }

    SECTION("Inverse hyperbolic tangent") {
        test_result(sigma::atanh(a), sigma::atanh(a.interval()),
                    std::atanh(a.nominal()));
        REQUIRE(sigma::atanh(empty).empty());
        testing_t out(value_t{-1}, value_t{0}, value_t{0.5});
        REQUIRE_THROWS_AS(sigma::atanh(out), std::domain_error);
    }
}
