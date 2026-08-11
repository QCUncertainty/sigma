#pragma once
#include "sigma/interval/interval.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Interval<T> sqrt(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    auto low  = std::sqrt(a.lower());
    auto high = std::sqrt(a.upper());
    return Interval<T>(low, high, a.left_open(), a.right_open());
}

template<typename T>
Interval<T> exp(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    auto low  = std::exp(a.lower());
    auto high = std::exp(a.upper());
    return Interval<T>(low, high, a.left_open(), a.right_open());
}

template<typename T>
Interval<T> log(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    auto low  = std::log(a.lower());
    auto high = std::log(a.upper());
    return Interval<T>(low, high, a.left_open(), a.right_open());
}

template<typename T, typename U>
Interval<T> pow(const Interval<T>& a, const U& exp) {
    if(a.empty()) { return Interval<T>(); }
    if(exp == U(0)) { return Interval<T>(T(1), T(1)); }

    // x^exp is monotonic across [a.lower(), a.upper()] -- and therefore
    // attains its extrema at the endpoints, which is all the code below
    // checks -- EXCEPT when a spans 0 and exp is a positive even integer: x^2
    // (for example) decreases from a.lower() down to 0 and then increases
    // back up to a.upper(), so 0 (an interior point, not either endpoint) is
    // the true minimum. Handle that case directly; every other combination
    // of sign and exponent is already monotonic end-to-end.
    bool spans_zero = a.lower() < T(0) && a.upper() > T(0);
    bool is_integer = std::floor(exp) == exp;
    if(spans_zero && exp > U(0) && is_integer &&
       static_cast<long long>(exp) % 2 == 0) {
        auto abs_lo = -a.lower();
        auto abs_hi = a.upper();
        if(abs_lo > abs_hi) {
            return Interval<T>(T(0), std::pow(abs_lo, exp), false,
                                a.left_open());
        }
        if(abs_hi > abs_lo) {
            return Interval<T>(T(0), std::pow(abs_hi, exp), false,
                                a.right_open());
        }
        return Interval<T>(T(0), std::pow(abs_hi, exp), false,
                            a.left_open() && a.right_open());
    }

    auto low  = std::pow(a.lower(), exp);
    auto high = std::pow(a.upper(), exp);
    if(low > high) {
        return Interval<T>(high, low, a.right_open(), a.left_open());
    }
    return Interval<T>(low, high, a.left_open(), a.right_open());
}

} // namespace sigma
