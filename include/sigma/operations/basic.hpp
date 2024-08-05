#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Absolute Value
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> abs(const Uncertain<T>& a);

/** @relates Uncertain
 *  @brief The Square of the Absolute Value
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The square of the absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> abs2(const Uncertain<T>& a);

/** @relates Uncertain
 *  @brief Nearest integer not less than the given value
 *
 *  Note that this returns an Uncertain<T> value with no dependencies
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The nearest integer not greater than @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> ceil(const Uncertain<T>& a);

/** @relates Uncertain
 *  @brief Nearest integer not greater than the given value
 *
 *  Note that this returns an Uncertain<T> value with no dependencies
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The nearest integer not greater than @p u
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> floor(const Uncertain<T>& a);

/** @relates Uncertain
 *  @brief Floating point module
 *
 *  @tparam T The value type of the variable
 *  @param a The first variable
 *  @param b The first variable
 *
 *  @return A variable that is the floating point remainder of @p a / @p b
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> fmod(const Uncertain<T>& a, const Uncertain<T>& b);
template<typename T>
Uncertain<T> fmod(const Uncertain<T>& a, double b);
template<typename T>
Uncertain<T> fmod(double a, const Uncertain<T>& b);

/** @relates Uncertain
 *  @brief Copy the sign of one value to another
 *
 *  @tparam T The value type of the variable
 *  @param a The variable whose magnitude is copied
 *  @param b The variable whose sign is copied
 *
 *  @return The copy of @p a with the sign of @p b
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> copysign(const Uncertain<T>& a, const Uncertain<T>& b);
template<typename T>
Uncertain<T> copysign(const Uncertain<T>& a, double b);
template<typename T>
double copysign(double a, const Uncertain<T>& b);

/** @relates Uncertain
 *  @brief Remove the fractional part from a variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable whose value is truncated
 *
 *  @return A variable whose value is the truncated value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> trunc(const Uncertain<T>& a);

} // namespace sigma

#include "basic.ipp"
