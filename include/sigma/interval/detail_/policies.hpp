#pragma once
#include <boost/numeric/interval.hpp>
#include <cmath>
#include <limits>

/** @file policies.hpp
 *  @brief Rounding and checking policies used by the boost interval sigma
 *         wraps.
 *
 *  Boost's default policies only round the arithmetic operators; the
 *  transcendental functions in boost/numeric/interval/transc.hpp additionally
 *  require the rounding policy to supply exp_down(), log_up(), etc. Supplying
 *  those, in a way that actually encloses the result, is what this file is
 *  for.
 */

namespace sigma::detail_ {

/** @brief Outward rounding for the transcendental functions.
 *
 *  Boost ships three transcendental rounding policies, and all three obtain a
 *  directed rounding by switching the FPU's rounding mode around a call to the
 *  platform's libm (rounded_transc_std, rounded_transc_opp) or by trusting
 *  libm to be exact (rounded_transc_exact). None of them is safe here, for two
 *  separate reasons:
 *
 *  1. libm is not required to honor the rounding mode for the transcendental
 *     functions, and a libm that ignores it makes f_down(x) and f_up(x) return
 *     the same value. The interval then claims a degenerate enclosure for an
 *     inexact result, i.e., that log is exact -- which is not merely loose, it
 *     excludes the true value.
 *
 *  2. Worse, a libm that *partially* honors the mode is actively harmful. Its
 *     internal steps each round in the requested direction, so the accumulated
 *     error can carry the final result past the correctly rounded one and out
 *     the other side. Measured on macOS, atanh() evaluated in downward mode
 *     returns a value two ulps *above* the correctly rounded result, so the
 *     "lower bound" is not a lower bound at all.
 *
 *  This policy therefore never asks libm to round: every function is evaluated
 *  in to-nearest mode, which is the mode libm implementations are actually
 *  written and tested for, and the result is then widened outward by
 *  k_error_ulps_ ulps to cover libm's error. The arithmetic operators and
 *  sqrt() keep boost's hardware-directed rounding (rounded_arith_opp), which
 *  IEEE-754 does require to be correctly rounded, so they stay exactly as
 *  tight as they were.
 *
 *  @tparam T The floating point type being rounded.
 */
template<typename T>
struct rounded_transc_guarded
  : boost::numeric::interval_lib::rounded_arith_opp<T> {
private:
    /** @brief How many ulps of libm error each bound is widened by.
     *
     *  The C library targets an error under one ulp but does not achieve it
     *  uniformly. Measured against a 100 digit reference, macOS's libm is off
     *  by as much as 2.4 ulps (tan) and 1.7 ulps (atanh) in to-nearest mode,
     *  and glibc documents errors of two to three ulps for several of these
     *  functions. Four ulps covers the observed worst cases with headroom,
     *  at a cost in relative width of order 1e-15 -- nothing next to the
     *  O(h) overestimation interval arithmetic incurs anyway.
     */
    static constexpr int k_error_ulps_ = 4;

    /// @p y moved @p k_error_ulps_ ulps toward negative infinity
    static T down_(T y) {
        constexpr auto inf = std::numeric_limits<T>::infinity();
        for(int i = 0; i < k_error_ulps_; ++i) { y = std::nextafter(y, -inf); }
        return y;
    }

    /// @p y moved @p k_error_ulps_ ulps toward positive infinity
    static T up_(T y) {
        constexpr auto inf = std::numeric_limits<T>::infinity();
        for(int i = 0; i < k_error_ulps_; ++i) { y = std::nextafter(y, inf); }
        return y;
    }

public:
/// Generates the pair of bounds for one function, following the same pattern
/// boost's own BOOST_NUMERIC_INTERVAL_new_func macro uses. The rounding mode
/// is restored to upward afterwards because that is the invariant
/// rounded_arith_opp maintains for the arithmetic operators.
#define SIGMA_ROUNDED_TRANSC(f, using_decl) \
    T f##_down(const T& x) {                \
        using_decl(f);                      \
        this->to_nearest();                 \
        T y = this->force_rounding(f(x));   \
        this->upward();                     \
        return down_(y);                    \
    }                                       \
    T f##_up(const T& x) {                  \
        using_decl(f);                      \
        this->to_nearest();                 \
        T y = this->force_rounding(f(x));   \
        this->upward();                     \
        return up_(y);                      \
    }

    SIGMA_ROUNDED_TRANSC(exp, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(log, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(sin, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(cos, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(tan, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(asin, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(acos, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(atan, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(sinh, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(cosh, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(tanh, BOOST_NUMERIC_INTERVAL_using_math)
    SIGMA_ROUNDED_TRANSC(asinh, BOOST_NUMERIC_INTERVAL_using_ahyp)
    SIGMA_ROUNDED_TRANSC(acosh, BOOST_NUMERIC_INTERVAL_using_ahyp)
    SIGMA_ROUNDED_TRANSC(atanh, BOOST_NUMERIC_INTERVAL_using_ahyp)

#undef SIGMA_ROUNDED_TRANSC
};

/** @brief The policies the boost interval underlying sigma::Interval uses.
 *
 *  Two deviations from boost's defaults:
 *
 *  - The rounding policy is rounded_transc_guarded, so the transcendental
 *    functions of boost/numeric/interval/transc.hpp are available at all, and
 *    outwardly rounded when they are used. It extends, rather than replaces,
 *    the rounded_arith_opp arithmetic boost defaults to.
 *
 *  - The checking policy is checking_base rather than boost's default
 *    checking_strict. checking_strict inherits checking_no_empty, whose
 *    empty_lower() *throws*, and boost's log()/sqrt()/asin() signal a domain
 *    violation by returning an empty interval. checking_base lets that empty
 *    interval come back as a value, which sigma::Interval then maps onto its
 *    own representation of the empty set.
 *
 *  @tparam T The floating point type of the bounds.
 */
template<typename T>
using interval_policies_t = boost::numeric::interval_lib::policies<
  boost::numeric::interval_lib::save_state<rounded_transc_guarded<T>>,
  boost::numeric::interval_lib::checking_base<T>>;

/// The boost interval type sigma::Interval wraps
template<typename T>
using boost_interval_t = boost::numeric::interval<T, interval_policies_t<T>>;

} // namespace sigma::detail_
