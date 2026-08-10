#pragma once
#include <algorithm>
#include <cmath>
#include <sigma/affine/affine.hpp>

namespace sigma {

template<typename T>
Affine<T> abs(const Affine<T>& a) {
    if(a.empty()) { return a; }
    auto a_range = a.range();
    if(a_range.lower() >= T(0.0)) {
        return a;
    } else if(a_range.upper() <= T(0.0)) {
        return -a;
    }
    auto m = std::max(std::abs(a_range.lower()), a_range.upper());
    return Affine<T>(typename Affine<T>::interval_t(T(0.0), m));
}

template<typename T>
Affine<T> fabs(const Affine<T>& a) {
    return abs(a);
}

} // namespace sigma
