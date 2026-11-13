#pragma once
#include "sigma/interval/interval.hpp"
#include <algorithm>
#include <cmath>

namespace sigma {

template<typename T>
Interval<T> abs(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // If interval is positive already, just return it.
    if(a.lower() >= 0) { return a; }

    // If interval is completely negative, return the negative of it.
    if(a.upper() <= 0) { return -a; }

    // We know it straddles zero, so the minimum is the interior point zero.
    return Interval<T>(0, std::max(std::abs(a.lower()), std::abs(a.upper())));
}

template<typename T>
Interval<T> fabs(const Interval<T>& a) {
    return abs(a);
}

} // namespace sigma
