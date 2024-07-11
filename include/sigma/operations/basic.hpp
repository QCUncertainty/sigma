#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Absolute Value
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return The absolute value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType abs(const UncertainType& u);

/** @relates Uncertain
 *  @brief Floating point module
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The first variable
 *  @param b The first variable
 *
 *  @return A variable that is the floating point remainder of @p a / @p b
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType fmod(const UncertainType& a, const UncertainType& b);
template<typename UncertainType>
UncertainType fmod(const UncertainType& a, double b);
template<typename UncertainType>
UncertainType fmod(double a, const UncertainType& b);

} // namespace sigma