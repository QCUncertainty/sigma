#pragma once
#include <sigma/affine/affine.hpp>

/** @file exponents.hpp
 *  @brief Exponent operations for affine arithmetic
 *
 *  Formula are from affapy documentation:
 *  https://affapy.readthedocs.io/en/latest/aa.html
 */

namespace sigma {

/** @brief Calculate the square root of an affine form
 *
 *  @tparam T The value type of the affine form
 *
 *  The square root of an affine form is obtained by applying an affine
 *  transformation. We use the following affine transformation:
 *  @f[
 *  \begin{align}
 *  \alpha &= \frac{1}{\sqrt{a} + \sqrt{b}}\\
 *  \zeta &= \frac{\sqrt{a} + \sqrt{b}}{8} +
 *           \frac{\sqrt{ab}}{2(\sqrt{a} + \sqrt{b})}\\
 *  \delta &= \frac{(\sqrt{b} - \sqrt{a})^2}{8(\sqrt{a} + \sqrt{b})}
 *  \end{align}
 *  @f]
 *  where @f$a@f$ and @f$b@f$ are the lower and upper bounds of the range of the
 *  affine form, respectively.
 *
 *  @param[in] a The affine form whose square root is computed
 *
 *  @return An affine form whose bounds are the square root of @p a
 *
 *  @throw std::bad_alloc if
 */
template<typename T>
Affine<T> sqrt(const Affine<T>& a);

/** @brief Calculate the exponential of an affine form
 *
 *  @tparam T The value type of the affine form
 *
 *  The exponential of an affine form is obtained by applying an affine
 *  transformation. We use the following affine transformation:
 *  @f[
 *  \begin{align}
 *  \alpha &= \frac{e^b - e^a}{b - a} \\
 *  \zeta &= \frac{e^a - \alpha*a + \alpha*(1-\log(\alpha))}{2} \\
 *  \delta &= \frac{e^a - \alpha*a - \alpha*(1-\log(\alpha))}{2}
 *  \end{align}
 *  @f]
 *  where @f$a@f$ and @f$b@f$ are the lower and upper bounds of the range of the
 *  affine form, respectively. N.b. the affapy formula is wrong.
 *
 *  @param[in] a The affine form whose exponential is computed
 *
 *  @return An affine form whose bounds are the exponential of @p a
 *
 *  @throw std::bad_alloc if memory allocation fails
 */
template<typename T>
Affine<T> exp(const Affine<T>& a);

/** @brief Calculate the natural logarithm of an affine form
 *
 *  @tparam T The value type of the affine form
 *
 *  The natural logarithm of an affine form is obtained by applying an affine
 *  transformation. We use the following affine transformation:
 *  @f[
 *  \begin{align}
 *  \alpha &= \frac{\log(b) - \log(a)}{b - a} \\
 *  \zeta &= \frac{\log(x_s) + y_s}{2} - \alpha * x_s \\
 *  \delta &= \frac{\log(x_s) - y_s}{2}
 *  \end{align}
 *  @f]
 *  where @f$a@f$ and @f$b@f$ are the lower and upper bounds of the range of the
 *  affine form, respectively, and:
 *  @f[
 *  \begin{align}
 *  x_s &= \frac{1}{\alpha} \\
 *  y_s &= \alpha * (x_s - a) + \log(a)
 *  \end{align}
 *  @f]
 *  Note that the affapy formula for zeta is rendered wrong.
 *
 *  @param[in] a The affine form whose natural logarithm is computed
 *
 *  @return An affine form whose bounds are the natural logarithm of @p a
 *
 *  @throw std::domain_error if the range of @p a contains non-positive values
 */
template<typename T>
Affine<T> log(const Affine<T>& a);

/** @brief Calculate the power of an affine form
 *
 *  @tparam T The value type of the affine form
 *  @tparam U The type of the exponent
 *
 *  This method takes advantage of the fact that:
 *  @f[
 *  x^y = e^{y \log(x)}
 *  @f]
 *
 *  @param[in] a The affine form whose power is computed
 *  @param[in] exp The exponent
 *
 *  @return An affine form whose bounds are the power of @p a
 *
 *  @throw std::domain_error if the range of @p a contains non-positive values
 *                           and the exponent is not an integer. Strong throw
 *                           guarantee.
 *  @throw std::domain_error if the range of @p a contains 0 and the exponent is
 *                           negative. Strong throw guarantee.
 *  @throw std::bad_alloc if memory allocation fails. Strong throw guarantee.
 */
template<typename T, typename U>
Affine<T> pow(const Affine<T>& a, const U& exp);

} // namespace sigma

#include "exponents.ipp"
