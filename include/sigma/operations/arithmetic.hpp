#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Negation Operation
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable being negated
 *
 *  @return A copy of @p u, but with the sign of the mean reversed
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType operator-(const UncertainType& u);

/** @relates Uncertain
 *  @brief Addition Operation
 *
 *  @tparam UncertainType The type of the variables
 *  @param lhs The left-hand variable
 *  @param rhs The right-hand variable
 *
 *  @return A variable that is the sum of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType operator+(const UncertainType& lhs, const UncertainType& rhs);

/** @relates Uncertain
 *  @brief Inplace Addition Operation
 *
 *  @tparam UncertainType The type of the variables
 *  @param lhs The left-hand variable being modified
 *  @param rhs The right-hand variable
 *
 *  @return @p lhs as the sum of itself and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType& operator+=(UncertainType& lhs, const UncertainType& rhs);

/** @relates Uncertain
 *  @brief Subtraction Operation
 *
 *  @tparam UncertainType The type of the variables
 *  @param lhs The left-hand variable
 *  @param rhs The right-hand variable
 *
 *  @return A variable that is the difference of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType operator-(const UncertainType& lhs, const UncertainType& rhs);

/** @relates Uncertain
 *  @brief Inplace Subtraction Operation
 *
 *  @tparam UncertainType The type of the variables
 *  @param lhs The left-hand variable being modified
 *  @param rhs The right-hand variable
 *
 *  @return @p lhs as the difference of itself and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType& operator-=(UncertainType& lhs, const UncertainType& rhs);

/** @relates Uncertain
 *  @brief Multiplication Operation
 *
 *  @tparam UncertainType The type of the variables
 *  @param lhs The left-hand variable
 *  @param rhs The right-hand variable
 *
 *  @return A variable that is the product of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType operator*(const UncertainType& lhs, const UncertainType& rhs);
template<typename UncertainType>
UncertainType operator*(const UncertainType& lhs, double rhs);
template<typename UncertainType>
UncertainType operator*(double lhs, const UncertainType& rhs);

/** @relates Uncertain
 *  @brief Inplace Multiplication Operation
 *
 *  @tparam UncertainType The type of the variables
 *  @param lhs The left-hand variable being modified
 *  @param rhs The right-hand variable
 *
 *  @return @p lhs as the product of itself and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType& operator*=(UncertainType& lhs, const UncertainType& rhs);
template<typename UncertainType>
UncertainType& operator*=(UncertainType& lhs, double rhs);

/** @relates Uncertain
 *  @brief Division Operation
 *
 *  @tparam UncertainType The type of the variables
 *  @param lhs The left-hand variable
 *  @param rhs The right-hand variable
 *
 *  @return A variable that is the quotient of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType operator/(const UncertainType& lhs, const UncertainType& rhs);
template<typename UncertainType>
UncertainType operator/(double lhs, const UncertainType& rhs);
template<typename UncertainType>
UncertainType operator/(const UncertainType& lhs, double rhs);

/** @relates Uncertain
 *  @brief Inplace Division Operation
 *
 *  @tparam UncertainType The type of the variables
 *  @param lhs The left-hand variable being modified
 *  @param rhs The right-hand variable
 *
 *  @return @p lhs as the quotient of itself and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType& operator/=(UncertainType& lhs, const UncertainType& rhs);
template<typename UncertainType>
UncertainType& operator/=(UncertainType& lhs, double rhs);

} // namespace sigma