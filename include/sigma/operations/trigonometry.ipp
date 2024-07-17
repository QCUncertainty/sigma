#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType degrees(const UncertainType& a) {
    auto to_degrees = 180.0 / M_PI;
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(a.mean() * to_degrees);
    c_setter.update_derivatives(to_degrees);
    return c;
}

template<typename UncertainType>
UncertainType radians(const UncertainType& a) {
    auto to_radians = M_PI / 180.0;
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(a.mean() * to_radians);
    c_setter.update_derivatives(to_radians);
    return c;
}

template<typename UncertainType>
UncertainType sin(const UncertainType& a) {
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::sin(a.mean()));
    c_setter.update_derivatives(std::cos(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType cos(const UncertainType& a) {
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::cos(a.mean()));
    c_setter.update_derivatives(-std::sin(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType tan(const UncertainType& a) {
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::tan(a.mean()));
    c_setter.update_derivatives(std::pow(std::tan(a.mean()), 2.0) + 1);
    return c;
}

template<typename UncertainType>
UncertainType asin(const UncertainType& a) {
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::asin(a.mean()));
    c_setter.update_derivatives(1 / std::sqrt(1 - std::pow(a.mean(), 2)));
    return c;
}

template<typename UncertainType>
UncertainType acos(const UncertainType& a) {
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::acos(a.mean()));
    c_setter.update_derivatives(-1 / std::sqrt(1 - std::pow(a.mean(), 2)));
    return c;
}

template<typename UncertainType>
UncertainType atan(const UncertainType& a) {
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::atan(a.mean()));
    c_setter.update_derivatives(1 / (1 + std::pow(a.mean(), 2)));
    return c;
}

template<typename UncertainType>
UncertainType atan2(const UncertainType& y, const UncertainType& x) {
    auto dcda = x.mean() / (std::pow(x.mean(), 2) + std::pow(y.mean(), 2));
    auto dcdb = -y.mean() / (std::pow(x.mean(), 2) + std::pow(y.mean(), 2));
    UncertainType c(y);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::atan2(y.mean(), x.mean()));
    c_setter.update_derivatives(dcda, false);
    c_setter.update_derivatives(x.deps(), dcdb);
    return c;
}

template<typename UncertainType>
UncertainType atan2(const UncertainType& y, double x) {
    auto dcda = x / (std::pow(x, 2) + std::pow(y.mean(), 2));
    UncertainType c(y);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::atan2(y.mean(), x));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename UncertainType>
UncertainType atan2(double y, const UncertainType& x) {
    auto dcda = -y / (std::pow(x.mean(), 2) + std::pow(y, 2));
    UncertainType c(x);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::atan2(y, x.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

} // namespace sigma
