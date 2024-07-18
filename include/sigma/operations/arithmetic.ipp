#pragma once

#include "sigma/detail_/setter.hpp"

namespace sigma {

template<typename UncertainType>
UncertainType operator-(const UncertainType& a) {
    UncertainType c(a);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(-a.mean());
    c_setter.update_derivatives(-1.0);
    return c;
}

template<typename UncertainType>
UncertainType operator+(const UncertainType& lhs, const UncertainType& rhs) {
    UncertainType c(lhs);
    c += rhs;
    return c;
}

template<typename UncertainType>
UncertainType& operator+=(UncertainType& lhs, const UncertainType& rhs) {
    detail_::Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() + rhs.mean());
    lhs_setter.update_derivatives(rhs.deps(), 1.0);
    return lhs;
}

template<typename UncertainType>
UncertainType operator-(const UncertainType& lhs, const UncertainType& rhs) {
    UncertainType c(lhs);
    c -= rhs;
    return c;
}

template<typename UncertainType>
UncertainType operator*(const UncertainType& lhs, const UncertainType& rhs) {
    UncertainType c(lhs);
    c *= rhs;
    return c;
}

template<typename UncertainType>
UncertainType operator*(const UncertainType& lhs, double rhs) {
    UncertainType c(lhs);
    c *= rhs;
    return c;
}

template<typename UncertainType>
UncertainType operator*(double lhs, const UncertainType& rhs) {
    return rhs * lhs;
}

template<typename UncertainType>
UncertainType& operator*=(UncertainType& lhs, const UncertainType& rhs) {
    auto dcda = rhs.mean();
    auto dcdb = lhs.mean();
    detail_::Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() * rhs.mean());
    lhs_setter.update_derivatives(dcda, false);
    lhs_setter.update_derivatives(rhs.deps(), dcdb);
    return lhs;
}

template<typename UncertainType>
UncertainType& operator*=(UncertainType& lhs, double rhs) {
    detail_::Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() * rhs);
    lhs_setter.update_derivatives(rhs);
    return lhs;
}

template<typename UncertainType>
UncertainType& operator-=(UncertainType& lhs, const UncertainType& rhs) {
    detail_::Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() - rhs.mean());
    lhs_setter.update_derivatives(rhs.deps(), -1.0);
    return lhs;
}

template<typename UncertainType>
UncertainType operator/(const UncertainType& lhs, const UncertainType& rhs) {
    UncertainType c(lhs);
    c /= rhs;
    return c;
}

template<typename UncertainType>
UncertainType operator/(const UncertainType& lhs, double rhs) {
    UncertainType c(lhs);
    c /= rhs;
    return c;
}

template<typename UncertainType>
UncertainType operator/(double lhs, const UncertainType& rhs) {
    UncertainType c(rhs);
    detail_::Setter<UncertainType> c_setter(c);
    c_setter.update_mean(lhs / rhs.mean());
    c_setter.update_derivatives(-lhs / std::pow(rhs.mean(), 2.0));
    return c;
}

template<typename UncertainType>
UncertainType& operator/=(UncertainType& lhs, const UncertainType& rhs) {
    auto dcda = 1.0 / rhs.mean();
    auto dcdb = -lhs.mean() / std::pow(rhs.mean(), 2.0);
    detail_::Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() / rhs.mean());
    lhs_setter.update_derivatives(dcda, false);
    lhs_setter.update_derivatives(rhs.deps(), dcdb);
    return lhs;
}

template<typename UncertainType>
UncertainType& operator/=(UncertainType& lhs, double rhs) {
    detail_::Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() / rhs);
    lhs_setter.update_derivatives(1.0 / rhs);
    return lhs;
}

} // namespace sigma
