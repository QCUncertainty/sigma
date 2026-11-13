#pragma once
#include "sigma/detail/numeric.hpp"
#include "sigma/interval/detail_/convert.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>

namespace sigma {
namespace detail_ {

/** @brief The exponent of a non-integer pow(), as an interval.
 *
 *  Converting @p exp to the interval's value type may not be exact, e.g., when
 *  a double exponent is applied to an IFloat. When it is not, the exponent is
 *  widened to the pair of representable values bracketing it so the enclosure
 *  remains valid.
 *
 *  @tparam T The value type of the interval.
 *  @tparam U The numeric type of the exponent.
 *
 *  @param[in] exp The exponent to convert.
 *
 *  @return An interval containing @p exp.
 *
 *  @throw none No throw guarantee.
 */
template<typename T, typename U>
boost_interval_t<T> exponent_interval_(const U& exp) {
    const auto value = static_cast<T>(exp);
    if(static_cast<U>(value) == exp) { return boost_interval_t<T>(value); }
    constexpr auto inf = std::numeric_limits<T>::infinity();
    return boost_interval_t<T>(std::nextafter(value, -inf),
                               std::nextafter(value, inf));
}

} // namespace detail_

template<typename T>
Interval<T> sqrt(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // Silently dropping the part of a that is out of domain would answer a
    // question that was not asked, so say so instead. Boost would clamp the
    // lower bound to zero.
    if(a.lower() < T(0)) {
        throw std::domain_error("Interval has negative values.");
    }
    return detail_::from_boost_(boost::numeric::sqrt(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> exp(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::exp(detail_::to_boost_(a)));
}

template<typename T>
Interval<T> log(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // The domain is the strictly positive reals, so a lower bound at zero is
    // out of domain. Boost would clamp instead.
    if(a.lower() <= T(0)) {
        throw std::domain_error("Interval has non-positive values.");
    }
    return detail_::from_boost_(boost::numeric::log(detail_::to_boost_(a)));
}

template<typename T, typename U>
Interval<T> pow(const Interval<T>& a, const U& exp) {
    if(a.empty()) { return Interval<T>(); }
    if(exp == U(0)) { return Interval<T>(T(1), T(1)); }

    // A negative exponent is a reciprocal, and dividing by an interval that
    // contains zero is the error operator/= already rejects.
    if(exp < U(0) && a.contains(T(0))) {
        throw std::domain_error(
          "Can not raise an interval containing 0 to a negative power.");
    }

    auto x = detail_::to_boost_(a);

    if(!detail::is_integer_exponent(exp)) {
        // A negative base raised to a fractional power is not a real number,
        // so the base must be non-negative. Zero itself is fine for a positive
        // exponent: the evaluation below runs through log, whose unbounded
        // result exponentiates back to the 0 that 0^exp really is.
        if(a.lower() < T(0)) {
            throw std::domain_error("Can not raise an interval with negative "
                                    "values to a non-integer power.");
        }
        // Away from that, a non-integer power is monotonic: increasing for a
        // positive exponent, decreasing for a negative one.
        auto y = boost::numeric::exp(detail_::exponent_interval_<T>(exp) *
                                     boost::numeric::log(x));
        return detail_::from_boost_(y);
    }

    const auto power = static_cast<long long>(exp);
    return detail_::from_boost_(
      boost::numeric::pow(x, static_cast<int>(power)));
}

template<typename T>
Interval<T> pow(const Interval<T>& a, const Interval<T>& exponent) {
    if(a.empty() || exponent.empty()) { return Interval<T>(); }
    // The point-exponent case is expected to be far more common than a
    // genuinely uncertain exponent, so it is forwarded to the scalar
    // overload for tighter bounds and that overload's more permissive
    // domain restriction (e.g., a negative base raised to an integer power
    // is fine, even though it is not a valid base for log()).
    if(exponent.width() == T(0)) { return pow(a, exponent.lower()); }
    // log() enforces the strictly-positive-base domain restriction and
    // throws std::domain_error itself, so there's nothing left to check
    // here.
    return sigma::exp(exponent * sigma::log(a));
}

} // namespace sigma
