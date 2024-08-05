#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Uncertain<T> degrees(const Uncertain<T>& a) {
    auto to_degrees = 180.0 / M_PI;
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(a.mean() * to_degrees);
    c_setter.update_derivatives(to_degrees);
    return c;
}

template<typename T>
Uncertain<T> radians(const Uncertain<T>& a) {
    auto to_radians = M_PI / 180.0;
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(a.mean() * to_radians);
    c_setter.update_derivatives(to_radians);
    return c;
}

template<typename T>
Uncertain<T> sin(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::sin(a.mean()));
    c_setter.update_derivatives(std::cos(a.mean()));
    return c;
}

template<typename T>
Uncertain<T> cos(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::cos(a.mean()));
    c_setter.update_derivatives(-std::sin(a.mean()));
    return c;
}

template<typename T>
Uncertain<T> tan(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::tan(a.mean()));
    c_setter.update_derivatives(std::pow(std::tan(a.mean()), 2.0) + 1);
    return c;
}

template<typename T>
Uncertain<T> asin(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::asin(a.mean()));
    c_setter.update_derivatives(1 / std::sqrt(1 - std::pow(a.mean(), 2)));
    return c;
}

template<typename T>
Uncertain<T> acos(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::acos(a.mean()));
    c_setter.update_derivatives(-1 / std::sqrt(1 - std::pow(a.mean(), 2)));
    return c;
}

template<typename T>
Uncertain<T> atan(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::atan(a.mean()));
    c_setter.update_derivatives(1 / (1 + std::pow(a.mean(), 2)));
    return c;
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
