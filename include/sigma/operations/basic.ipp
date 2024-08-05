#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Uncertain<T> abs(const Uncertain<T>& a) {
    auto dcda = (a.mean() >= 0) ? 1.0 : -1.0;
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::abs(a.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename T>
Uncertain<T> abs2(const Uncertain<T>& a) {
    Uncertain<T> c = abs(a);
    c               = pow(c, 2.0);
    return c;
}

template<typename T>
Uncertain<T> ceil(const Uncertain<T>& a) {
    Uncertain<T> c{};
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::ceil(a.mean()));
    return c;
}

template<typename T>
Uncertain<T> floor(const Uncertain<T>& a) {
    Uncertain<T> c{};
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::floor(a.mean()));
    return c;
}

template<typename T>
Uncertain<T> fmod(const Uncertain<T>& a, const Uncertain<T>& b) {
    auto dcda = 1.0;
    auto dcdb = -std::floor(a.mean() / b.mean());
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::fmod(a.mean(), b.mean()));
    c_setter.update_derivatives(dcda, false);
    c_setter.update_derivatives(b.deps(), dcdb);
    return c;
}

template<typename T>
Uncertain<T> fmod(const Uncertain<T>& a, double b) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::fmod(a.mean(), b));
    c_setter.update_derivatives(1.0);
    return c;
}

template<typename T>
Uncertain<T> fmod(double a, const Uncertain<T>& b) {
    Uncertain<T> c(b);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::fmod(a, b.mean()));
    c_setter.update_derivatives(-std::floor(a / b.mean()));
    return c;
}

template<typename T>
Uncertain<T> copysign(const Uncertain<T>& a, const Uncertain<T>& b) {
    return copysign(a, b.mean());
}

template<typename T>
Uncertain<T> copysign(const Uncertain<T>& a, double b) {
    auto b_sign = std::copysign(1.0, b);
    auto dcda   = (a.mean() >= 0) ? b_sign : -b_sign;
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::copysign(a.mean(), b));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename T>
double copysign(double a, const Uncertain<T>& b) {
    return std::copysign(a, b.mean());
}

template<typename T>
Uncertain<T> trunc(const Uncertain<T>& a) {
    Uncertain<T> c{};
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::trunc(a.mean()));
    return c;
}

} // namespace sigma
