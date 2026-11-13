#pragma once
#include "sigma/interval/general_interval.hpp"

/** @file general_operations/hyperbolic.hpp
 *  @brief Hyperbolic operations for general interval variables
 *
 *  These follow the Interval overloads, and report the openness of each bound
 *  of the result: it carries over from the argument for a monotonic function,
 *  and a bound that comes from an interior extremum is closed.
 */

namespace sigma {

/** @brief Calculate the hyperbolic sine of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose hyperbolic sine is computed
 *
 *  @return An interval enclosing the hyperbolic sine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> sinh(const GeneralInterval<T>& a);

/** @brief Calculate the hyperbolic cosine of a general interval
 *
 *  An argument spanning zero has a result whose lower bound is the attained
 *  minimum, one, which is closed.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose hyperbolic cosine is computed
 *
 *  @return An interval enclosing the hyperbolic cosine of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> cosh(const GeneralInterval<T>& a);

/** @brief Calculate the hyperbolic tangent of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose hyperbolic tangent is computed
 *
 *  @return An interval enclosing the hyperbolic tangent of every value in @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> tanh(const GeneralInterval<T>& a);

/** @brief Calculate the inverse hyperbolic sine of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose inverse hyperbolic sine is computed
 *
 *  @return An interval enclosing the inverse hyperbolic sine of every value in
 *          @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> asinh(const GeneralInterval<T>& a);

/** @brief Calculate the inverse hyperbolic cosine of a general interval
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose inverse hyperbolic cosine is computed
 *
 *  @return An interval enclosing the inverse hyperbolic cosine of every value
 *          in @p a
 *
 *  @throw std::domain_error if @p a contains a value below one. Strong throw
 *         guarantee.
 */
template<typename T>
GeneralInterval<T> acosh(const GeneralInterval<T>& a);

/** @brief Calculate the inverse hyperbolic tangent of a general interval
 *
 *  The domain is the open \f$ (-1, 1) \f$, so a bound at plus or minus one is
 *  only out of domain if it is closed, and an open bound there leaves the
 *  result unbounded on that side.
 *
 *  @tparam T The value type of the interval
 *  @param a The interval whose inverse hyperbolic tangent is computed
 *
 *  @return An interval enclosing the inverse hyperbolic tangent of every value
 *          in @p a
 *
 *  @throw std::domain_error if @p a contains a value outside
 *         \f$ (-1, 1) \f$. Strong throw guarantee.
 */
template<typename T>
GeneralInterval<T> atanh(const GeneralInterval<T>& a);

} // namespace sigma

#include "hyperbolic.ipp"
