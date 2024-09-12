#pragma once
#include "sigma/uncertain.hpp"
#include <complex>

/** @file complex.hpp
 *  @brief Complex number operations for uncertain variables
 */

namespace sigma {

/** @brief Complex Conjugate
 *
 *  This is a stub to satisfy Eigen
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The complex conjugate of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
const Uncertain<T>& conj(const Uncertain<T>& a) {
    return a;
}

/** @brief Get the real part of a value
 *
 *  This is a stub to satisfy Eigen
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The real part of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
const Uncertain<T>& real(const Uncertain<T>& a) {
    return a;
}

/** @brief Get the imaginary part of a value
 *
 *  This is a stub to satisfy Eigen
 *
 *  @tparam T The value type of the variable
 *  @param a The variable
 *
 *  @return The imaginary part of @p a
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Uncertain<T> imag(const Uncertain<T>& a) {
    return Uncertain<T>{0.0, 0.0};
}

} // namespace sigma