#pragma once
#include "sigma/uncertain.hpp"

/** @file exponents.hpp
 *  @brief Exponent operations for uncertain variables
 */

namespace sigma {

/** @brief Exponentiation of a variable
 *
 *  @tparam T The value type of the variable
 *  @param a The base variable
 *  @param exp The exponent to raise the base by
 *
 *  @return A variable that is @p a raise to the power @p exp
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> pow(const Uncertain<T>& a, double exp);

/** @brief Exponentiation of a variable by an uncertain variable
 *
 *  @tparam T The value type of the variables
 *  @param a The base variable
 *  @param exp The uncertain exponent to raise the base by
 *
 *  @return A variable that is @p a raise to the power @p exp
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> pow(const Uncertain<T>& a, const Uncertain<T>& exp);

/** @brief Calculate the square root of an uncertain variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable whose root is computed
 *
 *  @return A variable whose value is the square root of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> sqrt(const Uncertain<T>& a);

/** @brief Calculate the Euler's number raised to the power of an uncertain
 *         variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable that is the exponent
 *
 *  @return A variable whose value Euler's number raised by the mean of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> exp(const Uncertain<T>& a);

/** @brief Calculate the natural logarithm of a variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable whose logarithm is determined
 *
 *  @return A variable whose value is the natural logarithm of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> log(const Uncertain<T>& a);

/** @brief Calculate the base 10 logarithm of a variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable whose logarithm is determined
 *
 *  @return A variable whose value is the base 10 logarithm of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> log10(const Uncertain<T>& a);

/** @brief Calculate the square root of the sum of squared arguments
 *
 *  @tparam T The value type of the variable
 *  @param a The first variable
 *  @param b The second variable
 *
 *  @return A variable whose value is the base 10 logarithm of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> hypot(const Uncertain<T>& a, const Uncertain<T>& b);
/** @overload */
template<typename T>
Uncertain<T> hypot(const Uncertain<T>& a, double b);
/** @overload */
template<typename T>
Uncertain<T> hypot(double a, const Uncertain<T>& b);

} // namespace sigma

#include "exponents.ipp"
