#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType pow(const UncertainType& u, double exp) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::pow(u.mean(), exp));
    c_setter.update_derivatives(exp * std::pow(u.mean(), exp - 1));
    return c;
}

template<typename UncertainType>
UncertainType pow(const UncertainType& u, const UncertainType& exp) {
    auto dcda = exp.mean() * std::pow(u.mean(), exp.mean() - 1);
    auto dcdb = std::log(u.mean()) * std::pow(u.mean(), exp.mean());
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::pow(u.mean(), exp.mean()));
    c_setter.update_derivatives(u.deps(), dcda, exp.deps(), dcdb);
    return c;
}

template<typename UncertainType>
UncertainType sqrt(const UncertainType& u) {
    return pow(std::move(u), 0.5);
}

template<typename UncertainType>
UncertainType exp(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::exp(u.mean()));
    c_setter.update_derivatives(std::exp(u.mean()));
    return c;
}

template<typename UncertainType>
UncertainType log(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::log(u.mean()));
    c_setter.update_derivatives(1.0 / u.mean());
    return c;
}

template<typename UncertainType>
UncertainType log10(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::log10(u.mean()));
    c_setter.update_derivatives(1.0 / (u.mean() * std::log(10.0)));
    return c;
}

template<typename UncertainType>
UncertainType hypot(const UncertainType& a, const UncertainType& b) {
    auto dcda = a.mean() / std::hypot(a.mean(), b.mean());
    auto dcdb = b.mean() / std::hypot(a.mean(), b.mean());
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::hypot(a.mean(), b.mean()));
    c_setter.update_derivatives(a.deps(), dcda, b.deps(), dcdb);
    return c;
}

template<typename UncertainType>
UncertainType hypot(const UncertainType& a, double b) {
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::hypot(a.mean(), b));
    c_setter.update_derivatives(a.mean() / std::hypot(a.mean(), b));
    return c;
}

template<typename UncertainType>
UncertainType hypot(double a, const UncertainType& b) {
    return hypot(b, a);
}

} // namespace sigma
