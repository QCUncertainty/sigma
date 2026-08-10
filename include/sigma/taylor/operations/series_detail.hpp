#pragma once
#include <cstddef>
#include <sigma/detail/numeric.hpp>
#include <vector>

/** @file series_detail.hpp
 *  @brief Shared helpers for building the outer Taylor series of the
 *         elementary functions (exp, log, 1/x, sqrt).
 *
 *  These are used both by Taylor's own (non-rigorous) elementary functions in
 *  operations/exponents.ipp and by TaylorModel's, which additionally need the
 *  (n+1)-th derivative of the same outer function for the Lagrange remainder
 *  of Eq. \f$\eqref{eq:tm-outer-rem}\f$ (see docs/taylor.md).
 *
 *  is_integer_exponent(), used by pow() but not itself series-related, lives
 *  in sigma/detail/numeric.hpp and is re-exposed here (via the include
 *  above) so existing callers of `detail::is_integer_exponent` under
 *  sigma/taylor/ keep working unqualified.
 */

namespace sigma::detail {

/// Returns k! as a value_t.
template<typename T>
T factorial(std::size_t k) {
    T result{1};
    for(std::size_t i = 2; i <= k; ++i) { result *= static_cast<T>(i); }
    return result;
}

/// Returns the k-th binomial-series coefficient C(1/2, k) = \prod_{i=0}^{k-1}
/// (1/2 - i) / k!, used by the Taylor series of sqrt.
template<typename T>
T half_binomial_coefficient(std::size_t k) {
    if(k == 0) { return T{1}; }
    T numerator{1};
    for(std::size_t i = 0; i < k; ++i) {
        numerator *= (T(0.5) - static_cast<T>(i));
    }
    return numerator / factorial<T>(k);
}

/// Returns the k-th falling factorial (1/2)(1/2-1)...(1/2-k+1), i.e.
/// half_binomial_coefficient(k) * k!, used for the (n+1)-th derivative of
/// sqrt.
template<typename T>
T half_falling_factorial(std::size_t k) {
    if(k == 0) { return T{1}; }
    T result{1};
    for(std::size_t i = 0; i < k; ++i) {
        result *= (T(0.5) - static_cast<T>(i));
    }
    return result;
}

/// Outer-series coefficients of exp(x) about c, i.e. exp(c)/k! for
/// k = 0, ..., order.
template<typename T>
std::vector<T> exp_outer_coeffs(T c, std::size_t order) {
    std::vector<T> coeffs(order + 1);
    T ec = std::exp(c);
    for(std::size_t k = 0; k <= order; ++k) {
        coeffs[k] = ec / factorial<T>(k);
    }
    return coeffs;
}

/// Outer-series coefficients of log(x) about c: log(c) for k=0, and
/// (-1)^(k-1) / (k c^k) for k >= 1.
template<typename T>
std::vector<T> log_outer_coeffs(T c, std::size_t order) {
    std::vector<T> coeffs(order + 1);
    coeffs[0]   = std::log(c);
    T c_power_k = T{1};
    for(std::size_t k = 1; k <= order; ++k) {
        c_power_k *= c;
        T sign    = (k % 2 == 1) ? T{1} : T{-1};
        coeffs[k] = sign / (static_cast<T>(k) * c_power_k);
    }
    return coeffs;
}

/// Outer-series coefficients of 1/x about c: (-1)^k / c^(k+1).
template<typename T>
std::vector<T> reciprocal_outer_coeffs(T c, std::size_t order) {
    std::vector<T> coeffs(order + 1);
    T c_power_kp1 = c;
    for(std::size_t k = 0; k <= order; ++k) {
        T sign    = (k % 2 == 0) ? T{1} : T{-1};
        coeffs[k] = sign / c_power_kp1;
        c_power_kp1 *= c;
    }
    return coeffs;
}

/// Outer-series coefficients of sqrt(x) about c: sqrt(c) * C(1/2,k) / c^k.
template<typename T>
std::vector<T> sqrt_outer_coeffs(T c, std::size_t order) {
    std::vector<T> coeffs(order + 1);
    T sqrt_c    = std::sqrt(c);
    T c_power_k = T{1};
    for(std::size_t k = 0; k <= order; ++k) {
        coeffs[k] = sqrt_c * half_binomial_coefficient<T>(k) / c_power_k;
        c_power_k *= c;
    }
    return coeffs;
}

} // namespace sigma::detail
