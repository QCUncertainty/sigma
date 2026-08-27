#pragma once
#include "sigma/interval/detail_/convert.hpp"

namespace sigma {

template<typename T>
Interval<T> sin(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // Each bound may come from a peak interior to a rather than from a bound
    // of a, so both are reported closed.
    return detail_::from_boost_(boost::numeric::sin(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> cos(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::cos(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> tan(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::tan(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> asin(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // Increasing, so the openness carries over, except where a bound came
    // from the edge of the domain rather than from a bound of a.
    auto left_open  = a.lower() <= T(-1) ? false : a.left_open();
    auto right_open = a.upper() >= T(1) ? false : a.right_open();
    return detail_::from_boost_(boost::numeric::asin(detail_::to_boost_(a)),
                                left_open, right_open);
}

template<typename T>
Interval<T> acos(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // Decreasing, so the lower bound of the result comes from the upper bound
    // of a and vice versa.
    auto left_open  = a.upper() >= T(1) ? false : a.right_open();
    auto right_open = a.lower() <= T(-1) ? false : a.left_open();
    return detail_::from_boost_(boost::numeric::acos(detail_::to_boost_(a)),
                                left_open, right_open);
}

template<typename T>
Interval<T> atan(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::atan(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

} // namespace sigma
