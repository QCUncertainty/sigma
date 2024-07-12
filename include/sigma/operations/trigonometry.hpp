#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Convert from radians to degrees
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return The variable @p u in degrees
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType degrees(const UncertainType& u);

/** @relates Uncertain
 *  @brief Convert from degrees to radians
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return The variable @p u in radians
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType radians(const UncertainType& u);

/** @relates Uncertain
 *  @brief Sine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the sine value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType sin(const UncertainType& u);

/** @relates Uncertain
 *  @brief Cosine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the cosine value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType cos(const UncertainType& u);

/** @relates Uncertain
 *  @brief Tangent of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the tangent value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType tan(const UncertainType& u);

/** @relates Uncertain
 *  @brief Arcsine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the arcsine value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType asin(const UncertainType& u);

/** @relates Uncertain
 *  @brief Arccosine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the arccosine value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType acos(const UncertainType& u);

/** @relates Uncertain
 *  @brief Arctangent of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param u The variable
 *
 *  @return A variable that is the arctangent value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType atan(const UncertainType& u);

/** @relates Uncertain
 *  @brief Two argument arctangent
 *
 *  @tparam UncertainType The type of the variable
 *  @param y The first variable
 *  @param x The first variable
 *
 *  @return A variable that is the arctangent value of @p u
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType atan2(const UncertainType& y, const UncertainType& x);
template<typename UncertainType>
UncertainType atan2(const UncertainType& y, double x);
template<typename UncertainType>
UncertainType atan2(double y, const UncertainType& x);

} // namespace sigma