#pragma once
#include <sigma/affine/affine.hpp>

/** @file exponents.hpp
 *  @brief Exponent operations for affine arithmetic
 */

namespace sigma {

/** @brief Calculate the square root of an affine form
 *
 *  @tparam T The value type of the affine form
 *  @param a The affine form whose square root is computed
 *
 *  @return An affine form whose bounds are the square root of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Affine<T> sqrt(const Affine<T>& a);

} // namespace sigma
