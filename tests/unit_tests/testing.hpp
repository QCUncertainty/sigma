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

template<typename TestType>
void test_uncertain(TestType&& x, double m, double s, std::size_t n) {
    REQUIRE(x.mean() == Catch::Approx(m).margin(1.0e-4));
    REQUIRE(x.sd() == Catch::Approx(s).margin(1.0e-4));
    REQUIRE(x.deps().size() == n);
}

} // namespace testing
