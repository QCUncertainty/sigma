#pragma once

#include "sigma/detail_/operation_common.hpp"
#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Uncertain<T> degrees(const Uncertain<T>& a) {
    auto to_degrees = 180.0 / M_PI;
    T mean          = a.mean() * to_degrees;
    T dcda          = to_degrees;
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> radians(const Uncertain<T>& a) {
    auto to_radians = M_PI / 180.0;
    T mean          = a.mean() * to_radians;
    T dcda          = to_radians;
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> sin(const Uncertain<T>& a) {
    T mean = std::sin(a.mean());
    T dcda = std::cos(a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> cos(const Uncertain<T>& a) {
    T mean = std::cos(a.mean());
    T dcda = -std::sin(a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> tan(const Uncertain<T>& a) {
    T mean = std::tan(a.mean());
    T dcda = std::pow(std::tan(a.mean()), 2.0) + 1;
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> asin(const Uncertain<T>& a) {
    T mean = std::asin(a.mean());
    T dcda = 1 / std::sqrt(1 - std::pow(a.mean(), 2));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> acos(const Uncertain<T>& a) {
    T mean = std::acos(a.mean());
    T dcda = -1 / std::sqrt(1 - std::pow(a.mean(), 2));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> atan(const Uncertain<T>& a) {
    T mean = std::atan(a.mean());
    T dcda = 1 / (1 + std::pow(a.mean(), 2));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> atan2(const Uncertain<T>& y, const Uncertain<T>& x) {
    auto dcda = x.mean() / (std::pow(x.mean(), 2) + std::pow(y.mean(), 2));
    auto dcdb = -y.mean() / (std::pow(x.mean(), 2) + std::pow(y.mean(), 2));
    Uncertain<T> c(y);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::atan2(y.mean(), x.mean()));
    c_setter.update_derivatives(dcda, false);
    c_setter.update_derivatives(x.deps(), dcdb);
    return c;
}

template<typename T>
Uncertain<T> atan2(const Uncertain<T>& y, double x) {
    auto dcda = x / (std::pow(x, 2) + std::pow(y.mean(), 2));
    Uncertain<T> c(y);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::atan2(y.mean(), x));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename T>
Uncertain<T> atan2(double y, const Uncertain<T>& x) {
    auto dcda = -y / (std::pow(x.mean(), 2) + std::pow(y, 2));
    Uncertain<T> c(x);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::atan2(y, x.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

} // namespace sigma
