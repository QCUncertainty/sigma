#pragma once
#include "sigma/triplex/detail_/apply_unary.hpp"
#include "sigma/triplex/triplex.hpp"

/** @file trigonometry.hpp
 *  @brief Trigonometric operations for triplex variables
 *
 *  The domain of each function is that of the corresponding function of the
 *  interval type.
 */

namespace sigma {

/** @brief Calculate the sine of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex, in radians
 *
 *  @return The sine of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> sin(const Triplex<IntervalType>& a);

/** @brief Calculate the cosine of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex, in radians
 *
 *  @return The cosine of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> cos(const Triplex<IntervalType>& a);

/** @brief Calculate the tangent of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex, in radians
 *
 *  @return The tangent of @p a
 *
 *  @throw std::domain_error if @p a contains, or cannot be shown to exclude,
 *         an odd multiple of \f$ \pi/2 \f$. Strong throw guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> tan(const Triplex<IntervalType>& a);

/** @brief Calculate the arcsine of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The arcsine of @p a, in radians
 *
 *  @throw std::domain_error if @p a contains a value outside
 *         \f$ [-1, 1] \f$. Strong throw guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> asin(const Triplex<IntervalType>& a);

/** @brief Calculate the arccosine of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The arccosine of @p a, in radians
 *
 *  @throw std::domain_error if @p a contains a value outside
 *         \f$ [-1, 1] \f$. Strong throw guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> acos(const Triplex<IntervalType>& a);

/** @brief Calculate the arctangent of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The arctangent of @p a, in radians
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> atan(const Triplex<IntervalType>& a);

} // namespace sigma

#include "trigonometry.ipp"
