#pragma once
#include "sigma/interval/general_interval.hpp"
#include <cmath>

namespace sigma {

template<typename T>
GeneralInterval<T> abs(const GeneralInterval<T>& a) {
    if(a.empty()) { return GeneralInterval<T>(); }
    // If interval is positive already, just return it.
    if(a.lower() >= 0) { return a; }

    // If interval is completely negative, return the negative of it.
    if(a.upper() <= 0) { return -a; }

    // We know it straddles zero.
    auto abs_low  = std::abs(a.lower());
    auto abs_high = std::abs(a.upper());
    if(abs_low < abs_high) {
        return GeneralInterval<T>(0, abs_high, false, a.right_open());
    } else if(abs_low > abs_high) {
        return GeneralInterval<T>(0, abs_low, false, a.left_open());
    } else { // Equal in magnitude
        return GeneralInterval<T>(0, abs_low, false, a.left_open() && a.right_open());
    }
}

template<typename T>
GeneralInterval<T> fabs(const GeneralInterval<T>& a) {
    return abs(a);
}

} // namespace sigma
