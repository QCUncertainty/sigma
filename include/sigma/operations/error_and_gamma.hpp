#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Error function
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return The error function value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType erf(const UncertainType& u);

/** @relates Uncertain
 *  @brief Complementary error function
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return The complementary error function value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType erfc(const UncertainType& u);

/** @relates Uncertain
 *  @brief Gamma function
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return The gamma function value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType tgamma(const UncertainType& u);

/** @relates Uncertain
 *  @brief Gamma function Natural Logarithm
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return The natural logarithm of the gamma function value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType lgamma(const UncertainType& u);

} // namespace sigma