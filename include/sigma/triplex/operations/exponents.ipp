#pragma once
#include <cmath>
#include <utility>

namespace sigma {

template<typename IntervalType>
Triplex<IntervalType> sqrt(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::sqrt(x); },
      [](auto m) { return std::sqrt(m); });
}

template<typename IntervalType>
Triplex<IntervalType> exp(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::exp(x); },
      [](auto m) { return std::exp(m); });
}

template<typename IntervalType>
Triplex<IntervalType> log(const Triplex<IntervalType>& a) {
    return detail_::apply_unary_(
      a, [](const IntervalType& x) { return sigma::log(x); },
      [](auto m) { return std::log(m); });
}

template<typename IntervalType, typename U>
Triplex<IntervalType> pow(const Triplex<IntervalType>& a, const U& exp) {
    return detail_::apply_unary_(
      a, [&exp](const IntervalType& x) { return sigma::pow(x, exp); },
      [&exp](auto m) { return std::pow(m, exp); });
}

template<typename IntervalType>
Triplex<IntervalType> pow(const Triplex<IntervalType>& a,
                          const Triplex<IntervalType>& exponent) {
    using value_t = typename IntervalType::value_t;
    if(a.empty() || exponent.empty()) { return Triplex<IntervalType>(); }
    auto bounds     = sigma::pow(a.interval(), exponent.interval());
    value_t nominal = std::pow(a.nominal(), exponent.nominal());
    return detail_::make_triplex_(std::move(bounds), nominal);
}

} // namespace sigma
