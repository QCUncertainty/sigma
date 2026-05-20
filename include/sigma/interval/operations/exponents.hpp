#pragma once
#include "sigma/interval/interval.hpp"

/** @file exponents.hpp
 *  @brief Exponent operations for interval variables
 */

namespace sigma {

/** @brief Calculate the square root of an interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose square root is computed
 *
 *  @return An interval whose bounds are the square root of @p a
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
 *  @tparam T The value type of the interval
 *  @param a The interval whose logarithm is determined
 *
 *  @return An interval whose bounds are the natural logarithm of @p a
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
