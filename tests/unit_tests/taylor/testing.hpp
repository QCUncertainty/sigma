#pragma once
#include "../interval/testing.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sigma/taylor/taylor.hpp>
#include <sigma/taylor/taylor_model.hpp>
#include <type_traits>

namespace testing {

template<typename T>
struct test_traits;

template<>
struct test_traits<sigma::TFloat> {
    using other_t = sigma::TDouble;
};

template<>
struct test_traits<sigma::TDouble> {
    using other_t = sigma::TFloat;
};

template<>
struct test_traits<sigma::TMFloat> {
    using other_t = sigma::TMDouble;
};

template<>
struct test_traits<sigma::TMDouble> {
    using other_t = sigma::TMFloat;
};

template<typename TestType>
void test_taylor(TestType&& x, double lo, double hi) {
    test_interval(x.range(), lo, hi);
}

} // namespace testing
