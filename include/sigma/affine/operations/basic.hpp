#pragma once
#include <sigma/affine/affine.hpp>

/** @file basic.hpp
 *  @brief Basic operations for interval variables
 */

namespace sigma {

/** @brief Absolute Value of an affine form
 *
 *  @tparam T The value type of the interval
 *
 *  Let @f$\hat{x}@f$ be an affine form. The absolute value of the affine form
 *  is defined as:
 *  @f[
 *     |\hat{x}| = \begin{cases}
 *       \hat{x} & \text{if } \hat{x}_0 > 0 \\
 *       -\hat{x}& \text{if } \hat{x}_0 < 0 \\
 *       \frac{x_0}{2} + \sum_{i=1}^n \frac{x_i}{2} \epsilon_i &
 * \text{otherwise}
 *    \end{cases}
 *  @f]
 *
 *  @param[in] a The interval
 *
 *  @return The absolute value of @p a
 *
 *  @throw std::bad_alloc If memory allocation for the new affine form fails.
 *                       Strong throw guarantee.
 */
template<typename T>
Affine<T> abs(const Affine<T>& a);

/** @brief Absolute Value of an affine form
 *
 *  @tparam T The value type of the interval
 *
 *  This function is an alias for abs(const Affine<T>&). See the documentation
 *  for abs(const Affine<T>&) for details on how the absolute value of an affine
 * form is defined and computed.
 *
 *  @param[in] a The interval
 *
 *  @return The absolute value of @p a
 *
 *  @throw std::bad_alloc If memory allocation for the new affine form fails.
 *                       Strong throw guarantee.
 */
template<typename T>
Affine<T> fabs(const Affine<T>& a);

} // namespace sigma

#include "basic.ipp"
