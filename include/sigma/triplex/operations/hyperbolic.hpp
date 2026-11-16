#pragma once
#include "sigma/triplex/detail_/apply_unary.hpp"
#include "sigma/triplex/triplex.hpp"

/** @file hyperbolic.hpp
 *  @brief Hyperbolic operations for triplex variables
 *
 *  The domain of each function is that of the corresponding function of the
 *  interval type.
 */

namespace sigma {

/** @brief Calculate the hyperbolic sine of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The hyperbolic sine of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> sinh(const Triplex<IntervalType>& a);

/** @brief Calculate the hyperbolic cosine of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The hyperbolic cosine of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> cosh(const Triplex<IntervalType>& a);

/** @brief Calculate the hyperbolic tangent of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The hyperbolic tangent of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> tanh(const Triplex<IntervalType>& a);

/** @brief Calculate the inverse hyperbolic sine of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The inverse hyperbolic sine of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> asinh(const Triplex<IntervalType>& a);

/** @brief Calculate the inverse hyperbolic cosine of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The inverse hyperbolic cosine of @p a
 *
 *  @throw std::domain_error if @p a contains a value below one. Strong throw
 *         guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> acosh(const Triplex<IntervalType>& a);

/** @brief Calculate the inverse hyperbolic tangent of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The inverse hyperbolic tangent of @p a
 *
 *  @throw std::domain_error if @p a contains a value outside
 *         \f$ (-1, 1) \f$. Strong throw guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> atanh(const Triplex<IntervalType>& a);

} // namespace sigma

#include "hyperbolic.ipp"
