#pragma once
#include "sigma/interval/detail_/convert.hpp"
#include <algorithm>
#include <stdexcept>

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
    // Trimming the argument back to the domain would answer a narrower
    // question than the one asked. Boost would clamp the lower bound to one.
    if(a.lower() < T(1)) {
        throw std::domain_error("Interval has values less than 1.");
    }
    // Increasing, so the openness of each bound carries over.
    return detail_::from_boost_(boost::numeric::acosh(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

template<typename T>
Interval<T> atanh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // The domain is open, so a bound AT plus or minus one is out of domain
    // only if it is closed -- the same rule log() applies at zero.
    detail_::assert_within_unit_(a, true, "inverse hyperbolic tangent");
    // Increasing. An open bound at either end of the domain leaves the result
    // unbounded on that side, and from_boost_ opens that infinite bound.
    return detail_::from_boost_(boost::numeric::atanh(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

} // namespace sigma
