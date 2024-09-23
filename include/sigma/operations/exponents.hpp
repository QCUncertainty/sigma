#pragma once
#include "sigma/uncertain.hpp"

/** @file exponents.hpp
 *  @brief Exponent operations for uncertain variables
 */

namespace sigma {

/** @brief Exponentiation of a variable
 *
 *  @tparam T The value type of the variable
 *  @tparam U The numeric type of the exponent
 *  @param a The base variable
 *  @param exp The exponent to raise the base by
 *
 *  @return A variable that is @p a raise to the power @p exp
 *
 *  @throw none No throw guarantee
 */
template<typename T, typename U>
Uncertain<T> pow(const Uncertain<T>& a, const U& exp);

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

/** @brief Calculate the cube root of an uncertain variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable whose root is computed
 *
 *  @return A variable whose value is the cube root of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> cbrt(const Uncertain<T>& a);

/** @brief Calculate the Euler's number raised to the power of an uncertain
 *         variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable that is the exponent
 *
 *  @return A variable whose value is Euler's number raised by the mean of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> exp(const Uncertain<T>& a);

/** @brief Calculate 2 raised to the power of an uncertain variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable that is the exponent
 *
 *  @return A variable whose value is 2 raised by the mean of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> exp2(const Uncertain<T>& a);

/** @brief Calculate the Euler's number raised to the power of an uncertain
 *         variable, then subtract 1.
 *
 *  @tparam T The value type of the variable
 *  @param a The variable that is the exponent
 *
 *  @return A variable whose value is Euler's number raised by the mean of @p a,
 *          then subtract 1.
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> expm1(const Uncertain<T>& a);

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

/** @brief Calculate the base 2 logarithm of a variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable whose logarithm is determined
 *
 *  @return A variable whose value is the base 2 logarithm of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> log2(const Uncertain<T>& a);

/** @brief Calculate the natural logarithm of one plus a variable
 *
 *  @tparam T The value type of the variable
 *  @param a The variable whose logarithm is determined
 *
 *  @return A variable whose value is the natural logarithm of @p a + 1
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> log1p(const Uncertain<T>& a);

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

/** @brief Calculate the square root of the sum of squared arguments
 *
 *  @tparam T The value type of the variable
 *  @tparam U The numeric type of @p b
 *  @param a The first variable
 *  @param b The second variable
 *
 *  @return A variable whose value is the base 10 logarithm of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T, typename U>
Uncertain<T> hypot(const Uncertain<T>& a, const U& b);

/** @brief Calculate the square root of the sum of squared arguments
 *
 *  @tparam T The value type of the variable
 *  @tparam U The numeric type of @p a
 *  @param a The first variable
 *  @param b The second variable
 *
 *  @return A variable whose value is the base 10 logarithm of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T, typename U>
Uncertain<T> hypot(const U& a, const Uncertain<T>& b);

} // namespace sigma

#include "exponents.ipp"
