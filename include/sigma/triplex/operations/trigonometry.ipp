#pragma once
#include <cmath>

namespace sigma {

template<typename IntervalType>
Triplex<IntervalType> sin(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::sin(x); },
      [](auto m) { return std::sin(m); });
}

template<typename IntervalType>
Triplex<IntervalType> cos(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::cos(x); },
      [](auto m) { return std::cos(m); });
}

template<typename IntervalType>
Triplex<IntervalType> tan(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::tan(x); },
      [](auto m) { return std::tan(m); });
}

template<typename IntervalType>
Triplex<IntervalType> asin(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::asin(x); },
      [](auto m) { return std::asin(m); });
}

template<typename IntervalType>
Triplex<IntervalType> acos(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::acos(x); },
      [](auto m) { return std::acos(m); });
}

template<typename IntervalType>
Triplex<IntervalType> atan(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::atan(x); },
      [](auto m) { return std::atan(m); });
}

} // namespace sigma
