#pragma once
#include <cmath>
#include <sigma/detail/pow.hpp>
#include <sigma/taylor/operations/series_detail.hpp>
#include <sigma/taylor/taylor.hpp>

namespace sigma {

template<typename T>
Taylor<T> sqrt(const Taylor<T>& a) {
    if(a.empty()) { return a; }
    if(a.bound().lower() <= T(0.0)) {
        throw std::domain_error("Taylor polynomial has non-positive values.");
    }
    return a.compose_(
      detail::sqrt_outer_coeffs(a.constant(), a.max_order()));
}

template<typename T>
Taylor<T> exp(const Taylor<T>& a) {
    if(a.empty()) { return a; }
    return a.compose_(detail::exp_outer_coeffs(a.constant(), a.max_order()));
}

template<typename T>
Taylor<T> log(const Taylor<T>& a) {
    if(a.empty()) { return a; }
    if(a.bound().lower() <= T(0.0)) {
        throw std::domain_error("Taylor polynomial has non-positive values.");
    }
    return a.compose_(detail::log_outer_coeffs(a.constant(), a.max_order()));
}

template<typename T, typename U>
Taylor<T> pow(const Taylor<T>& a, const U& exp) {
    auto order = a.max_order();
    return detail::pow_impl(a, exp, [order](T v) {
        return Taylor<T>(v, typename Taylor<T>::Order(order));
    });
}

} // namespace sigma
