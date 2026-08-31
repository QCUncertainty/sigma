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
 *  Every function below is named with an explicit `std::`. Boost's own
 *  policies reach them through the BOOST_NUMERIC_INTERVAL_using_math and
 *  _using_ahyp macros, which expand to nothing on some platforms (MSVC among
 *  them); the call is then unqualified and finds sigma's overload for Interval
 *  rather than the scalar one.
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

    /** @brief Moves @p y k_error_ulps_ ulps toward negative infinity.
     *
     *  @param[in] y The value to widen.
     *
     *  @return The widened value. Negative infinity is a fixed point.
     *
     *  @throw none No throw guarantee.
     */
    static T down_(T y) {
        constexpr auto inf = std::numeric_limits<T>::infinity();
        for(int i = 0; i < k_error_ulps_; ++i) { y = std::nextafter(y, -inf); }
        return y;
    }

    /** @brief Moves @p y k_error_ulps_ ulps toward positive infinity.
     *
     *  @param[in] y The value to widen.
     *
     *  @return The widened value. Positive infinity is a fixed point.
     *
     *  @throw none No throw guarantee.
     */
    static T up_(T y) {
        constexpr auto inf = std::numeric_limits<T>::infinity();
        for(int i = 0; i < k_error_ulps_; ++i) { y = std::nextafter(y, inf); }
        return y;
    }

    /** @brief Evaluates @p f with the FPU rounding to nearest.
     *
     *  The mode is put back to upward on the way out because that is the
     *  invariant rounded_arith_opp maintains for the arithmetic operators.
     *
     *  @tparam FunctionType The type of a callable taking no arguments and
     *                       returning a T.
     *
     *  @param[in] f The libm call to evaluate.
     *
     *  @return The value @p f returned, rounded to nearest.
     *
     *  @throw none No throw guarantee, assuming @p f does not throw.
     */
    template<typename FunctionType>
    T nearest_(FunctionType&& f) {
        this->to_nearest();
        T y = this->force_rounding(f());
        this->upward();
        return y;
    }

