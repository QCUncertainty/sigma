#pragma once
#include <utility>

/** @file apply_unary.hpp
 *  @brief Defines the helper the elementary functions of a triplex share.
 *
 *  This is an implementation detail of the triplex operations and is not
 *  meant for users.
 */

namespace sigma {

template<typename IntervalType>
class Triplex;

namespace detail_ {

// Defined in make_triplex.hpp, which is included at the end of this file. It
// is declared here instead so that apply_unary_ is defined no matter which
// header is included first: make_triplex.hpp pulls in the operations, and they
// call apply_unary_.
template<typename IntervalType>
Triplex<IntervalType> make_triplex_(IntervalType bounds,
                                    typename IntervalType::value_t nominal);

/** @brief Applies a unary function to a triplex.
 *
 *  Code factorization for the elementary functions.
 *
 *  @tparam IntervalType The interval type of the triplex.
 *  @tparam BoundsFunction The type of a callable taking an IntervalType and
 *                         returning an IntervalType.
 *  @tparam NominalFunction The type of a callable taking a value_t and
 *                          returning something convertible to value_t.
 *
 *  @param[in] a The argument.
 *  @param[in] bounds_fxn Computes the bounds of the result from those of
 *                        @p a.
 *  @param[in] nominal_fxn Computes the nominal value of the result from that
 *                         of @p a.
 *
 *  @return The function applied to @p a.
 *
 *  @throw std::domain_error if @p bounds_fxn rejects the bounds of @p a.
 *         Strong throw guarantee.
 */
template<typename IntervalType, typename BoundsFunction,
         typename NominalFunction>
Triplex<IntervalType> apply_unary_(const Triplex<IntervalType>& a,
                                   BoundsFunction&& bounds_fxn,
                                   NominalFunction&& nominal_fxn) {
    using value_t = typename IntervalType::value_t;
    if(a.empty()) { return Triplex<IntervalType>(); }
    auto bounds     = bounds_fxn(a.interval());
    value_t nominal = nominal_fxn(a.nominal());
    return make_triplex_(std::move(bounds), nominal);
}

} // namespace detail_
} // namespace sigma

#include "sigma/triplex/detail_/make_triplex.hpp"
