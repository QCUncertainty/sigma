#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename T>
Uncertain<T> sinh(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::sinh(a.mean()));
    c_setter.update_derivatives(std::cosh(a.mean()));
    return c;
}

template<typename T>
Uncertain<T> cosh(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::cosh(a.mean()));
    c_setter.update_derivatives(std::sinh(a.mean()));
    return c;
}

template<typename T>
Uncertain<T> tanh(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::tanh(a.mean()));
    c_setter.update_derivatives(1.0 - std::pow(std::tanh(a.mean()), 2.0));
    return c;
}

template<typename T>
Uncertain<T> asinh(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::asinh(a.mean()));
    c_setter.update_derivatives(1.0 / std::sqrt(1 + std::pow(a.mean(), 2.0)));
    return c;
}

template<typename T>
Uncertain<T> acosh(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::acosh(a.mean()));
    c_setter.update_derivatives(1.0 / std::sqrt(std::pow(a.mean(), 2.0) - 1.0));
    return c;
}

template<typename T>
Uncertain<T> atanh(const Uncertain<T>& a) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(std::atanh(a.mean()));
    c_setter.update_derivatives(1.0 / (1.0 - std::pow(a.mean(), 2.0)));
    return c;
}

} // namespace sigma
