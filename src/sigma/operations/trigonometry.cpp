#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>
#include <math.h>

namespace sigma {

// -- Definitions --------------------------------------------------------------
template<typename UncertainType>
UncertainType degrees(const UncertainType& u) {
    auto to_degrees = 180.0 / M_PI;
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(u.mean() * to_degrees);
    c_setter.update_derivatives(to_degrees);
    return c;
}

template<typename UncertainType>
UncertainType radians(const UncertainType& u) {
    auto to_radians = M_PI / 180.0;
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(u.mean() * to_radians);
    c_setter.update_derivatives(to_radians);
    return c;
}

template<typename UncertainType>
UncertainType sin(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::sin(u.mean()));
    c_setter.update_derivatives(std::cos(u.mean()));
    return c;
}

template<typename UncertainType>
UncertainType cos(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::cos(u.mean()));
    c_setter.update_derivatives(-std::sin(u.mean()));
    return c;
}

template<typename UncertainType>
UncertainType tan(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::tan(u.mean()));
    c_setter.update_derivatives(std::pow(std::tan(u.mean()), 2.0) + 1);
    return c;
}

template<typename UncertainType>
UncertainType asin(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::asin(u.mean()));
    c_setter.update_derivatives(1 / std::sqrt(1 - std::pow(u.mean(), 2)));
    return c;
}

template<typename UncertainType>
UncertainType acos(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::acos(u.mean()));
    c_setter.update_derivatives(-1 / std::sqrt(1 - std::pow(u.mean(), 2)));
    return c;
}

template<typename UncertainType>
UncertainType atan(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::atan(u.mean()));
    c_setter.update_derivatives(1 / (1 + std::pow(u.mean(), 2)));
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

// -- Explicit Instantiation ---------------------------------------------------
template UFloat degrees<UFloat>(const UFloat&);
template UDouble degrees<UDouble>(const UDouble&);

template UFloat radians<UFloat>(const UFloat&);
template UDouble radians<UDouble>(const UDouble&);

template UFloat sin<UFloat>(const UFloat&);
template UDouble sin<UDouble>(const UDouble&);

template UFloat cos<UFloat>(const UFloat&);
template UDouble cos<UDouble>(const UDouble&);

template UFloat tan<UFloat>(const UFloat&);
template UDouble tan<UDouble>(const UDouble&);

template UFloat asin<UFloat>(const UFloat&);
template UDouble asin<UDouble>(const UDouble&);

template UFloat acos<UFloat>(const UFloat&);
template UDouble acos<UDouble>(const UDouble&);

template UFloat atan<UFloat>(const UFloat&);
template UDouble atan<UDouble>(const UDouble&);

template UFloat atan2<UFloat>(const UFloat&, const UFloat&);
template UDouble atan2<UDouble>(const UDouble&, const UDouble&);

template UFloat atan2<UFloat>(const UFloat&, double);
template UDouble atan2<UDouble>(const UDouble&, double);

template UFloat atan2<UFloat>(double, const UFloat&);
template UDouble atan2<UDouble>(double, const UDouble&);

} // namespace sigma