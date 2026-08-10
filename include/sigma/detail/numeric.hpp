#pragma once
#include <cmath>
#include <type_traits>

/** @file numeric.hpp
 *  @brief Small numeric helpers shared across Sigma's representations.
 */

namespace sigma::detail {

/** @brief True if @p exp has no fractional part.
 *
 *  Always true for an integral @p U. Used by pow() (Affine, Taylor,
 *  TaylorModel) to decide whether a zero-containing or negative-valued
 *  domain can be raised to @p exp at all -- a negative base raised to a
 *  fractional power is not a real number.
 *
 *  @tparam U The type of the exponent.
 *
 *  @param[in] exp The exponent to check.
 *
 *  @return True if @p exp is an integer, false otherwise.
 *
 *  @throw none No throw guarantee.
 */
template<typename U>
bool is_integer_exponent(const U& exp) {
    using clean_u_t = std::decay_t<U>;
    if constexpr(std::is_floating_point_v<clean_u_t>) {
        clean_u_t exp_int;
        return std::modf(exp, &exp_int) == U(0);
    } else {
        return true;
    }
}

} // namespace sigma::detail
