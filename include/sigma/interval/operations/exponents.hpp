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
 *  The square root is only defined for non-negative values, so @p a must not
 *  contain any negative value. The empty interval contains nothing and is
 *  returned unchanged.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose square root is computed
 *
 *  @return An interval enclosing the square root of every value in @p a
 *
 *  @throw std::domain_error if @p a contains a negative value, i.e., if its
 *         lower bound is negative. Strong throw guarantee.
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
 *  The logarithm is only defined for positive values, so @p a must not contain
 *  zero or any negative value. A bound *at* zero is only out of domain if it
 *  is closed: \f$ (0, 1] \f$ contains no non-positive value, and its
 *  logarithm is unbounded below. The empty interval contains nothing and is
 *  returned unchanged.
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
Interval<T> log(const Interval<T>& a);

/** @brief Exponentiation of a variable
 *
 *  Two combinations of base and exponent have no real value, and @p a must not
 *  contain a value in either. A negative base has no real fractional power,
 *  and a zero base has no negative power. As elsewhere, an open bound is not a
 *  value the interval contains, so \f$ (0, 1] \f$ may be raised to a negative
 *  power even though \f$ [0, 1] \f$ may not. The empty interval contains
 *  nothing and is returned unchanged.
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
Interval<T> pow(const Interval<T>& a, const U& exp);

/** @brief Exponentiation of an interval by an interval-valued exponent
 *
 *  Overload of pow(const Interval<T>&, const U&) for the case where the
 *  exponent is itself an Interval<T> rather than a plain scalar -- as
 *  happens, for example, when a caller's exponent is a tuning parameter
 *  stored as the same type T as the base rather than as a raw scalar (a
 *  common pattern for values that don't need their own uncertainty
 *  tracking but end up typed T anyway for API convenience). A general real
 *  power with a real exponent is computed as @f$ x^y = e^{y \ln x} @f$,
 *  which only needs exp/log/multiplication -- all already sound
 *  enclosures -- so this holds for any exponent range, not just the
 *  integer/scalar cases the other overload special-cases; the tradeoff is
 *  that it's looser than the scalar overload's tighter, case-analyzed
 *  bounds, and it inherits log's domain restriction to strictly positive
 *  bases (a negative or zero base has no general real power).
 *
 *  A point exponent (@p exponent has zero width) is expected to be the
 *  common case -- a caller passing a genuinely uncertain exponent is the
 *  exception -- so it is forwarded to the scalar overload instead of going
 *  through log/exp, both for tighter bounds and to avoid needlessly
 *  rejecting a negative base raised to, e.g., an integer point exponent.
 *  The strictly-positive-base restriction below therefore only kicks in
 *  once @p exponent actually has width; a point exponent instead follows
 *  the scalar overload's domain restrictions.
 *
 *  @tparam T The value type of the base and exponent
 *  @param a The interval base
 *  @param exponent The interval exponent to raise the base by
 *
 *  @return An interval enclosing every value in @p a raised to every power
 *          in @p exponent
 *
 *  @throw std::domain_error if @p exponent has width and @p a contains a
 *         non-positive value, i.e., if its lower bound is negative, or is a
 *         closed zero. If @p exponent is a point, throws under the same
 *         conditions as the scalar overload instead. Strong throw
 *         guarantee.
 */
template<typename T>
Interval<T> pow(const Interval<T>& a, const Interval<T>& exponent);

} // namespace sigma

#include "exponents.ipp"
