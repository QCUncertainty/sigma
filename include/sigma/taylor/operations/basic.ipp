#pragma once
#include <algorithm>
#include <cmath>
#include <sigma/taylor/taylor.hpp>

namespace sigma {

template<typename T>
Taylor<T> abs(const Taylor<T>& a) {
    if(a.empty()) { return a; }
    auto a_bound = a.bound();
    if(a_bound.lower() >= T(0.0)) {
        return a;
    } else if(a_bound.upper() <= T(0.0)) {
        return -a;
    }
    auto m = std::max(std::abs(a_bound.lower()), a_bound.upper());
    return Taylor<T>(typename Taylor<T>::interval_t(T(0.0), m),
                     typename Taylor<T>::Order(a.max_order()));
}

template<typename T>
Taylor<T> fabs(const Taylor<T>& a) {
    return abs(a);
}

} // namespace sigma
