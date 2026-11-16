#pragma once
#include <cmath>

namespace sigma {

template<typename IntervalType>
Triplex<IntervalType> sinh(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::sinh(x); },
      [](auto m) { return std::sinh(m); });
}

template<typename IntervalType>
Triplex<IntervalType> cosh(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::cosh(x); },
      [](auto m) { return std::cosh(m); });
}

template<typename IntervalType>
Triplex<IntervalType> tanh(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::tanh(x); },
      [](auto m) { return std::tanh(m); });
}

template<typename IntervalType>
Triplex<IntervalType> asinh(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::asinh(x); },
      [](auto m) { return std::asinh(m); });
}

template<typename IntervalType>
Triplex<IntervalType> acosh(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::acosh(x); },
      [](auto m) { return std::acosh(m); });
}

template<typename IntervalType>
Triplex<IntervalType> atanh(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::atanh(x); },
      [](auto m) { return std::atanh(m); });
}

} // namespace sigma
