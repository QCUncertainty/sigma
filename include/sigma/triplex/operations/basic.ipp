#pragma once
#include <cmath>

namespace sigma {

template<typename IntervalType>
Triplex<IntervalType> abs(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::abs(x); },
      [](auto m) { return std::abs(m); });
}

template<typename IntervalType>
Triplex<IntervalType> fabs(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::fabs(x); },
      [](auto m) { return std::fabs(m); });
}

} // namespace sigma
