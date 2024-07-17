#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Absolute Value
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The absolute value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType abs(const UncertainType& a);

/** @relates Uncertain
 *  @brief Nearest integer not less than the given value
 *
 *  Note that this returns an UncertainType value with no dependencies
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The nearest integer not greater than @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType ceil(const UncertainType& a);

/** @relates Uncertain
 *  @brief Nearest integer not greater than the given value
 *
 *  Note that this returns an UncertainType value with no dependencies
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The nearest integer not greater than @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType floor(const UncertainType& a);

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

/** @relates Uncertain
 *  @brief Copy the sign of one value to another
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable whose magnitude is copied
 *  @param b The variable whose sign is copied
 *
 *  @return The copy of @p a with the sign of @p b
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType copysign(const UncertainType& a, const UncertainType& b);
template<typename UncertainType>
UncertainType copysign(const UncertainType& a, double b);
template<typename UncertainType>
double copysign(double a, const UncertainType& b);

/** @relates Uncertain
 *  @brief Remove the fractional part from a variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable whose value is truncated
 *
 *  @return A variable whose value is the truncated value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType trunc(const UncertainType& a);

} // namespace sigma

#include "basic.ipp"
