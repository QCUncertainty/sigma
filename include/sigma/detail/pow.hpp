#pragma once
#include <sigma/detail/numeric.hpp>
#include <stdexcept>

/** @file pow.hpp
 *  @brief Shared implementation of pow() for Affine, Taylor, and
 *         TaylorModel.
 */

namespace sigma::detail {

/** @brief Implements x^y, shared by Affine::pow, Taylor::pow, and
 *         TaylorModel::pow.
 *
 *  All three representations compute a power the same way: compose from log
 *  and exp for a strictly one-signed range/bound, fall back to repeated
 *  multiplication for a zero-containing range/bound with a positive integer
 *  exponent (log(a) and log(-a) both require a strictly one-signed domain,
 *  so neither applies there), and throw for everything else that isn't
 *  real-valued. Only two things differ between the three call sites: how to
 *  build a fresh "point" instance of @p X at a given value (a plain
 *  `Affine<T>(v)` vs. a `Taylor<T>`/`TaylorModel<T>` that also needs to carry
 *  the argument's truncation order) -- supplied here as @p make_point rather
 *  than hard-coded -- and which free `log`/`exp` overload applies, which
 *  ordinary argument-dependent lookup resolves for whichever @p X is used.
 *
 *  @tparam X The representation type (Affine<T>, Taylor<T>, or
 *             TaylorModel<T>).
 *  @tparam U The type of the exponent.
 *  @tparam MakePoint A callable taking `X::value_t` and returning an X that
 *                     represents that single value, with the same
 *                     truncation order (if any) as @p a.
 *
 *  @param[in] a The value whose power is computed.
 *  @param[in] power The exponent. Named @p power rather than the more usual
 *                    `exp` so that the unqualified calls to the exponential
 *                    function `exp(...)` below -- deliberately unqualified,
 *                    so that argument-dependent lookup finds whichever
 *                    representation's `exp`/`log` overload matches @p X,
 *                    rather than the one visible at this template's own
 *                    definition point, i.e. wherever `pow.hpp` happens to
 *                    first get included -- aren't shadowed by the parameter.
 *  @param[in] make_point A callable that constructs a new @p X at a given
 *                        value.
 *
 *  @return @p a raised to @p power.
 *
 *  @throw std::domain_error If @p a contains 0 and @p power is negative, or
 *                           if @p a contains non-positive values (and is not
 *                           exactly the point 0) and @p power is not an
 *                           integer. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation fails. Strong throw guarantee.
 */
template<typename X, typename U, typename MakePoint>
X pow_impl(const X& a, const U& power, MakePoint&& make_point) {
    using value_t = typename X::value_t;
    U zero(0);
    if(a.empty()) { return a; }
    if(power == zero) { return make_point(value_t(1.0)); }

    if(a.contains(zero) && power < zero) {
        throw std::domain_error(
          "Can not raise a form containing 0 to a negative power.");
    } else if(a.contains(zero) && power > zero) {
        auto a_range = a.range();
        if(a_range.lower() == value_t(0.0) && a_range.upper() == value_t(0.0)) {
            // a is exactly the point 0: 0^power = 0 for any power > 0.
            return make_point(value_t(0.0));
        }
        // The range/bound genuinely straddles or touches 0 alongside other
        // values, so x^power is real-valued only for a positive integer
        // power (a negative base raised to a fractional power is not a
        // real number); compute via repeated multiplication instead, which
        // operator* already handles correctly (and, for TaylorModel,
        // rigorously -- see Eq. \f$\eqref{eq:tm-mul}\f$) across a
        // zero-containing range.
        if(!is_integer_exponent(power)) {
            throw std::domain_error(
              "Can not raise a form containing 0 to a non-integer power.");
        }
        X result = make_point(value_t(1.0));
        for(long long i = 0; i < static_cast<long long>(power); ++i) {
            result = result * a;
        }
        return result;
    }

    // Handle cases where the range/bound is strictly negative
    if(a.range().upper() < zero) {
        if(!is_integer_exponent(power)) {
            throw std::domain_error(
              "Can not raise a form with negative values to a non-integer "
              "power.");
        }

        auto abs_log             = log(-a);
        auto pow_abs             = exp(value_t(power) * abs_log);
        const bool power_is_even = static_cast<long long>(power) % 2 == 0;
        return power_is_even ? pow_abs : -pow_abs;
    }

    // Handle cases where the range/bound is strictly positive
    auto loga = log(a);
    return exp(loga * power);
}

} // namespace sigma::detail
