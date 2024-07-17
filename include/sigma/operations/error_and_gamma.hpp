#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Error function
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The error function value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType erf(const UncertainType& a);

/** @relates Uncertain
 *  @brief Complementary error function
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The complementary error function value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType erfc(const UncertainType& a);

/** @relates Uncertain
 *  @brief Gamma function
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The gamma function value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType tgamma(const UncertainType& a);

/** @relates Uncertain
 *  @brief Gamma function Natural Logarithm
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The natural logarithm of the gamma function value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType lgamma(const UncertainType& a);

} // namespace sigma

#include "error_and_gamma.ipp"