public:
    /** @brief A lower bound for the exponential of @p x.
     *
     *  @param[in] x The argument to std::exp.
     *
     *  @return A value no greater than the true value of exp(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T exp_down(const T& x) {
        return down_(nearest_([&x] { return std::exp(x); }));
    }

    /** @brief A upper bound for the exponential of @p x.
     *
     *  @param[in] x The argument to std::exp.
     *
     *  @return A value no less than the true value of exp(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T exp_up(const T& x) {
        return up_(nearest_([&x] { return std::exp(x); }));
    }

    /** @brief A lower bound for the natural logarithm of @p x.
     *
     *  @param[in] x The argument to std::log.
     *
     *  @return A value no greater than the true value of log(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T log_down(const T& x) {
        return down_(nearest_([&x] { return std::log(x); }));
    }

    /** @brief A upper bound for the natural logarithm of @p x.
     *
     *  @param[in] x The argument to std::log.
     *
     *  @return A value no less than the true value of log(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T log_up(const T& x) {
        return up_(nearest_([&x] { return std::log(x); }));
    }

    /** @brief A lower bound for the sine of @p x.
     *
     *  @param[in] x The argument to std::sin.
     *
     *  @return A value no greater than the true value of sin(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T sin_down(const T& x) {
        return down_(nearest_([&x] { return std::sin(x); }));
    }

    /** @brief A upper bound for the sine of @p x.
     *
     *  @param[in] x The argument to std::sin.
     *
     *  @return A value no less than the true value of sin(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T sin_up(const T& x) {
        return up_(nearest_([&x] { return std::sin(x); }));
    }

    /** @brief A lower bound for the cosine of @p x.
     *
     *  @param[in] x The argument to std::cos.
     *
     *  @return A value no greater than the true value of cos(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T cos_down(const T& x) {
        return down_(nearest_([&x] { return std::cos(x); }));
    }

    /** @brief A upper bound for the cosine of @p x.
     *
     *  @param[in] x The argument to std::cos.
     *
     *  @return A value no less than the true value of cos(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T cos_up(const T& x) {
        return up_(nearest_([&x] { return std::cos(x); }));
    }

    /** @brief A lower bound for the tangent of @p x.
     *
     *  @param[in] x The argument to std::tan.
     *
     *  @return A value no greater than the true value of tan(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T tan_down(const T& x) {
        return down_(nearest_([&x] { return std::tan(x); }));
    }

    /** @brief A upper bound for the tangent of @p x.
     *
     *  @param[in] x The argument to std::tan.
     *
     *  @return A value no less than the true value of tan(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T tan_up(const T& x) {
        return up_(nearest_([&x] { return std::tan(x); }));
    }

    /** @brief A lower bound for the arcsine of @p x.
     *
     *  @param[in] x The argument to std::asin.
     *
     *  @return A value no greater than the true value of asin(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T asin_down(const T& x) {
        return down_(nearest_([&x] { return std::asin(x); }));
    }

    /** @brief A upper bound for the arcsine of @p x.
     *
     *  @param[in] x The argument to std::asin.
     *
     *  @return A value no less than the true value of asin(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T asin_up(const T& x) {
        return up_(nearest_([&x] { return std::asin(x); }));
    }

    /** @brief A lower bound for the arccosine of @p x.
     *
     *  @param[in] x The argument to std::acos.
     *
     *  @return A value no greater than the true value of acos(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T acos_down(const T& x) {
        return down_(nearest_([&x] { return std::acos(x); }));
    }

    /** @brief A upper bound for the arccosine of @p x.
     *
     *  @param[in] x The argument to std::acos.
     *
     *  @return A value no less than the true value of acos(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T acos_up(const T& x) {
        return up_(nearest_([&x] { return std::acos(x); }));
    }

    /** @brief A lower bound for the arctangent of @p x.
     *
     *  @param[in] x The argument to std::atan.
     *
     *  @return A value no greater than the true value of atan(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T atan_down(const T& x) {
        return down_(nearest_([&x] { return std::atan(x); }));
    }

    /** @brief A upper bound for the arctangent of @p x.
     *
     *  @param[in] x The argument to std::atan.
     *
     *  @return A value no less than the true value of atan(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T atan_up(const T& x) {
        return up_(nearest_([&x] { return std::atan(x); }));
    }

    /** @brief A lower bound for the hyperbolic sine of @p x.
     *
     *  @param[in] x The argument to std::sinh.
     *
     *  @return A value no greater than the true value of sinh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T sinh_down(const T& x) {
        return down_(nearest_([&x] { return std::sinh(x); }));
    }

    /** @brief A upper bound for the hyperbolic sine of @p x.
     *
     *  @param[in] x The argument to std::sinh.
     *
     *  @return A value no less than the true value of sinh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T sinh_up(const T& x) {
        return up_(nearest_([&x] { return std::sinh(x); }));
    }

    /** @brief A lower bound for the hyperbolic cosine of @p x.
     *
     *  @param[in] x The argument to std::cosh.
     *
     *  @return A value no greater than the true value of cosh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T cosh_down(const T& x) {
        return down_(nearest_([&x] { return std::cosh(x); }));
    }

    /** @brief A upper bound for the hyperbolic cosine of @p x.
     *
     *  @param[in] x The argument to std::cosh.
     *
     *  @return A value no less than the true value of cosh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T cosh_up(const T& x) {
        return up_(nearest_([&x] { return std::cosh(x); }));
    }

    /** @brief A lower bound for the hyperbolic tangent of @p x.
     *
     *  @param[in] x The argument to std::tanh.
     *
     *  @return A value no greater than the true value of tanh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T tanh_down(const T& x) {
        return down_(nearest_([&x] { return std::tanh(x); }));
    }

    /** @brief A upper bound for the hyperbolic tangent of @p x.
     *
     *  @param[in] x The argument to std::tanh.
     *
     *  @return A value no less than the true value of tanh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T tanh_up(const T& x) {
        return up_(nearest_([&x] { return std::tanh(x); }));
    }

    /** @brief A lower bound for the inverse hyperbolic sine of @p x.
     *
     *  @param[in] x The argument to std::asinh.
     *
     *  @return A value no greater than the true value of asinh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T asinh_down(const T& x) {
        return down_(nearest_([&x] { return std::asinh(x); }));
    }

    /** @brief A upper bound for the inverse hyperbolic sine of @p x.
     *
     *  @param[in] x The argument to std::asinh.
     *
     *  @return A value no less than the true value of asinh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T asinh_up(const T& x) {
        return up_(nearest_([&x] { return std::asinh(x); }));
    }

    /** @brief A lower bound for the inverse hyperbolic cosine of @p x.
     *
     *  @param[in] x The argument to std::acosh.
     *
     *  @return A value no greater than the true value of acosh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T acosh_down(const T& x) {
        return down_(nearest_([&x] { return std::acosh(x); }));
    }

    /** @brief A upper bound for the inverse hyperbolic cosine of @p x.
     *
     *  @param[in] x The argument to std::acosh.
     *
     *  @return A value no less than the true value of acosh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T acosh_up(const T& x) {
        return up_(nearest_([&x] { return std::acosh(x); }));
    }

    /** @brief A lower bound for the inverse hyperbolic tangent of @p x.
     *
     *  @param[in] x The argument to std::atanh.
     *
     *  @return A value no greater than the true value of atanh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T atanh_down(const T& x) {
        return down_(nearest_([&x] { return std::atanh(x); }));
    }

    /** @brief A upper bound for the inverse hyperbolic tangent of @p x.
     *
     *  @param[in] x The argument to std::atanh.
     *
     *  @return A value no less than the true value of atanh(@p x).
     *
     *  @throw none No throw guarantee.
     */
    T atanh_up(const T& x) {
        return up_(nearest_([&x] { return std::atanh(x); }));
    }
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
