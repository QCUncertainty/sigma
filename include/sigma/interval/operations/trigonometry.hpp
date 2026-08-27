#pragma once
#include "sigma/interval/interval.hpp"

/** @file trigonometry.hpp
 *  @brief Trigonometric operations for interval variables
 *
 *  The bounds of every result are rounded outward, so the returned interval is
 *  guaranteed to enclose the image of the argument.
 */

namespace sigma {

/** @brief Calculate the sine of an interval
 *
 *  Sine is not monotonic, so the result is not in general the sine of the
 *  bounds: an interval spanning a peak encloses the peak. The bounds of the
 *  result are always closed.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose sine is computed
 *
 *  @return An interval enclosing the sine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> sin(const Interval<T>& a);

/** @brief Calculate the cosine of an interval
 *
 *  Cosine is not monotonic, so the result is not in general the cosine of the
 *  bounds: an interval spanning a peak encloses the peak. The bounds of the
 *  result are always closed.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose cosine is computed
 *
 *  @return An interval enclosing the cosine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> cos(const Interval<T>& a);

/** @brief Calculate the tangent of an interval
 *
 *  Tangent is unbounded at odd multiples of \f$ \pi/2 \f$. An interval
 *  containing such a point has no finite enclosure, so the result is the whole
 *  real line. The bounds of the result are always closed, unless they are
 *  infinite.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose tangent is computed
 *
 *  @return An interval enclosing the tangent of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> tan(const Interval<T>& a);

/** @brief Calculate the arcsine of an interval
 *
 *  Arcsine is only defined on \f$ [-1, 1] \f$. The part of @p a outside that
 *  domain is discarded, and an interval lying entirely outside it gives the
 *  empty interval.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose arcsine is computed
 *
 *  @return An interval enclosing the arcsine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> asin(const Interval<T>& a);

/** @brief Calculate the arccosine of an interval
 *
 *  Arccosine is only defined on \f$ [-1, 1] \f$. The part of @p a outside that
 *  domain is discarded, and an interval lying entirely outside it gives the
 *  empty interval. Arccosine is decreasing, so the bounds of @p a exchange
 *  places.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose arccosine is computed
 *
 *  @return An interval enclosing the arccosine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> acos(const Interval<T>& a);

/** @brief Calculate the arctangent of an interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose arctangent is computed
 *
 *  @return An interval enclosing the arctangent of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> atan(const Interval<T>& a);

} // namespace sigma

#include "trigonometry.ipp"
