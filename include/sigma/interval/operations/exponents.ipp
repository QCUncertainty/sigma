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
    auto low  = std::pow(a.lower(), exp);
    auto high = std::pow(a.upper(), exp);
    if(low > high) {
        return Interval<T>(high, low, a.right_open(), a.left_open());
    }
    return Interval<T>(low, high, a.left_open(), a.right_open());
}

} // namespace sigma
