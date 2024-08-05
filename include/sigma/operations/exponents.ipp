#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Uncertain<T> pow(const Uncertain<T>& a, double exp) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::pow(a.mean(), exp));
    c_setter.update_derivatives(exp * std::pow(a.mean(), exp - 1));
    return c;
}

template<typename T>
Uncertain<T> pow(const Uncertain<T>& a, const Uncertain<T>& exp) {
    auto dcda = exp.mean() * std::pow(a.mean(), exp.mean() - 1);
    auto dcdb = std::log(a.mean()) * std::pow(a.mean(), exp.mean());
    Uncertain<T> c{};
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::pow(a.mean(), exp.mean()));
    c_setter.update_derivatives(a.deps(), dcda, exp.deps(), dcdb);
    return c;
}

template<typename T>
Uncertain<T> sqrt(const Uncertain<T>& a) {
    return pow(std::move(a), 0.5);
}

template<typename T>
Uncertain<T> exp(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::exp(a.mean()));
    c_setter.update_derivatives(std::exp(a.mean()));
    return c;
}

template<typename T>
Uncertain<T> log(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::log(a.mean()));
    c_setter.update_derivatives(1.0 / a.mean());
    return c;
}

template<typename T>
Uncertain<T> log10(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::log10(a.mean()));
    c_setter.update_derivatives(1.0 / (a.mean() * std::log(10.0)));
    return c;
}

template<typename T>
Uncertain<T> hypot(const Uncertain<T>& a, const Uncertain<T>& b) {
    auto dcda = a.mean() / std::hypot(a.mean(), b.mean());
    auto dcdb = b.mean() / std::hypot(a.mean(), b.mean());
    Uncertain<T> c{};
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::hypot(a.mean(), b.mean()));
    c_setter.update_derivatives(a.deps(), dcda, b.deps(), dcdb);
    return c;
}

template<typename T>
Uncertain<T> hypot(const Uncertain<T>& a, double b) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::hypot(a.mean(), b));
    c_setter.update_derivatives(a.mean() / std::hypot(a.mean(), b));
    return c;
}

template<typename T>
Uncertain<T> hypot(double a, const Uncertain<T>& b) {
    return hypot(b, a);
}

} // namespace sigma
