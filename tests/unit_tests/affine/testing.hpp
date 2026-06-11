#pragma once
#include "../interval/testing.hpp"
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
    test_interval(x.range(), lo, hi);
}

} // namespace testing
