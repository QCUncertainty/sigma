#pragma once
#include "sigma/interval/general_interval.hpp"

/** @file general_operations/basic.hpp
 *  @brief Basic operations for general interval variables
 */

namespace sigma {

/** @brief Absolute Value of a general interval
 *
 *  As for abs(const Interval<T>&). When @p a straddles zero the lower bound of
 *  the result is the attained zero, and so is closed.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> abs(const GeneralInterval<T>& a);

/** @brief Absolute Value of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> fabs(const GeneralInterval<T>& a);

} // namespace sigma

#include "basic.ipp"
