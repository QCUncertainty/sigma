#pragma once
#include "sigma/interval/interval.hpp"

/** @file hyperbolic.hpp
 *  @brief Hyperbolic operations for interval variables
 *
 *  The bounds of every result are rounded outward, so the returned interval is
 *  guaranteed to enclose the image of the argument.
 */

namespace sigma {

/** @brief Calculate the hyperbolic sine of an interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose hyperbolic sine is computed
 *
 *  @return An interval enclosing the hyperbolic sine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> sinh(const Interval<T>& a);

/** @brief Calculate the hyperbolic cosine of an interval
 *
 *  Hyperbolic cosine has a minimum of one at the origin, so an interval
 *  spanning zero has a result whose lower bound is one rather than the
 *  hyperbolic cosine of either bound.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose hyperbolic cosine is computed
 *
 *  @return An interval enclosing the hyperbolic cosine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> cosh(const Interval<T>& a);

/** @brief Calculate the hyperbolic tangent of an interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose hyperbolic tangent is computed
 *
 *  @return An interval enclosing the hyperbolic tangent of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> tanh(const Interval<T>& a);

/** @brief Calculate the inverse hyperbolic sine of an interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose inverse hyperbolic sine is computed
 *
 *  @return An interval enclosing the inverse hyperbolic sine of every value in
 *          @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> asinh(const Interval<T>& a);

/** @brief Calculate the inverse hyperbolic cosine of an interval
 *
 *  Inverse hyperbolic cosine is only defined for values of at least one. The
 *  part of @p a below one is discarded, and an interval lying entirely below
 *  one gives the empty interval.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose inverse hyperbolic cosine is computed
 *
 *  @return An interval enclosing the inverse hyperbolic cosine of every value
 *          in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> acosh(const Interval<T>& a);

/** @brief Calculate the inverse hyperbolic tangent of an interval
 *
 *  Inverse hyperbolic tangent is only defined on \f$ (-1, 1) \f$ and is
 *  unbounded at both ends of that domain, so an interval reaching either end
 *  has an unbounded result. An interval lying entirely outside the domain
 *  gives the empty interval.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose inverse hyperbolic tangent is computed
 *
 *  @return An interval enclosing the inverse hyperbolic tangent of every value
 *          in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> atanh(const Interval<T>& a);

} // namespace sigma

#include "hyperbolic.ipp"
