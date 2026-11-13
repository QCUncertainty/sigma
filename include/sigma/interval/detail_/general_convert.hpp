#pragma once
#include "sigma/interval/detail_/convert.hpp"
#include "sigma/interval/general_interval.hpp"
#include <stdexcept>
#include <string>

/** @file general_convert.hpp
 *  @brief Conversions between sigma::GeneralInterval and the boost interval its
 *         closure wraps.
 *
 *  The elementary functions of a GeneralInterval hand the bounds to boost,
 *  exactly as those of an Interval do, and then restore the openness of the
 *  bounds, which boost does not track.
 *
 *  @note On openness. Each caller states what the openness of the result
 *        should be:
 *
 *        - For a monotonically increasing function the bounds of the result
 *          come from the bounds of the input, so the input's openness carries
 *          over unchanged.
 *        - For a monotonically decreasing function the bounds swap, and so
 *          does their openness.
 *        - When a bound comes from an interior extremum (the maximum of cos,
 *          the minimum of cosh) or from a clamp (the 0 of sqrt, the
 *          \f$ \pm\pi/2 \f$ of asin), it does not correspond to an input bound
 *          at all. Such a bound is reported closed: the value really is
 *          attained, and in the cases where it is not, a closed bound is a
 *          superset of an open one and therefore still a valid enclosure.
 */

namespace sigma::detail_ {

/** @brief Convert a sigma::GeneralInterval into a boost interval.
 *
 *  @tparam T The value type of the interval.
 *
 *  @param[in] a The interval to convert. Must not be empty.
 *
 *  @return A boost interval with the same bounds as @p a. The openness of
 *          @p a's bounds is not represented in the result.
 *
 *  @throw std::domain_error if @p a is empty. Strong throw guarantee.
 */
template<typename T>
boost_interval_t<T> to_boost_(const GeneralInterval<T>& a) {
    return to_boost_(a.closure());
}

/** @brief Convert a boost interval into a sigma::GeneralInterval.
 *
 *  @tparam T The value type of the interval.
 *
 *  @param[in] x The interval to convert.
 *  @param[in] left_open Should the lower bound of the result be open?
 *  @param[in] right_open Should the upper bound of the result be open?
 *
 *  @return An interval with the bounds of @p x and the requested openness. An
 *          empty boost interval converts to the empty sigma::GeneralInterval.
 *          An infinite bound is always open, regardless of what the caller
 *          requested, since it is not a value the interval contains.
 *
 *  @throw none No throw guarantee.
 */
template<typename T>
GeneralInterval<T> general_from_boost_(const boost_interval_t<T>& x,
                                       bool left_open  = false,
                                       bool right_open = false) {
    if(boost::numeric::empty(x)) { return GeneralInterval<T>(); }
    return GeneralInterval<T>(x.lower(), x.upper(), left_open, right_open);
}

/** @brief Throws unless every value in @p a lies in the unit interval.
 *
 *  The openness-aware counterpart of the Interval overload. Whether a bound
 *  *at* plus or minus one is in domain depends on both @p open and the
 *  openness of that bound, since an open bound is not a value the interval
 *  contains: \f$ (-1, 1) \f$ is an acceptable argument to the inverse
 *  hyperbolic tangent, and \f$ [-1, 1] \f$ is not.
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
void assert_within_unit_(const GeneralInterval<T>& a, bool open,
                         const std::string& name) {
    auto below =
      a.lower() < T(-1) || (open && a.lower() == T(-1) && a.left_closed());
    auto above =
      a.upper() > T(1) || (open && a.upper() == T(1) && a.right_closed());
    if(below || above) {
        throw std::domain_error("Interval has values outside the domain of " +
                                name + ", " + (open ? "(-1, 1)" : "[-1, 1]") +
                                ".");
    }
}

} // namespace sigma::detail_
