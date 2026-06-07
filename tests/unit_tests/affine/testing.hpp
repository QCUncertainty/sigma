#pragma once
#include <catch2/catch_approx.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sigma/affine/affine.hpp>
#include <type_traits>

namespace testing {

template<typename T>
struct test_traits;

template<>
struct test_traits<sigma::AFloat> {
    using other_t = sigma::ADouble;
};

template<>
struct test_traits<sigma::ADouble> {
    using other_t = sigma::AFloat;
};

template<typename TestType>
void test_affine(TestType&& x, double lo, double hi) {
    // REQUIRE(x.lower() == Catch::Approx(lo).margin(1.0e-4));
    // REQUIRE(x.upper() == Catch::Approx(hi).margin(1.0e-4));
    // REQUIRE(x.median() == Catch::Approx((lo + hi) / 2.0).margin(1.0e-4));
    // REQUIRE(x.radius() == Catch::Approx((hi - lo) / 2.0).margin(1.0e-4));
}

} // namespace testing
