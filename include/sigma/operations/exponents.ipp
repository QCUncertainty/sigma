#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType pow(const UncertainType& a, double exp) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::pow(a.mean(), exp));
    c_setter.update_derivatives(exp * std::pow(a.mean(), exp - 1));
    return c;
}

template<typename UncertainType>
UncertainType pow(const UncertainType& a, const UncertainType& exp) {
    auto dcda = exp.mean() * std::pow(a.mean(), exp.mean() - 1);
    auto dcdb = std::log(a.mean()) * std::pow(a.mean(), exp.mean());
    UncertainType c{};
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::pow(a.mean(), exp.mean()));
    c_setter.update_derivatives(a.deps(), dcda, exp.deps(), dcdb);
    return c;
}

template<typename UncertainType>
UncertainType sqrt(const UncertainType& a) {
    return pow(std::move(a), 0.5);
}

template<typename UncertainType>
UncertainType exp(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::exp(a.mean()));
    c_setter.update_derivatives(std::exp(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType log(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::log(a.mean()));
    c_setter.update_derivatives(1.0 / a.mean());
    return c;
}

template<typename UncertainType>
UncertainType log10(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::log10(a.mean()));
    c_setter.update_derivatives(1.0 / (a.mean() * std::log(10.0)));
    return c;
}

template<typename UncertainType>
UncertainType hypot(const UncertainType& a, const UncertainType& b) {
    auto dcda = a.mean() / std::hypot(a.mean(), b.mean());
    auto dcdb = b.mean() / std::hypot(a.mean(), b.mean());
    UncertainType c{};
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::hypot(a.mean(), b.mean()));
    c_setter.update_derivatives(a.deps(), dcda, b.deps(), dcdb);
    return c;
}

template<typename UncertainType>
UncertainType hypot(const UncertainType& a, double b) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::hypot(a.mean(), b));
    c_setter.update_derivatives(a.mean() / std::hypot(a.mean(), b));
    return c;
}

template<typename UncertainType>
UncertainType hypot(double a, const UncertainType& b) {
    return hypot(b, a);
}

} // namespace sigma
