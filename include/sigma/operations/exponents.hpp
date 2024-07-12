#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Exponentiation of a variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The base variable
 *  @param exp The exponent to raise the base by
 *
 *  @return A variable that is @p u raise to the power @p exp
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType pow(const UncertainType& u, double exp);

/** @relates Uncertain
 *  @brief Exponentiation of a variable by an uncertain variable
 *
 *  @tparam UncertainType The type of the variables
 *  @param u The base variable
 *  @param exp The uncertain exponent to raise the base by
 *
 *  @return A variable that is @p u raise to the power @p exp
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType pow(const UncertainType& u, const UncertainType& exp);

/** @relates Uncertain
 *  @brief Calculate the square root of an uncertain variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable whose root is computed
 *
 *  @return A variable whose value is the square root of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType sqrt(const UncertainType& u);

/** @relates Uncertain
 *  @brief Calculate the Euler's number raised to the power of an uncertain
 *         variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable that is the exponent
 *
 *  @return A variable whose value Euler's number raised by the mean of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType exp(const UncertainType& u);

/** @relates Uncertain
 *  @brief Calculate the natural logarithm of a variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable whose logarithm is determined
 *
 *  @return A variable whose value is the natural logarithm of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType log(const UncertainType& u);

/** @relates Uncertain
 *  @brief Calculate the base 10 logarithm of a variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable whose logarithm is determined
 *
 *  @return A variable whose value is the base 10 logarithm of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType log10(const UncertainType& u);

template<typename UncertainType>
UncertainType hypot(const UncertainType& a, const UncertainType& b);
template<typename UncertainType>
UncertainType hypot(const UncertainType& a, double b);
template<typename UncertainType>
UncertainType hypot(double a, const UncertainType& b);

} // namespace sigma