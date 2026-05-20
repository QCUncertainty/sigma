#pragma once
#include <catch2/catch_approx.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sigma/uncertain/uncertain.hpp>
#include <type_traits>

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

#define VALUE_TYPE typename std::remove_reference<TestType>::type::value_t

template<typename TestType>
void test_uncertain(
  TestType&& x, VALUE_TYPE m, VALUE_TYPE s, std::size_t n,
  VALUE_TYPE threshold = std::numeric_limits<VALUE_TYPE>::epsilon()) {
    REQUIRE(x.mean() == Catch::Approx(m).margin(1.0e-4));
    REQUIRE(x.sd() == Catch::Approx(s).margin(1.0e-4));
    REQUIRE(x.deps().size() == n);
    REQUIRE(x.threshold() == threshold);
}

#undef VALUE_TYPE

} // namespace testing
