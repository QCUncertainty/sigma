#pragma once
#include "sigma/interval/general_interval.hpp"

/** @file general_operations/exponents.hpp
 *  @brief Exponent operations for general interval variables
 *
 *  These follow the Interval overloads, except that an open bound is not a
 *  value the interval contains, so it never puts the argument out of domain.
 */

namespace sigma {

/** @brief Calculate the square root of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose square root is computed
 *
 *  @return An interval enclosing the square root of every value in @p a
 *
 *  @throw std::domain_error if @p a contains a negative value. Strong throw
 *         guarantee.
 */
template<typename T>
GeneralInterval<T> sqrt(const GeneralInterval<T>& a);

/** @brief Calculate Euler's number raised to the power of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval that is the exponent
 *
 *  @return An interval whose bounds are Euler's number raised by @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> exp(const GeneralInterval<T>& a);

/** @brief Calculate the natural logarithm of a general interval
 *
 *  A bound *at* zero is only out of domain if it is closed:
 *  \f$ (0, 1\rbrack \f$ contains no non-positive value, and its logarithm is
 *  unbounded below.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose logarithm is determined
 *
 *  @return An interval enclosing the natural logarithm of every value in @p a
 *
 *  @throw std::domain_error if @p a contains a non-positive value, i.e., if
 *         its lower bound is negative, or is a closed zero. Strong throw
 *         guarantee.
 */
template<typename T>
GeneralInterval<T> log(const GeneralInterval<T>& a);

/** @brief Exponentiation of a general interval
 *
 *  A negative base has no real fractional power, and a zero base has no
 *  negative power. An open bound is not a value the interval contains, so
 *  \f$ (0, 1\rbrack \f$ may be raised to a negative power even though
 *  \f$ [0, 1] \f$ may not.
 *
 *  @tparam T The value type of the variable
 *  @tparam U The numeric type of the exponent
 *  @param a The interval base
 *  @param exp The exponent to raise the base by
 *
 *  @return An interval enclosing every value in @p a raised to the power
 *          @p exp
 *
 *  @throw std::domain_error if @p exp is negative and @p a contains zero, or
 *         if @p exp is not an integer and @p a contains a negative value.
 *         Strong throw guarantee.
 */
template<typename T, typename U>
GeneralInterval<T> pow(const GeneralInterval<T>& a, const U& exp);

/** @brief Exponentiation of a general interval by an interval exponent
 *
 *  As for pow(const Interval<T>&, const Interval<T>&): a point exponent is
 *  forwarded to the scalar overload, and any other exponent is evaluated as
 *  \f$ e^{y \ln x} \f$, which restricts the base to positive values.
 *
 *  @tparam T The value type of the base and exponent
 *  @param a The interval base
 *  @param exponent The interval exponent to raise the base by
 *
 *  @return An interval enclosing every value in @p a raised to every power
 *          in @p exponent
 *
 *  @throw std::domain_error if @p exponent has width and @p a contains a
 *         non-positive value. If @p exponent is a point, throws under the
 *         same conditions as the scalar overload instead. Strong throw
 *         guarantee.
 */
template<typename T>
GeneralInterval<T> pow(const GeneralInterval<T>& a,
                       const GeneralInterval<T>& exponent);

} // namespace sigma

#include "exponents.ipp"
