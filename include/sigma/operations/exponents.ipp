#pragma once

#include "sigma/detail_/operation_common.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Uncertain<T> pow(const Uncertain<T>& a, double exp) {
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
    return pow(std::move(a), 0.5);
}

template<typename T>
Uncertain<T> exp(const Uncertain<T>& a) {
    T mean = std::exp(a.mean());
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
Uncertain<T> hypot(const Uncertain<T>& a, const Uncertain<T>& b) {
    T mean = std::hypot(a.mean(), b.mean());
    T dcda = a.mean() / std::hypot(a.mean(), b.mean());
    T dcdb = b.mean() / std::hypot(a.mean(), b.mean());
    return detail_::binary_result(a, b, mean, dcda, dcdb);
}

template<typename T>
Uncertain<T> hypot(const Uncertain<T>& a, double b) {
    T mean = std::hypot(a.mean(), b);
    T dcda = a.mean() / std::hypot(a.mean(), b);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> hypot(double a, const Uncertain<T>& b) {
    return hypot(b, a);
}

} // namespace sigma
