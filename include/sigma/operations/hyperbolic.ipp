#pragma once

#include "sigma/detail_/operation_common.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Uncertain<T> sinh(const Uncertain<T>& a) {
    T mean = std::sinh(a.mean());
    T dcda = std::cosh(a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> cosh(const Uncertain<T>& a) {
    T mean = std::cosh(a.mean());
    T dcda = std::sinh(a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> tanh(const Uncertain<T>& a) {
    T mean = std::tanh(a.mean());
    T dcda = 1.0 - std::pow(std::tanh(a.mean()), 2.0);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> asinh(const Uncertain<T>& a) {
    T mean = std::asinh(a.mean());
    T dcda = 1.0 / std::sqrt(1 + std::pow(a.mean(), 2.0));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> acosh(const Uncertain<T>& a) {
    T mean = std::acosh(a.mean());
    T dcda = 1.0 / std::sqrt(std::pow(a.mean(), 2.0) - 1.0);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> atanh(const Uncertain<T>& a) {
    T mean = std::atanh(a.mean());
    T dcda = 1.0 / (1.0 - std::pow(a.mean(), 2.0));
    return detail_::unary_result(a, mean, dcda);
}

} // namespace sigma
