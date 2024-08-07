#pragma once

#include "sigma/detail_/operation_common.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Uncertain<T> abs(const Uncertain<T>& a) {
    T mean = std::abs(a.mean());
    T dcda = (a.mean() >= 0) ? 1.0 : -1.0;
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> abs2(const Uncertain<T>& a) {
    return pow(abs(a), 2.0);
}

template<typename T>
Uncertain<T> ceil(const Uncertain<T>& a) {
    return Uncertain<T>(std::ceil(a.mean()));
}

template<typename T>
Uncertain<T> floor(const Uncertain<T>& a) {
    return Uncertain<T>(std::floor(a.mean()));
}

template<typename T>
Uncertain<T> fmod(const Uncertain<T>& a, const Uncertain<T>& b) {
    T mean = std::fmod(a.mean(), b.mean());
    T dcda = 1.0;
    T dcdb = -std::floor(a.mean() / b.mean());
    return detail_::binary_result(a, b, mean, dcda, dcdb);
}

template<typename T>
Uncertain<T> fmod(const Uncertain<T>& a, double b) {
    T mean = std::fmod(a.mean(), b);
    T dcda = 1.0;
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> fmod(double a, const Uncertain<T>& b) {
    T mean = std::fmod(a, b.mean());
    T dcda = -std::floor(a / b.mean());
    return detail_::unary_result(b, mean, dcda);
}

template<typename T>
Uncertain<T> copysign(const Uncertain<T>& a, const Uncertain<T>& b) {
    return copysign(a, b.mean());
}

template<typename T>
Uncertain<T> copysign(const Uncertain<T>& a, double b) {
    auto b_sign = std::copysign(1.0, b);
    T mean      = std::copysign(a.mean(), b);
    T dcda      = (a.mean() >= 0) ? b_sign : -b_sign;
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
double copysign(double a, const Uncertain<T>& b) {
    return std::copysign(a, b.mean());
}

template<typename T>
Uncertain<T> trunc(const Uncertain<T>& a) {
    return Uncertain<T>(std::trunc(a.mean()));
}

} // namespace sigma
