#pragma once
#include <sigma/taylor/taylor.hpp>

/** @file exponents.hpp
 *  @brief Exponent operations for Taylor polynomials
 *
 *  Each function here composes the outer function's Taylor series about
 *  constant() with *this via Taylor::compose_ (see docs/taylor.md's
 *  "Elementary functions" section). Because Taylor alone carries no
 *  remainder, the composition is exact only for a polynomial argument -- it
 *  makes no claim to enclose the true value of the elementary function, just
 *  as Taylor's own arithmetic makes no such claim (see taylor.hpp). The
 *  rigorous version of each of these lives on TaylorModel, which pairs the
 *  same composition with a Lagrange remainder bound.
 */

namespace sigma {

/** @brief Calculate the square root of a Taylor polynomial
 *
 *  @tparam T The value type of the polynomial
 *
 *  Composes the binomial series of sqrt about a.constant() with @p a.
 *
 *  @param[in] a The Taylor polynomial whose square root is computed
 *
 *  @return A Taylor polynomial approximating the square root of @p a
 *
 *  @throw std::domain_error If @p a is empty or bound(a) contains
 *                           non-positive values. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation fails. Strong throw guarantee.
 */
template<typename T>
Taylor<T> sqrt(const Taylor<T>& a);

/** @brief Calculate the exponential of a Taylor polynomial
 *
 *  @tparam T The value type of the polynomial
 *
 *  Composes the Taylor series of exp about a.constant() with @p a.
 *
 *  @param[in] a The Taylor polynomial whose exponential is computed
 *
 *  @return A Taylor polynomial approximating the exponential of @p a
 *
 *  @throw std::domain_error If @p a is empty. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation fails. Strong throw guarantee.
 */
template<typename T>
Taylor<T> exp(const Taylor<T>& a);

/** @brief Calculate the natural logarithm of a Taylor polynomial
 *
 *  @tparam T The value type of the polynomial
 *
 *  Composes the Taylor series of log about a.constant() with @p a.
 *
 *  @param[in] a The Taylor polynomial whose natural logarithm is computed
 *
 *  @return A Taylor polynomial approximating the natural logarithm of @p a
 *
 *  @throw std::domain_error If @p a is empty or bound(a) contains
 *                           non-positive values. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation fails. Strong throw guarantee.
 */
template<typename T>
Taylor<T> log(const Taylor<T>& a);

/** @brief Calculate the power of a Taylor polynomial
 *
 *  @tparam T The value type of the polynomial
 *  @tparam U The type of the exponent
 *
 *  For a strictly one-signed bound this method takes advantage of the fact
 *  that:
 *  @f[
 *  x^y = e^{y \log(x)}
 *  @f]
 *  mirroring Affine::pow. If bound(a) straddles or touches 0 (and is not
 *  exactly the point 0, where @f$0^y = 0@f$ for any @f$y > 0@f$), neither
 *  @f$\log@f$ branch is valid, so @p exp must be a positive integer, and the
 *  result is computed by repeated multiplication instead.
 *
 *  @param[in] a The Taylor polynomial whose power is computed
 *  @param[in] exp The exponent
 *
 *  @return A Taylor polynomial approximating @p a raised to @p exp
 *
 *  @throw std::domain_error If bound(a) contains 0 and @p exp is negative, or
 *                           if bound(a) contains non-positive values (and is
 *                           not exactly the point 0) and @p exp is not an
 *                           integer. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation fails. Strong throw guarantee.
 */
template<typename T, typename U>
Taylor<T> pow(const Taylor<T>& a, const U& exp);

} // namespace sigma

#include "exponents.ipp"
