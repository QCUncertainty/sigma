#pragma once

#include "sigma/detail_/operation_common.hpp"
#include <cmath>

namespace sigma {

// -- Definitions --------------------------------------------------------------
template<typename T>
Uncertain<T> erf(const Uncertain<T>& a) {
    T mean = std::erf(a.mean());
    T dcda = std::exp(-std::pow(a.mean(), 2)) * (2 / std::sqrt(detail_::pi));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> erfc(const Uncertain<T>& a) {
    T mean = std::erfc(a.mean());
    T dcda = -std::exp(-std::pow(a.mean(), 2)) * (2 / std::sqrt(detail_::pi));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> tgamma(const Uncertain<T>& a) {
    auto func = [](decltype(a.mean()) x) { return std::tgamma(x); };
    T mean    = std::tgamma(a.mean());
    T dcda    = detail_::numeric_derivative(func, a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> lgamma(const Uncertain<T>& a) {
    auto func = [](decltype(a.mean()) x) { return std::lgamma(x); };
    T mean    = std::lgamma(a.mean());
    T dcda    = detail_::numeric_derivative(func, a.mean());
    return detail_::unary_result(a, mean, dcda);
}

} // namespace sigma
