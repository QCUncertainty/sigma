#pragma once
#include "sigma/triplex/detail_/apply_unary.hpp"
#include "sigma/triplex/triplex.hpp"

/** @file basic.hpp
 *  @brief Basic operations for triplex variables
 *
 *  Every function in the triplex operation headers works the same way: the
 *  bounds of the result are those of the corresponding interval function, the
 *  nominal value is the corresponding std function applied to the nominal
 *  value, and a finite nominal value that the bounds miss widens the bounds
 *  (see the Triplex Arithmetic documentation page). The empty triplex is
 *  returned unchanged.
 */

namespace sigma {

/** @brief Absolute value of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> abs(const Triplex<IntervalType>& a);

/** @brief Absolute value of a triplex
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param a The triplex
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
Triplex<IntervalType> fabs(const Triplex<IntervalType>& a);

} // namespace sigma

#include "basic.ipp"
