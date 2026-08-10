#pragma once
#include <sigma/taylor/taylor.hpp>

/** @file basic.hpp
 *  @brief Basic operations for Taylor polynomials
 */

namespace sigma {

/** @brief Absolute value of a Taylor polynomial
 *
 *  @tparam T The value type of the polynomial
 *
 *  Let @f$P@f$ be a Taylor polynomial with bound (see Taylor::bound())
 *  @f$[\mathrm{lo}, \mathrm{hi}]@f$. Mirroring Affine::abs, the absolute
 *  value is defined as:
 *  @f[
 *     |P| = \begin{cases}
 *       P & \text{if } \mathrm{lo} \geq 0 \\
 *       -P & \text{if } \mathrm{hi} \leq 0 \\
 *       [0,\, \max(|\mathrm{lo}|, \mathrm{hi})] & \text{otherwise}
 *    \end{cases}
 *  @f]
 *  As with Affine::abs, the third case is not a polynomial approximation of
 *  @f$|\cdot|@f$ -- @f$|\cdot|@f$ is not differentiable at 0, so it has no
 *  Taylor series to compose_() with there -- but the exact range of
 *  @f$|\cdot|@f$ over @f$[\mathrm{lo}, \mathrm{hi}]@f$, returned via the
 *  interval constructor (Taylor(const interval_t&, Order)).
 *
 *  @param[in] a The Taylor polynomial
 *
 *  @return The absolute value of @p a
 *
 *  @throw std::domain_error If @p a is empty. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation for the new polynomial fails.
 *                        Strong throw guarantee.
 */
template<typename T>
Taylor<T> abs(const Taylor<T>& a);

/** @brief Absolute value of a Taylor polynomial
 *
 *  @tparam T The value type of the polynomial
 *
 *  This function is an alias for abs(const Taylor<T>&). See the documentation
 *  for abs(const Taylor<T>&) for details.
 *
 *  @param[in] a The Taylor polynomial
 *
 *  @return The absolute value of @p a
 *
 *  @throw std::domain_error If @p a is empty. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation for the new polynomial fails.
 *                        Strong throw guarantee.
 */
template<typename T>
Taylor<T> fabs(const Taylor<T>& a);

} // namespace sigma

#include "basic.ipp"
