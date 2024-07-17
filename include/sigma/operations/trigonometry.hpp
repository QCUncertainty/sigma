#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Convert from radians to degrees
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The variable @p a in degrees
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType degrees(const UncertainType& a);

/** @relates Uncertain
 *  @brief Convert from degrees to radians
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The variable @p a in radians
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType radians(const UncertainType& a);

/** @relates Uncertain
 *  @brief Sine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the sine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType sin(const UncertainType& a);

/** @relates Uncertain
 *  @brief Cosine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the cosine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType cos(const UncertainType& a);

/** @relates Uncertain
 *  @brief Tangent of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the tangent value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType tan(const UncertainType& a);

/** @relates Uncertain
 *  @brief Arcsine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the arcsine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType asin(const UncertainType& a);

/** @relates Uncertain
 *  @brief Arccosine of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the arccosine value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType acos(const UncertainType& a);

/** @relates Uncertain
 *  @brief Arctangent of the variable
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return A variable that is the arctangent value of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename UncertainType>
UncertainType atan(const UncertainType& a);

/** @relates Uncertain
 *  @brief Two argument arctangent
 *
 *  @tparam UncertainType The type of the variable
 *  @param y The first variable
 *  @param x The first variable
 *
 *  @return The two argument arctangent value of @p y and @p x
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

#include "trigonometry.ipp"
