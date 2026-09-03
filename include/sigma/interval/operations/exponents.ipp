#pragma once
#include "sigma/detail/numeric.hpp"
#include "sigma/interval/detail_/convert.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <utility>

namespace sigma {
namespace detail_ {

/** @brief The openness of the bounds of pow(@p a, @p n) for a positive
 *         integer @p n.
 *
 *  Mirrors the case analysis boost::numeric::pow performs on the sign of the
 *  bounds, since it is that analysis which decides where the bounds of the
 *  result come from.
 *
 *  @tparam T The value type of the interval.
 *
 *  @param[in] a The base. Must not be empty.
 *  @param[in] n The exponent. Must be positive.
 *
 *  @return The openness of the lower and upper bound, respectively.
 *
 *  @throw std::domain_error if @p a is empty. Strong throw guarantee.
 */
template<typename T>
std::pair<bool, bool> integer_pow_openness_(const Interval<T>& a, long long n) {
    const bool is_odd = (n % 2 != 0);
    bool left_open    = a.left_open();
    bool right_open   = a.right_open();

    if(a.upper() < T(0)) {
        // Entirely negative. An odd power is increasing, an even one is
        // decreasing, which swaps which input bound each output bound came
        // from.
        if(!is_odd) { std::swap(left_open, right_open); }
    } else if(a.lower() < T(0)) {
        // Spans zero. An odd power is still increasing across all of it, but
        // an even power dips to an interior minimum at x == 0 and takes its
        // maximum at whichever endpoint is farther from zero.
        if(!is_odd) {
            const T abs_lower = -a.lower();
            left_open         = false; // 0 is attained, and is an interior
                                       // point rather than an input bound
            if(abs_lower > a.upper()) {
                right_open = a.left_open();
            } else if(abs_lower < a.upper()) {
                right_open = a.right_open();
            } else {
                right_open = a.left_open() && a.right_open();
            }
        }
    }
    // Entirely non-negative: increasing, so the openness carries over as-is.

    return {left_open, right_open};
}

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
    // Square root is increasing, so the openness of each bound carries over.
    return detail_::from_boost_(boost::numeric::sqrt(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

template<typename T>
Interval<T> exp(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    return detail_::from_boost_(boost::numeric::exp(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

template<typename T>
Interval<T> log(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    // The domain is the strictly positive reals, so a closed bound at zero is
    // out of domain while an open one is not: (0, 1] contains no non-positive
    // value even though its lower bound is zero. Boost would clamp instead.
    if(a.lower() < T(0) || (a.lower() == T(0) && a.left_closed())) {
        throw std::domain_error("Interval has non-positive values.");
    }
    // An argument bounded below by an open zero is unbounded below in the
    // result, and from_boost_ opens that infinite bound for us. Logarithm is
    // increasing, so the openness of each bound otherwise carries over.
    return detail_::from_boost_(boost::numeric::log(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
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
        if(exp < U(0)) {
            return detail_::from_boost_(y, a.right_open(), a.left_open());
        }
        return detail_::from_boost_(y, a.left_open(), a.right_open());
    }

    const auto power = static_cast<long long>(exp);
    auto [left_open, right_open] =
      detail_::integer_pow_openness_(a, std::abs(power));
    // A negative power is the reciprocal of the positive one, which reverses
    // the interval and so exchanges the openness of its bounds.
    if(power < 0) { std::swap(left_open, right_open); }

    return detail_::from_boost_(boost::numeric::pow(x, static_cast<int>(power)),
                                left_open, right_open);
}

template<typename T>
Interval<T> pow(const Interval<T>& a, const Interval<T>& exponent) {
    if(a.empty() || exponent.empty()) { return Interval<T>(); }
    if(exponent().width() == T(0)) { return pow(a, exponent.lower()); }
    // log() enforces the strictly-positive-base domain restriction and
    // throws std::domain_error itself, so there's nothing left to check
    // here.
    return sigma::exp(exponent * sigma::log(a));
}

} // namespace sigma
