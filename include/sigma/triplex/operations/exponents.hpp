#pragma once
#include "sigma/triplex/detail_/apply_unary.hpp"
#include "sigma/triplex/triplex.hpp"

/** @file exponents.hpp
 *  @brief Exponent operations for triplex variables
 *
 *  The domain of each function is that of the corresponding function of the
 *  interval type. The nominal value lies in the interval, so whenever the
 *  interval function accepts the argument the nominal value is in the domain
 *  too.
 */

namespace sigma {

/** @brief Calculate the square root of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex whose square root is computed
 *
 *  @return The square root of @p a
 *
 *  @throw std::domain_error if @p a contains a negative value. Strong throw
 *         guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> sqrt(const Triplex<IntervalType>& a);

/** @brief Calculate Euler's number raised to the power of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex that is the exponent
 *
 *  @return Euler's number raised to @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> exp(const Triplex<IntervalType>& a);

/** @brief Calculate the natural logarithm of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex whose logarithm is determined
 *
 *  @return The natural logarithm of @p a
 *
 *  @throw std::domain_error if @p a contains a non-positive value. Strong
 *         throw guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> log(const Triplex<IntervalType>& a);

/** @brief Exponentiation of a triplex by a scalar
 *
 *  The nominal value is std::pow applied to the nominal value of @p a and to
 *  @p exp.
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @tparam U The numeric type of the exponent
 *  @param a The triplex base
 *  @param exp The exponent to raise the base by
 *
 *  @return @p a raised to the power @p exp
 *
 *  @throw std::domain_error if @p exp is negative and @p a contains zero, or
 *         if @p exp is not an integer and @p a contains a negative value.
 *         Strong throw guarantee.
 */
template<typename IntervalType, typename U>
Triplex<IntervalType> pow(const Triplex<IntervalType>& a, const U& exp);

/** @brief Exponentiation of a triplex by a triplex
 *
 *  The bounds are those of the interval overload of pow, which forwards a
 *  point exponent to the scalar overload and otherwise requires a positive
 *  base. The nominal value is std::pow applied to the two nominal values.
 *
 *  @tparam IntervalType The interval type of the triplex numbers
 *  @param a The triplex base
 *  @param exponent The triplex exponent to raise the base by
 *
 *  @return @p a raised to the power @p exponent. Empty if either argument is.
 *
 *  @throw std::domain_error if @p exponent has width and @p a contains a
 *         non-positive value. If @p exponent is a point, throws under the same
 *         conditions as the scalar overload instead. Strong throw guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> pow(const Triplex<IntervalType>& a,
                          const Triplex<IntervalType>& exponent);

} // namespace sigma

#include "exponents.ipp"
