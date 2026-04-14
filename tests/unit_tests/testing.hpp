#pragma once
#include <catch2/catch_approx.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sigma/sigma.hpp>

namespace testing {

template<typename T>
struct test_traits;

template<>
struct test_traits<sigma::UFloat> {
    using other_t = sigma::UDouble;
};

template<>
struct test_traits<sigma::UDouble> {
    using other_t = sigma::UFloat;
};

template<>
struct test_traits<sigma::IFloat> {
    using other_t = sigma::IDouble;
};

template<>
struct test_traits<sigma::IDouble> {
    using other_t = sigma::IFloat;
};

template<typename TestType>
void test_uncertain(TestType&& x, double m, double s, std::size_t n) {
    REQUIRE(x.mean() == Catch::Approx(m).margin(1.0e-4));
    REQUIRE(x.sd() == Catch::Approx(s).margin(1.0e-4));
    REQUIRE(x.deps().size() == n);
}

template<typename TestType>
void test_interval(TestType&& x, double lo, double hi) {
    REQUIRE(x.lower() == Catch::Approx(lo).margin(1.0e-4));
    REQUIRE(x.upper() == Catch::Approx(hi).margin(1.0e-4));
    REQUIRE(x.median() == Catch::Approx((lo + hi) / 2.0).margin(1.0e-4));
    REQUIRE(x.radius() == Catch::Approx((hi - lo) / 2.0).margin(1.0e-4));
}

} // namespace testing
