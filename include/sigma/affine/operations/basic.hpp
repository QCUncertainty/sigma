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
 *  Let @f$\hat{x}@f$ be an affine form with range @f$[\mathrm{lo},
 *  \mathrm{hi}]@f$. The absolute value of the affine form is defined as:
 *  @f[
 *     |\hat{x}| = \begin{cases}
 *       \hat{x} & \text{if } \mathrm{lo} \geq 0 \\
 *       -\hat{x}& \text{if } \mathrm{hi} \leq 0 \\
 *       [0,\, \max(|\mathrm{lo}|, \mathrm{hi})] & \text{otherwise}
 *    \end{cases}
 *  @f]
 *  The first two cases are exact, since @f$|\cdot|@f$ is linear (the identity
 *  or its negation) once its sign is fixed. The third case is not built from
 *  an affine transformation of @f$\hat{x}@f$ at all: @f$|\cdot|@f$ is not
 *  differentiable at 0, so no such transformation both agrees with it locally
 *  and stays a valid enclosure, and picking one anyway (e.g. naively halving
 *  the center and every error term) produces a result that does not actually
 *  contain every value @f$|\hat{x}|@f$ can take. @f$[0, \max(|\mathrm{lo}|,
 *  \mathrm{hi})]@f$ is instead the exact range of @f$|\cdot|@f$ over
 *  @f$[\mathrm{lo}, \mathrm{hi}]@f$, returned as a fresh single-error-term
 *  affine form via the interval constructor.
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
