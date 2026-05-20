#pragma once
#include "sigma/interval/interval.hpp"

/** @file basic.hpp
 *  @brief Basic operations for interval variables
 */

namespace sigma {

/** @brief Absolute Value of an interval
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
Interval<T> abs(const Interval<T>& a);

/** @brief Absolute Value of an interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> fabs(const Interval<T>& a);

} // namespace sigma

#include "basic.ipp"
