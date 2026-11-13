#pragma once
#include "sigma/interval/detail_/policies.hpp"
#include "sigma/interval/interval.hpp"
#include <stdexcept>
#include <string>

/** @file convert.hpp
 *  @brief Conversions between sigma::Interval and the boost interval it wraps.
 *
 *  The elementary functions of an Interval are implemented by handing the
 *  bounds to boost, which does the outwardly rounded numerics, and then
 *  restoring the one piece of state boost does not track: emptiness. These
 *  helpers are the two halves of that round trip. The counterparts for
 *  GeneralInterval, which also restore the openness of the bounds, are in
 *  general_convert.hpp.
 */

namespace sigma::detail_ {

/** @brief Convert a sigma::Interval into the boost interval it wraps.
 *
 *  @tparam T The value type of the interval.
 *
 *  @param[in] a The interval to convert. Must not be empty.
 *
 *  @return A boost interval with the same bounds as @p a.
 *
 *  @throw std::domain_error if @p a is empty. Strong throw guarantee.
 */
template<typename T>
boost_interval_t<T> to_boost_(const Interval<T>& a) {
    return boost_interval_t<T>(a.lower(), a.upper());
}

/** @brief Convert a boost interval into a sigma::Interval.
 *
 *  @tparam T The value type of the interval.
 *
 *  @param[in] x The interval to convert.
 *
 *  @return An interval with the bounds of @p x. An empty boost interval, which
 *          is how boost signals that a function was given an argument outside
 *          its domain, converts to the empty sigma::Interval.
 *
 *  @throw none No throw guarantee.
 */
template<typename T>
Interval<T> from_boost_(const boost_interval_t<T>& x) {
    if(boost::numeric::empty(x)) { return Interval<T>(); }
    return Interval<T>(x.lower(), x.upper());
}

/** @brief Throws unless every value in @p a lies in the unit interval.
 *
 *  Code factorization for the three functions whose domain is the unit
 *  interval: arcsine and arccosine take the closed \f$ [-1, 1] \f$, and the
 *  inverse hyperbolic tangent the open \f$ (-1, 1) \f$. The bounds of @p a are
 *  members of it, so for the open domain a bound *at* plus or minus one is out
 *  of domain.
 *
 *  @tparam T The value type of the interval.
 *
 *  @param[in] a The interval to check. Must not be empty.
 *  @param[in] open Is the domain the open unit interval? False for the closed
 *                  one.
 *  @param[in] name The name of the function being checked, for the error
 *                  message.
 *
 *  @throw std::domain_error if @p a contains a value outside the domain.
 *         Strong throw guarantee.
 *  @throw std::domain_error if @p a is empty. Strong throw guarantee.
 */
template<typename T>
void assert_within_unit_(const Interval<T>& a, bool open,
                         const std::string& name) {
    auto below = a.lower() < T(-1) || (open && a.lower() == T(-1));
    auto above = a.upper() > T(1) || (open && a.upper() == T(1));
    if(below || above) {
        throw std::domain_error("Interval has values outside the domain of " +
                                name + ", " + (open ? "(-1, 1)" : "[-1, 1]") +
                                ".");
    }
}

} // namespace sigma::detail_
