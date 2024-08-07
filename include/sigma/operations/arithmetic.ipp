#pragma once

#include "sigma/detail_/operation_common.hpp"

namespace sigma {

template<typename T>
Uncertain<T> operator-(const Uncertain<T>& a) {
    T mean = -a.mean();
    T dcda = -1.0;
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> operator+(const Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    Uncertain<T> c(lhs);
    c += rhs;
    return c;
}

template<typename T>
Uncertain<T> operator+(const Uncertain<T>& lhs, double rhs) {
    Uncertain<T> c(lhs);
    c += rhs;
    return c;
}

template<typename T>
Uncertain<T> operator+(double lhs, const Uncertain<T>& rhs) {
    Uncertain<T> c(rhs);
    c += lhs;
    return c;
}

template<typename T>
Uncertain<T>& operator+=(Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    T mean = lhs.mean() + rhs.mean();
    T dcda = 1.0;
    T dcdb = 1.0;
    detail_::inplace_binary(lhs, rhs, mean, dcda, dcdb);
    return lhs;
}

template<typename T>
Uncertain<T>& operator+=(Uncertain<T>& lhs, double rhs) {
    T mean = lhs.mean() + rhs;
    T dcda = 1.0;
    detail_::inplace_unary(lhs, mean, dcda);
    return lhs;
}

template<typename T>
Uncertain<T> operator-(const Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    Uncertain<T> c(lhs);
    c -= rhs;
    return c;
}

template<typename T>
Uncertain<T> operator-(const Uncertain<T>& lhs, double rhs) {
    Uncertain<T> c(lhs);
    c -= rhs;
    return c;
}

template<typename T>
Uncertain<T> operator-(double lhs, const Uncertain<T>& rhs) {
    T mean = lhs - rhs.mean();
    T dcda = -1.0;
    return detail_::unary_result(rhs, mean, dcda);
}

template<typename T>
Uncertain<T>& operator-=(Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    T mean = lhs.mean() - rhs.mean();
    T dcda = 1.0;
    T dcdb = -1.0;
    detail_::inplace_binary(lhs, rhs, mean, dcda, dcdb);
    return lhs;
}

template<typename T>
Uncertain<T>& operator-=(Uncertain<T>& lhs, double rhs) {
    T mean = lhs.mean() - rhs;
    T dcda = 1.0;
    detail_::inplace_unary(lhs, mean, dcda);
    return lhs;
}

template<typename T>
Uncertain<T> operator*(const Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    Uncertain<T> c(lhs);
    c *= rhs;
    return c;
}

template<typename T>
Uncertain<T> operator*(const Uncertain<T>& lhs, double rhs) {
    Uncertain<T> c(lhs);
    c *= rhs;
    return c;
}

template<typename T>
Uncertain<T> operator*(double lhs, const Uncertain<T>& rhs) {
    return rhs * lhs;
}

template<typename T>
Uncertain<T>& operator*=(Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    T mean = lhs.mean() * rhs.mean();
    T dcda = rhs.mean();
    T dcdb = lhs.mean();
    detail_::inplace_binary(lhs, rhs, mean, dcda, dcdb);
    return lhs;
}

template<typename T>
Uncertain<T>& operator*=(Uncertain<T>& lhs, double rhs) {
    T mean = lhs.mean() * rhs;
    T dcda = rhs;
    detail_::inplace_unary(lhs, mean, dcda);
    return lhs;
}

template<typename T>
Uncertain<T> operator/(const Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    Uncertain<T> c(lhs);
    c /= rhs;
    return c;
}

template<typename T>
Uncertain<T> operator/(const Uncertain<T>& lhs, double rhs) {
    Uncertain<T> c(lhs);
    c /= rhs;
    return c;
}

template<typename T>
Uncertain<T> operator/(double lhs, const Uncertain<T>& rhs) {
    T mean = lhs / rhs.mean();
    T dcda = -lhs / std::pow(rhs.mean(), 2.0);
    return detail_::unary_result(rhs, mean, dcda);
}

template<typename T>
Uncertain<T>& operator/=(Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    T mean = lhs.mean() / rhs.mean();
    T dcda = 1.0 / rhs.mean();
    T dcdb = -lhs.mean() / std::pow(rhs.mean(), 2.0);
    detail_::inplace_binary(lhs, rhs, mean, dcda, dcdb);
    return lhs;
}

template<typename T>
Uncertain<T>& operator/=(Uncertain<T>& lhs, double rhs) {
    T mean = lhs.mean() / rhs;
    T dcda = 1.0 / rhs;
    detail_::inplace_unary(lhs, mean, dcda);
    return lhs;
}

} // namespace sigma
