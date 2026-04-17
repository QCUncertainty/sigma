#pragma once

#include "sigma/detail_/operation_common.hpp"
#include "sigma/interval.hpp"
#include <cmath>

namespace sigma {

template<typename T, typename U>
Uncertain<T> pow(const Uncertain<T>& a, const U& exp) {
    T mean = std::pow(a.mean(), exp);
    T dcda = exp * std::pow(a.mean(), exp - 1);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> pow(const Uncertain<T>& a, const Uncertain<T>& exp) {
    T mean = std::pow(a.mean(), exp.mean());
    T dcda = exp.mean() * std::pow(a.mean(), exp.mean() - 1);
    T dcdb = std::log(a.mean()) * std::pow(a.mean(), exp.mean());
    return detail_::binary_result(a, exp, mean, dcda, dcdb);
}

template<typename T>
Uncertain<T> sqrt(const Uncertain<T>& a) {
    T mean = std::sqrt(a.mean());
    T dcda = 1.0 / (2.0 * std::sqrt(a.mean()));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> cbrt(const Uncertain<T>& a) {
    T mean = std::cbrt(a.mean());
    T dcda = 1.0 / (3.0 * std::cbrt(std::pow(a.mean(), 2.0)));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> exp(const Uncertain<T>& a) {
    T mean = std::exp(a.mean());
    T dcda = std::exp(a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> exp2(const Uncertain<T>& a) {
    T mean = std::exp2(a.mean());
    T dcda = mean * std::log(2.0);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> expm1(const Uncertain<T>& a) {
    T mean = std::expm1(a.mean());
    T dcda = std::exp(a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> log(const Uncertain<T>& a) {
    T mean = std::log(a.mean());
    T dcda = 1.0 / a.mean();
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> log10(const Uncertain<T>& a) {
    T mean = std::log10(a.mean());
    T dcda = 1.0 / (a.mean() * std::log(10.0));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> log2(const Uncertain<T>& a) {
    T mean = std::log2(a.mean());
    T dcda = 1.0 / (a.mean() * std::log(2.0));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> log1p(const Uncertain<T>& a) {
    T mean = std::log1p(a.mean());
    T dcda = 1.0 / (a.mean() + 1.0);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> hypot(const Uncertain<T>& a, const Uncertain<T>& b) {
    T mean = std::hypot(a.mean(), b.mean());
    T dcda = a.mean() / std::hypot(a.mean(), b.mean());
    T dcdb = b.mean() / std::hypot(a.mean(), b.mean());
    return detail_::binary_result(a, b, mean, dcda, dcdb);
}

template<typename T, typename U>
Uncertain<T> hypot(const Uncertain<T>& a, const U& b) {
    T mean = std::hypot(a.mean(), b);
    T dcda = a.mean() / std::hypot(a.mean(), b);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T, typename U>
Uncertain<T> hypot(const U& a, const Uncertain<T>& b) {
    return hypot(b, a);
}

template<typename T>
Interval<T> sqrt(const Interval<T>& a) {
    auto result =
      boost::numeric::sqrt(boost::numeric::interval<T>(a.lower(), a.upper()));
    return Interval<T>(result.lower(), result.upper());
}

template<typename T>
Interval<T> exp(const Interval<T>& a) {
    return Interval<T>(std::exp(a.lower()), std::exp(a.upper()));
}

template<typename T>
Interval<T> log(const Interval<T>& a) {
    return Interval<T>(std::log(a.lower()), std::log(a.upper()));
}

template<typename T>
GeneralInterval<T> sqrt(const GeneralInterval<T>& a) {
    using value_t   = typename GeneralInterval<T>::value_t;
    auto new_center = sigma::sqrt(a.center());
    auto new_dep    = a.dep();
    // Derivative of sqrt(x) = 1 / (2 * sqrt(x))
    auto deriv = value_t(0.5) / sigma::sqrt(a.as_interval());
    for(auto&& [radius, weight] : new_dep) { new_dep[radius] *= deriv; }
    return GeneralInterval<T>(new_center, new_dep);
}

template<typename T>
GeneralInterval<T> exp(const GeneralInterval<T>& a) {
    auto new_center = sigma::exp(a.center());
    auto new_dep    = a.dep();
    // Derivative of exp(x) = exp(x)
    auto deriv = sigma::exp(a.as_interval());
    for(auto&& [radius, weight] : new_dep) { new_dep[radius] *= deriv; }
    return GeneralInterval<T>(new_center, new_dep);
}

template<typename T>
GeneralInterval<T> log(const GeneralInterval<T>& a) {
    throw std::runtime_error("log(GeneralInterval) not implemented");
}

} // namespace sigma
