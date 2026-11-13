#pragma once
#include <catch2/catch_approx.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sigma/triplex/triplex.hpp>

/// The triplex instantiations every templated test runs over
#define TRIPLEX_TYPES                                                \
    sigma::TPFloat, sigma::TPDouble, sigma::Triplex<sigma::GIFloat>, \
      sigma::Triplex<sigma::GIDouble>

namespace testing {

/** @brief Checks the three values of a triplex to within a small margin.
 *
 *  Use for results that went through libm or outward rounding, whose exact
 *  values are not the point of the test.
 */
template<typename TestType>
void test_triplex(TestType&& x, double lo, double nominal, double hi) {
    REQUIRE(x.lower() == Catch::Approx(lo).margin(1.0e-4));
    REQUIRE(x.nominal() == Catch::Approx(nominal).margin(1.0e-4));
    REQUIRE(x.upper() == Catch::Approx(hi).margin(1.0e-4));
}

/** @brief Checks a triplex result against its two independent computations.
 *
 *  The bounds must be exactly those of the interval operation and the nominal
 *  value exactly that of the scalar operation.
 */
template<typename TriplexType>
void test_result(const TriplexType& r,
                 const typename TriplexType::interval_t& bounds,
                 typename TriplexType::value_t nominal) {
    REQUIRE(r.interval() == bounds);
    REQUIRE(r.nominal() == nominal);
}

/** @brief Checks that a triplex satisfies lower() <= nominal() <= upper().
 *
 *  For a general triplex the nominal value must also not sit on an open bound.
 */
template<typename TestType>
void test_invariant(TestType&& x) {
    REQUIRE(x.contains(x.nominal()));
}

} // namespace testing
