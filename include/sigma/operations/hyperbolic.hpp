#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Hyperbolic sine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the hyperbolic sine value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType sinh(const UncertainType& u);

/** @relates Uncertain
 *  @brief Hyperbolic cosine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the hyperbolic cosine value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType cosh(const UncertainType& u);

/** @relates Uncertain
 *  @brief Hyperbolic tangent of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the hyperbolic tangent value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType tanh(const UncertainType& u);

/** @relates Uncertain
 *  @brief Hyperbolic arcsine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the hyperbolic arcsine value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType asinh(const UncertainType& u);

/** @relates Uncertain
 *  @brief Hyperbolic arccosine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the hyperbolic arccosine value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType acosh(const UncertainType& u);

/** @relates Uncertain
 *  @brief Hyperbolic arctangent of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the hyperbolic arctangent value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType atanh(const UncertainType& u);

} // namespace sigma

#include "hyperbolic.ipp"
