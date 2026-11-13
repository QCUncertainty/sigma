#pragma once
#include "sigma/interval/detail_/convert.hpp"
#include <stdexcept>

namespace sigma {

template<typename T>
Interval<T> sinh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::sinh(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> cosh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }

    // Over an argument spanning zero, boost takes the minimum at the interior
    // point cosh(0) == 1.
    return detail_::from_boost_(boost::numeric::cosh(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> tanh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::tanh(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> asinh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::asinh(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> acosh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // Trimming the argument back to the domain would answer a narrower
    // question than the one asked. Boost would clamp the lower bound to one.
    if(a.lower() < T(1)) {
        throw std::domain_error("Interval has values less than 1.");
    }
    return detail_::from_boost_(boost::numeric::acosh(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> atanh(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // The domain is open, so a bound AT plus or minus one is out of domain.
    detail_::assert_within_unit_(a, true, "inverse hyperbolic tangent");
    return detail_::from_boost_(boost::numeric::atanh(detail_::to_boost_(a)));
}

} // namespace sigma
