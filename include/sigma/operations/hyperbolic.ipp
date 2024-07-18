#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType sinh(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::sinh(a.mean()));
    c_setter.update_derivatives(std::cosh(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType cosh(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::cosh(a.mean()));
    c_setter.update_derivatives(std::sinh(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType tanh(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::tanh(a.mean()));
    c_setter.update_derivatives(1.0 - std::pow(std::tanh(a.mean()), 2.0));
    return c;
}

template<typename UncertainType>
UncertainType asinh(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::asinh(a.mean()));
    c_setter.update_derivatives(1.0 / std::sqrt(1 + std::pow(a.mean(), 2.0)));
    return c;
}

template<typename UncertainType>
UncertainType acosh(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::acosh(a.mean()));
    c_setter.update_derivatives(1.0 / std::sqrt(std::pow(a.mean(), 2.0) - 1.0));
    return c;
}

template<typename UncertainType>
UncertainType atanh(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::atanh(a.mean()));
    c_setter.update_derivatives(1.0 / (1.0 - std::pow(a.mean(), 2.0)));
    return c;
}

} // namespace sigma
