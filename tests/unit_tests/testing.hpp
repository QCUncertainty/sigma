#pragma once
#include <catch2/catch.hpp>

namespace testing {

template<typename UncertainType>
void test_uncertain(UncertainType x, double m, double s, std::size_t n) {
    REQUIRE(x.mean() == Approx(m).margin(1.0e-4));
    REQUIRE(x.std() == Approx(s).margin(1.0e-4));
    REQUIRE(x.deps().size() == n);
}

} // namespace testing