#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType abs(const UncertainType& a) {
    auto dcda = (a.mean() >= 0) ? 1.0 : -1.0;
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::abs(a.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename UncertainType>
UncertainType ceil(const UncertainType& a) {
    UncertainType c;
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::ceil(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType floor(const UncertainType& a) {
    UncertainType c;
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::floor(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType fmod(const UncertainType& a, const UncertainType& b) {
    auto dcda = 1.0;
    auto dcdb = -std::floor(a.mean() / b.mean());
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::fmod(a.mean(), b.mean()));
    c_setter.update_derivatives(dcda, false);
    c_setter.update_derivatives(b.deps(), dcdb);
    return c;
}

template<typename UncertainType>
UncertainType fmod(const UncertainType& a, double b) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::fmod(a.mean(), b));
    c_setter.update_derivatives(1.0);
    return c;
}

template<typename UncertainType>
UncertainType fmod(double a, const UncertainType& b) {
    UncertainType c(b);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::fmod(a, b.mean()));
    c_setter.update_derivatives(-std::floor(a / b.mean()));
    return c;
}

template<typename UncertainType>
UncertainType copysign(const UncertainType& a, const UncertainType& b) {
    return copysign(a, b.mean());
}

template<typename UncertainType>
UncertainType copysign(const UncertainType& a, double b) {
    auto b_sign = std::copysign(1.0, b);
    auto dcda   = (a.mean() >= 0) ? b_sign : -b_sign;
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::copysign(a.mean(), b));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename UncertainType>
double copysign(double a, const UncertainType& b) {
    return std::copysign(a, b.mean());
}

template<typename UncertainType>
UncertainType trunc(const UncertainType& a) {
    UncertainType c;
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::trunc(a.mean()));
    return c;
}

} // namespace sigma
