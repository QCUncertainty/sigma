#pragma once
#include "sigma/uncertain.hpp"

/** @file error_and_gamma.hpp
 *  @brief Error and gamma functions for uncertain variables 
 */

namespace sigma {

/** @brief Error function
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The error function value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> erf(const Uncertain<T>& a);

/** @brief Complementary error function
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The complementary error function value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> erfc(const Uncertain<T>& a);

/** @brief Gamma function
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The gamma function value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> tgamma(const Uncertain<T>& a);

/** @brief Gamma function Natural Logarithm
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The natural logarithm of the gamma function value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> lgamma(const Uncertain<T>& a);

} // namespace sigma

#include "error_and_gamma.ipp"
