#pragma once
#include <sigma/affine/affine.hpp>

/** @file basic.hpp
 *  @brief Basic operations for interval variables
 */

namespace sigma {

/** @brief Absolute Value of an affine form
 *
 *  Returns the interval {|x| : x ∈ a}, i.e.:
 *  - [lo, hi]       if lo >= 0
 *  - [-hi, -lo]     if hi <= 0
 *  - [0, max(|lo|, hi)] if lo < 0 < hi
 *
 *  @tparam T The value type of the interval
 *  @param a The interval
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Affine<T> abs(const Affine<T>& a);

/** @brief Absolute Value of an affine form
 *
 *  @tparam T The value type of the interval
 *  @param a The interval
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Affine<T> fabs(const Affine<T>& a);

} // namespace sigma

#include "basic.ipp"
