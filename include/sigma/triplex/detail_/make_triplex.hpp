#pragma once
#include "sigma/interval/general_interval.hpp"
#include "sigma/interval/interval.hpp"
#include <cmath>
#include <utility>

/** @file make_triplex.hpp
 *  @brief Defines the function every triplex operation uses to build its
 *         result.
 *
 *  This is an implementation detail of Triplex and is not meant for users:
 *  unlike the public constructors, it can produce a triplex whose nominal value
 *  is not a member of its interval.
 */

namespace sigma {

template<typename IntervalType>
class Triplex;

namespace detail_ {

/** @brief Assembles a triplex from the result of an operation.
 *
 *  Every operation on a triplex computes its bounds and its nominal value
 *  separately and then hands them to this function, which applies the widening
 *  rule: a finite @p nominal that is not a member of @p bounds widens @p bounds
 *  to include it. The nominal value itself is never moved. A non-finite
 *  @p nominal is kept as is and @p bounds are left alone, so the result does
 *  not satisfy the class invariant, which is exactly what happens in ordinary
 *  floating point.
 *
 *  @tparam IntervalType The interval type of the triplex.
 *
 *  @param[in] bounds The bounds the interval operation produced.
 *  @param[in] nominal The nominal value the scalar operation produced.
 *
 *  @return The triplex with nominal value @p nominal and bounds @p bounds,
 *          widened if needed.
 *
 *  @throw none No throw guarantee.
 */
template<typename IntervalType>
Triplex<IntervalType> make_triplex_(IntervalType bounds,
                                    typename IntervalType::value_t nominal) {
    using triplex_t = Triplex<IntervalType>;
    if(std::isfinite(nominal)) { bounds = sigma::widen(bounds, nominal); }
    return triplex_t(std::move(bounds), nominal,
                     typename triplex_t::unchecked_tag_{});
}

} // namespace detail_
} // namespace sigma

#include "sigma/triplex/triplex.hpp"
