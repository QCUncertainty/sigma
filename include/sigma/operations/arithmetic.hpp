#pragma once
#include "sigma/uncertain.hpp"

/** @file arithmetic.hpp
 *  @brief Arithmetic operations for uncertain variables
 */

namespace sigma {

/** @brief Negation Operation
 *
 *  @tparam T The value type of the variable
 *  @param a The variable being negated
 *
 *  @return A copy of @p a, but with the sign of the mean reversed
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> operator-(const Uncertain<T>& a);

/** @brief Addition Operation
 *
 *  @tparam T The value type of the variables
 *  @param lhs The left-hand variable
 *  @param rhs The right-hand variable
 *
 *  @return A variable that is the sum of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> operator+(const Uncertain<T>& lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T> operator+(const Uncertain<T>& lhs, double rhs);
/** @overload */
template<typename T>
Uncertain<T> operator+(double lhs, const Uncertain<T>& rhs);

/** @brief Inplace Addition Operation
 *
 *  @tparam T The value type of the variables
 *  @param lhs The left-hand variable being modified
 *  @param rhs The right-hand variable
 *
 *  @return @p lhs as the sum of itself and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T>& operator+=(Uncertain<T>& lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T>& operator+=(Uncertain<T>& lhs, double rhs);

/** @brief Subtraction Operation
 *
 *  @tparam T The value type of the variables
 *  @param lhs The left-hand variable
 *  @param rhs The right-hand variable
 *
 *  @return A variable that is the difference of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> operator-(const Uncertain<T>& lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T> operator-(const Uncertain<T>& lhs, double rhs);
/** @overload */
template<typename T>
Uncertain<T> operator-(double lhs, const Uncertain<T>& rhs);

/** @brief Inplace Subtraction Operation
 *
 *  @tparam T The value type of the variables
 *  @param lhs The left-hand variable being modified
 *  @param rhs The right-hand variable
 *
 *  @return @p lhs as the difference of itself and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T>& operator-=(Uncertain<T>& lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T>& operator-=(Uncertain<T>& lhs, double rhs);

/** @brief Multiplication Operation
 *
 *  @tparam T The value type of the variables
 *  @param lhs The left-hand variable
 *  @param rhs The right-hand variable
 *
 *  @return A variable that is the product of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> operator*(const Uncertain<T>& lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T> operator*(const Uncertain<T>& lhs, double rhs);
/** @overload */
template<typename T>
Uncertain<T> operator*(double lhs, const Uncertain<T>& rhs);

/** @brief Inplace Multiplication Operation
 *
 *  @tparam T The value type of the variables
 *  @param lhs The left-hand variable being modified
 *  @param rhs The right-hand variable
 *
 *  @return @p lhs as the product of itself and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T>& operator*=(Uncertain<T>& lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T>& operator*=(Uncertain<T>& lhs, double rhs);

/** @brief Division Operation
 *
 *  @tparam T The value type of the variables
 *  @param lhs The left-hand variable
 *  @param rhs The right-hand variable
 *
 *  @return A variable that is the quotient of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> operator/(const Uncertain<T>& lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T> operator/(double lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T> operator/(const Uncertain<T>& lhs, double rhs);

/** @brief Inplace Division Operation
 *
 *  @tparam T The value type of the variables
 *  @param lhs The left-hand variable being modified
 *  @param rhs The right-hand variable
 *
 *  @return @p lhs as the quotient of itself and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T>& operator/=(Uncertain<T>& lhs, const Uncertain<T>& rhs);
/** @overload */
template<typename T>
Uncertain<T>& operator/=(Uncertain<T>& lhs, double rhs);

} // namespace sigma

#include "arithmetic.ipp"
