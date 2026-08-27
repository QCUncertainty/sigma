#pragma once
#include "sigma/interval/detail_/convert.hpp"
#include <algorithm>

namespace sigma {

template<typename T>
Interval<T> sinh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::sinh(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

template<typename T>
Interval<T> cosh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }

    bool left_open  = a.left_open();
    bool right_open = a.right_open();
    if(a.upper() < T(0)) {
        // Entirely negative, where cosh is decreasing
        std::swap(left_open, right_open);
    } else if(a.lower() < T(0)) {
        // Spans zero, so the minimum is the interior point cosh(0) == 1 and
        // the maximum comes from whichever bound is farther from zero.
        const T abs_lower = -a.lower();
        left_open         = false;
        if(abs_lower > a.upper()) {
            right_open = a.left_open();
        } else if(abs_lower < a.upper()) {
            right_open = a.right_open();
        } else {
            right_open = a.left_open() && a.right_open();
        }
    }

    return detail_::from_boost_(boost::numeric::cosh(detail_::to_boost_(a)),
                                left_open, right_open);
}

template<typename T>
Interval<T> tanh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::tanh(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

template<typename T>
Interval<T> asinh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::asinh(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

template<typename T>
Interval<T> acosh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // A lower bound below one is clamped to the edge of the domain, and a
    // bound that came from a clamp rather than from a bound of a is closed.
    auto left_open = a.lower() < T(1) ? false : a.left_open();
    return detail_::from_boost_(boost::numeric::acosh(detail_::to_boost_(a)),
                                left_open, a.right_open());
}

template<typename T>
Interval<T> atanh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // Reaching either end of the domain gives an infinite bound, which
    // from_boost_ opens for us.
    return detail_::from_boost_(boost::numeric::atanh(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

} // namespace sigma
