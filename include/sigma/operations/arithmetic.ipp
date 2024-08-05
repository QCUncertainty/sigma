#pragma once

#include "sigma/detail_/setter.hpp"

namespace sigma {

template<typename T>
Uncertain<T> operator-(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(-a.mean());
    c_setter.update_derivatives(-1.0);
    return c;
}

template<typename T>
Uncertain<T> operator+(const Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    Uncertain<T> c(lhs);
    c += rhs;
    return c;
}

template<typename T>
Uncertain<T>& operator+=(Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    detail_::Setter<Uncertain<T>> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() + rhs.mean());
    lhs_setter.update_derivatives(rhs.deps(), 1.0);
    return lhs;
}

template<typename T>
Uncertain<T> operator-(const Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    Uncertain<T> c(lhs);
    c -= rhs;
    return c;
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
    auto dcda = rhs.mean();
    auto dcdb = lhs.mean();
    detail_::Setter<Uncertain<T>> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() * rhs.mean());
    lhs_setter.update_derivatives(dcda, false);
    lhs_setter.update_derivatives(rhs.deps(), dcdb);
    return lhs;
}

template<typename T>
Uncertain<T>& operator*=(Uncertain<T>& lhs, double rhs) {
    detail_::Setter<Uncertain<T>> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() * rhs);
    lhs_setter.update_derivatives(rhs);
    return lhs;
}

template<typename T>
Uncertain<T>& operator-=(Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    detail_::Setter<Uncertain<T>> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() - rhs.mean());
    lhs_setter.update_derivatives(rhs.deps(), -1.0);
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
    Uncertain<T> c(rhs);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(lhs / rhs.mean());
    c_setter.update_derivatives(-lhs / std::pow(rhs.mean(), 2.0));
    return c;
}

template<typename T>
Uncertain<T>& operator/=(Uncertain<T>& lhs, const Uncertain<T>& rhs) {
    auto dcda = 1.0 / rhs.mean();
    auto dcdb = -lhs.mean() / std::pow(rhs.mean(), 2.0);
    detail_::Setter<Uncertain<T>> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() / rhs.mean());
    lhs_setter.update_derivatives(dcda, false);
    lhs_setter.update_derivatives(rhs.deps(), dcdb);
    return lhs;
}

template<typename T>
Uncertain<T>& operator/=(Uncertain<T>& lhs, double rhs) {
    detail_::Setter<Uncertain<T>> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() / rhs);
    lhs_setter.update_derivatives(1.0 / rhs);
    return lhs;
}

} // namespace sigma
