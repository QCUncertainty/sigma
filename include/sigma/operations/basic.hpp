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

} // namespace sigma