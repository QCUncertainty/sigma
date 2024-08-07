#pragma once

#include "sigma/detail_/setter.hpp"
#include "sigma/uncertain.hpp"

namespace sigma::detail_ {

/** @brief Generalized Inplace Unary Changes
 *
 *  @tparam T The value type of the variable
 *  @param c The variable being altered
 *  @param mean The new mean value of the variable
 *  @param dcda The partial derivative being added to the chain
 *
 *  @throw none No throw guarantee
 */
template<typename T>
void inplace_unary(Uncertain<T>& c, T mean, T dcda) {
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(mean);
    c_setter.update_derivatives(dcda);
}

/** @brief Generalized Unary Changes
 *
 *  @tparam T The value type of the variable
 *  @param a The variable being altered copied
 *  @param mean The new mean value of the variable
 *  @param dcda The partial derivative being added to the chain
 *
 *  @return A variable with the new mean value and the dependencies of @p a
 *          altered by @p dcda.
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> unary_result(const Uncertain<T>& a, T mean, T dcda) {
    Uncertain<T> c(a);
    detail_::inplace_unary(c, mean, dcda);
    return c;
}

/** @brief Generalized Inplace Binary Changes
 *
 *  @tparam T The value type of the variable
 *  @param c The variable being altered
 *  @param b The variable whose dependencies are being added to @p c's
 *  @param mean The new mean value of the variable
 *  @param dcda The partial derivative being added to the chain from @p a
 *  @param dcdb The partial derivative being added to the chain from @p b
 *
 *  @throw none No throw guarantee
 */
template<typename T>
void inplace_binary(Uncertain<T>& c, const Uncertain<T>& b, T mean, T dcda,
                    T dcdb) {
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(mean);
    c_setter.update_derivatives(dcda, false);
    c_setter.update_derivatives(b.deps(), dcdb);
}

/** @brief Generalized Binary Changes
 *
 *  @tparam T The value type of the variable
 *  @param a The variable being altered copied
 *  @param b The variable whose dependencies are being added to @p a's
 *  @param mean The new mean value of the variable
 *  @param dcda The partial derivative being added to the chain from @p a
 *  @param dcdb The partial derivative being added to the chain from @p b
 *
 *  @return A variable with the new mean value and the dependencies of @p a and
 *          @p b, altered respectively by @p dcda and @p dcdb.
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> binary_result(const Uncertain<T>& a, const Uncertain<T>& b, T mean,
                           T dcda, T dcdb) {
    Uncertain<T> c(a);
    detail_::inplace_binary(c, b, mean, dcda, dcdb);
    return c;
}

} // namespace sigma::detail_