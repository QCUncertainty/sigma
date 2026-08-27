#pragma once
#include "sigma/interval/interval.hpp"

/** @file exponents.hpp
 *  @brief Exponent operations for interval variables
 *
 *  The bounds of every result are rounded outward, so the returned interval is
 *  guaranteed to enclose the image of the argument.
 */

namespace sigma {

/** @brief Calculate the square root of an interval
 *
 *  The square root is only defined for non-negative values. The part of @p a
 *  below zero is discarded, and an entirely negative @p a gives the empty
 *  interval.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose square root is computed
 *
 *  @return An interval enclosing the square root of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> sqrt(const Interval<T>& a);

/** @brief Calculate Euler's number raised to the power of an interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval that is the exponent
 *
 *  @return An interval whose bounds are Euler's number raised by @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> exp(const Interval<T>& a);

/** @brief Calculate the natural logarithm of an interval
 *
 *  The logarithm is only defined for positive values, and is unbounded below
 *  at zero. An @p a whose lower bound is non-positive therefore has a result
 *  unbounded below, and an entirely non-positive @p a gives the empty
 *  interval.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose logarithm is determined
 *
 *  @return An interval enclosing the natural logarithm of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> log(const Interval<T>& a);

/** @brief Exponentiation of a variable
 *
 *  @tparam T The value type of the variable
 *  @tparam U The numeric type of the exponent
 *  @param a The interval base
 *  @param exp The exponent to raise the base by
 *
 *  @return A interval whose bounds are the bounds of @p a raised to the power
 *  @p exp
 *
 *  @throw none No throw guarantee
 */
template<typename T, typename U>
Interval<T> pow(const Interval<T>& a, const U& exp);

} // namespace sigma

#include "exponents.ipp"
