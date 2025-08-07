#include "testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Complex", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    testing_t a{1.0, 0.1};

    SECTION("Real Numbers") {
        SECTION("Complex Conjugate") { REQUIRE(a == sigma::conj(a)); }
        SECTION("Real Part") { REQUIRE(a == sigma::real(a)); }
        SECTION("Imaginary Part") {
            test_uncertain(sigma::imag(a), 0.0, 0.0, 1);
        }
    }
}
