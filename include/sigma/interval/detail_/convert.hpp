#pragma once
#include "sigma/interval/detail_/policies.hpp"
#include "sigma/interval/interval.hpp"

/** @file convert.hpp
 *  @brief Conversions between sigma::Interval and the boost interval it wraps.
 *
 *  The elementary functions of an Interval are implemented by handing the
 *  bounds to boost, which does the outwardly rounded numerics, and then
 *  restoring the two pieces of state boost does not track: emptiness and bound
 *  openness. These helpers are the two halves of that round trip.
 *
 *  @note On openness. Boost has no notion of an open bound, so each caller
 *        states what the openness of the result should be:
 *
 *        - For a monotonically increasing function the bounds of the result
 *          come from the bounds of the input, so the input's openness carries
 *          over unchanged.
 *        - For a monotonically decreasing function the bounds swap, and so
 *          does their openness.
 *        - When a bound comes from an interior extremum (the maximum of cos,
 *          the minimum of cosh) or from a clamp (the 0 of sqrt, the
 *          \f$ \pm\pi/2 \f$ of asin), it does not correspond to an input bound
 *          at all. Such a bound is reported closed: the value really is
 *          attained, and in the cases where it is not, a closed bound is a
 *          superset of an open one and therefore still a valid enclosure.
 */

namespace sigma::detail_ {

/** @brief Convert a sigma::Interval into the boost interval it wraps.
 *
 *  @tparam T The value type of the interval.
 *
 *  @param[in] a The interval to convert. Must not be empty.
 *
 *  @return A boost interval with the same bounds as @p a. The openness of
 *          @p a's bounds is not represented in the result.
 *
 *  @throw std::domain_error if @p a is empty. Strong throw guarantee.
 */
template<typename T>
boost_interval_t<T> to_boost_(const Interval<T>& a) {
    return boost_interval_t<T>(a.lower(), a.upper());
}

/** @brief Convert a boost interval into a sigma::Interval.
 *
 *  @tparam T The value type of the interval.
 *
 *  @param[in] x The interval to convert.
 *  @param[in] left_open Should the lower bound of the result be open?
 *  @param[in] right_open Should the upper bound of the result be open?
 *
 *  @return An interval with the bounds of @p x and the requested openness. An
 *          empty boost interval, which is how boost signals that a function
 *          was given an argument outside its domain, converts to the empty
 *          sigma::Interval. An infinite bound is always open, regardless of
 *          what the caller requested, since it is not a value the interval
 *          contains.
 *
 *  @throw none No throw guarantee.
 */
template<typename T>
Interval<T> from_boost_(const boost_interval_t<T>& x, bool left_open = false,
                        bool right_open = false) {
    if(boost::numeric::empty(x)) { return Interval<T>(); }
    if(std::isinf(x.lower())) { left_open = true; }
    if(std::isinf(x.upper())) { right_open = true; }
    return Interval<T>(x.lower(), x.upper(), left_open, right_open);
}

} // namespace sigma::detail_
