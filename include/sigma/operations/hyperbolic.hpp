#pragma once
#include "sigma/uncertain.hpp"

/** @file hyperbolic.hpp
 *  @brief Hyperbolic trigonometric functions for uncertain variables 
 */

namespace sigma {

/** @brief Hyperbolic sine of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the hyperbolic sine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> sinh(const Uncertain<T>& a);

/** @brief Hyperbolic cosine of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the hyperbolic cosine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> cosh(const Uncertain<T>& a);

/** @brief Hyperbolic tangent of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the hyperbolic tangent value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> tanh(const Uncertain<T>& a);

/** @brief Hyperbolic arcsine of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the hyperbolic arcsine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> asinh(const Uncertain<T>& a);

/** @brief Hyperbolic arccosine of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the hyperbolic arccosine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> acosh(const Uncertain<T>& a);

/** @brief Hyperbolic arctangent of the variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the hyperbolic arctangent value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> atanh(const Uncertain<T>& a);

} // namespace sigma

#include "hyperbolic.ipp"
