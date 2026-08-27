#pragma once
#include "sigma/detail/numeric.hpp"
#include "sigma/interval/detail_/convert.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
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
    // A negative lower bound is clamped to zero by boost, and a bound that
    // came from a clamp rather than from an input bound is closed.
    auto left_open = a.lower() < T(0) ? false : a.left_open();
    return detail_::from_boost_(boost::numeric::sqrt(detail_::to_boost_(a)),
                                left_open, a.right_open());
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
    // A non-positive lower bound gives an unbounded result, whose lower bound
    // from_boost_ opens for us.
    return detail_::from_boost_(boost::numeric::log(detail_::to_boost_(a)),
                                a.left_open(), a.right_open());
}

template<typename T, typename U>
Interval<T> pow(const Interval<T>& a, const U& exp) {
    if(a.empty()) { return Interval<T>(); }
    if(exp == U(0)) { return Interval<T>(T(1), T(1)); }

    auto x = detail_::to_boost_(a);

    if(!detail::is_integer_exponent(exp)) {
        // A non-integer power is only defined for a non-negative base, and is
        // monotonic there: increasing for a positive exponent, decreasing for
        // a negative one.
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

} // namespace sigma
