#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType sinh(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::sinh(u.mean()));
    c_setter.update_derivatives(std::cosh(u.mean()));
    return c;
}

template<typename UncertainType>
UncertainType cosh(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::cosh(u.mean()));
    c_setter.update_derivatives(std::sinh(u.mean()));
    return c;
}

template<typename UncertainType>
UncertainType tanh(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::tanh(u.mean()));
    c_setter.update_derivatives(1.0 - std::pow(std::tanh(u.mean()), 2.0));
    return c;
}

template<typename UncertainType>
UncertainType asinh(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::asinh(u.mean()));
    c_setter.update_derivatives(1.0 / std::sqrt(1 + std::pow(u.mean(), 2.0)));
    return c;
}

template<typename UncertainType>
UncertainType acosh(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::acosh(u.mean()));
    c_setter.update_derivatives(1.0 / std::sqrt(std::pow(u.mean(), 2.0) - 1.0));
    return c;
}

template<typename UncertainType>
UncertainType atanh(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::atanh(u.mean()));
    c_setter.update_derivatives(1.0 / (1.0 - std::pow(u.mean(), 2.0)));
    return c;
}

} // namespace sigma
