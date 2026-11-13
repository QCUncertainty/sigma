#pragma once
#include "sigma/interval/general_interval.hpp"

/** @file general_operations/trigonometry.hpp
 *  @brief Trigonometric operations for general interval variables
 *
 *  These follow the Interval overloads, and report the openness of each bound
 *  of the result: it carries over from the argument for a monotonic function,
 *  and a bound that comes from an interior extremum is closed.
 */

namespace sigma {

/** @brief Calculate the sine of a general interval
 *
 *  The bounds of the result are always closed, since either may come from a
 *  peak interior to @p a.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose sine is computed
 *
 *  @return An interval enclosing the sine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> sin(const GeneralInterval<T>& a);

/** @brief Calculate the cosine of a general interval
 *
 *  The bounds of the result are always closed, since either may come from a
 *  peak interior to @p a.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose cosine is computed
 *
 *  @return An interval enclosing the cosine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> cos(const GeneralInterval<T>& a);

/** @brief Calculate the tangent of a general interval
 *
 *  Openness cannot rescue an argument that reaches a pole: the poles are not
 *  representable, so a bound within rounding distance of one cannot be shown
 *  to exclude it.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose tangent is computed
 *
 *  @return An interval enclosing the tangent of every value in @p a
 *
 *  @throw std::domain_error if @p a contains, or cannot be shown to exclude,
 *         an odd multiple of \f$ \pi/2 \f$. Strong throw guarantee.
 */
template<typename T>
GeneralInterval<T> tan(const GeneralInterval<T>& a);

/** @brief Calculate the arcsine of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose arcsine is computed
 *
 *  @return An interval enclosing the arcsine of every value in @p a
 *
 *  @throw std::domain_error if @p a contains a value outside
 *         \f$ [-1, 1] \f$. Strong throw guarantee.
 */
template<typename T>
GeneralInterval<T> asin(const GeneralInterval<T>& a);

/** @brief Calculate the arccosine of a general interval
 *
 *  Arccosine is decreasing, so the openness of the bounds exchanges places.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose arccosine is computed
 *
 *  @return An interval enclosing the arccosine of every value in @p a
 *
 *  @throw std::domain_error if @p a contains a value outside
 *         \f$ [-1, 1] \f$. Strong throw guarantee.
 */
template<typename T>
GeneralInterval<T> acos(const GeneralInterval<T>& a);

/** @brief Calculate the arctangent of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose arctangent is computed
 *
 *  @return An interval enclosing the arctangent of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> atan(const GeneralInterval<T>& a);

} // namespace sigma

#include "trigonometry.ipp"
