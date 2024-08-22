#pragma once
#include "sigma/uncertain.hpp"

/** @file trigonometry.hpp
 *  @brief Trigonometric functions for uncertain variables 
 */

namespace sigma {

/** @brief Convert from radians to degrees
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The variable @p a in degrees
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> degrees(const Uncertain<T>& a);

/** @brief Convert from degrees to radians
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The variable @p a in radians
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> radians(const Uncertain<T>& a);

/** @brief Sine of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the sine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> sin(const Uncertain<T>& a);

/** @brief Cosine of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the cosine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> cos(const Uncertain<T>& a);

/** @brief Tangent of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the tangent value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> tan(const Uncertain<T>& a);

/** @brief Arcsine of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the arcsine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> asin(const Uncertain<T>& a);

/** @brief Arccosine of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the arccosine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> acos(const Uncertain<T>& a);

/** @brief Arctangent of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the arctangent value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> atan(const Uncertain<T>& a);

/** @brief Two argument arctangent
 *
 *  @tparam T The value type of the variable
 *  @param y The first variable
 *  @param x The first variable
 *
 *  @return The two argument arctangent value of @p y and @p x
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> atan2(const Uncertain<T>& y, const Uncertain<T>& x);
/** @overload */
template<typename T>
Uncertain<T> atan2(const Uncertain<T>& y, double x);
/** @overload */
template<typename T>
Uncertain<T> atan2(double y, const Uncertain<T>& x);

} // namespace sigma

#include "trigonometry.ipp"
