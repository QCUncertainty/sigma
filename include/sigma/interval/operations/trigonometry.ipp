#pragma once
#include "sigma/interval/detail_/convert.hpp"
#include <cmath>
#include <stdexcept>

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

    auto y = boost::numeric::tan(detail_::to_boost_(a));

    // Boost reduces the argument against the period and reports "this reaches
    // a pole" by returning the whole real line, which is the only way it
    // produces an unbounded result: every other path bounds the argument
    // strictly inside a single branch, where the tangent is finite. That makes
    // the test below exactly boost's own pole detection, with no period
    // arithmetic of ours to get wrong.
    //
    // The detection is conservative, as it must be: an odd multiple of pi/2 is
    // not representable, so for an argument whose bound sits within rounding
    // distance of one, the reduction cannot certify that the pole is excluded.
    // Such an argument is refused rather than answered.
    if(std::isinf(y.lower()) && std::isinf(y.upper())) {
        throw std::domain_error("Interval contains an odd multiple of pi/2, "
                                "where the tangent is undefined.");
    }

    // Every remaining argument lies strictly inside one branch, on which the
    // tangent is increasing, so the openness of each bound carries over.
    return detail_::from_boost_(y, a.left_open(), a.right_open());
}

template<typename T>
Interval<T> asin(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // Trimming the argument back to the domain would answer a narrower
    // question than the one asked. Boost would do exactly that.
    detail_::assert_within_unit_(a, false, "arcsine");
    // Increasing, so the openness of each bound carries over.
    return detail_::from_boost_(boost::numeric::asin(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

template<typename T>
Interval<T> acos(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    detail_::assert_within_unit_(a, false, "arccosine");
    // Decreasing, so the lower bound of the result comes from the upper bound
    // of a and vice versa.
    return detail_::from_boost_(boost::numeric::acos(detail_::to_boost_(a)),
                                a.right_open(), a.left_open());
}

template<typename T>
Interval<T> atan(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::atan(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

} // namespace sigma
