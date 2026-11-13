#pragma once
#include <type_traits>

/** @file traits.hpp
 *  @brief Type traits for the interval types
 */

namespace sigma {

// Forward declaration
template<typename ValueType>
class GeneralInterval;

/// Is @p T a GeneralInterval? False for everything else.
template<typename T>
struct is_general_interval : std::false_type {};

/// Specialization of is_general_interval for GeneralInterval
template<typename T>
struct is_general_interval<GeneralInterval<T>> : std::true_type {};

/// Convenience variable for the value of is_general_interval
template<typename T>
constexpr bool is_general_interval_v = is_general_interval<T>::value;

} // namespace sigma
