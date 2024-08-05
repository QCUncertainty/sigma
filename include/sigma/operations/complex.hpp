#pragma once
#include "sigma/uncertain.hpp"

namespace sigma {

/** @relates Uncertain
 *  @brief Complex Conjugate
 *
 *  sigma doesn't currently support complex values, so this is a stub.
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The complex conjugate of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
const Uncertain<T>& conj(const Uncertain<T>& x) {
    return x;
}

/** @relates Uncertain
 *  @brief Get the real part of a value
 *
 *  sigma doesn't currently support complex values, so this is a stub.
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The real part of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
const Uncertain<T>& real(const Uncertain<T>& x) {
    return x;
}

/** @relates Uncertain
 *  @brief Get the imaginary part of a value
 *
 *  sigma doesn't currently support complex values, so this is a stub.
 *
 *  @tparam UncertainType The type of the variable
 *  @param a The variable
 *
 *  @return The imaginary part of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> imag(const Uncertain<T>&) {
    return Uncertain<T>{0.0, 0.0};
}

} // namespace sigma