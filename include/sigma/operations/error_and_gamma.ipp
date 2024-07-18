#pragma once

#include "sigma/detail_/setter.hpp"
#include <cmath>

namespace sigma {

template<typename FunctionType, typename NumericType>
NumericType numeric_derivative(FunctionType f, NumericType a) {
    // Step and step size chosen for consistency with uncertainties package
    auto step_size = std::sqrt(std::numeric_limits<float>::epsilon());
    auto step      = step_size * std::abs(a);
    auto a_plus    = f(a + step);
    auto a_minus   = f(a - step);
    auto dcda      = (a_plus - a_minus) / (2 * step);
    return dcda;
}

// -- Definitions --------------------------------------------------------------
template<typename UncertainType>
UncertainType erf(const UncertainType& a) {
    auto dcda = std::exp(-std::pow(a.mean(), 2)) * (2 / std::sqrt(M_PI));
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::erf(a.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename UncertainType>
UncertainType erfc(const UncertainType& a) {
    auto dcda = -std::exp(-std::pow(a.mean(), 2)) * (2 / std::sqrt(M_PI));
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::erfc(a.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename UncertainType>
UncertainType tgamma(const UncertainType& a) {
    auto func = [](decltype(a.mean()) x) { return std::tgamma(x); };
    auto dcda = numeric_derivative(func, a.mean());
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::tgamma(a.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename UncertainType>
UncertainType lgamma(const UncertainType& a) {
    auto func = [](decltype(a.mean()) x) { return std::lgamma(x); };
    auto dcda = numeric_derivative(func, a.mean());
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::lgamma(a.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

} // namespace sigma
