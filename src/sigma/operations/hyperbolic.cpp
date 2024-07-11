#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

// -- Definitions --------------------------------------------------------------
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

// -- Explicit Instantiation ---------------------------------------------------
template UFloat sinh<UFloat>(const UFloat&);
template UDouble sinh<UDouble>(const UDouble&);

template UFloat cosh<UFloat>(const UFloat&);
template UDouble cosh<UDouble>(const UDouble&);

template UFloat tanh<UFloat>(const UFloat&);
template UDouble tanh<UDouble>(const UDouble&);

template UFloat asinh<UFloat>(const UFloat&);
template UDouble asinh<UDouble>(const UDouble&);

template UFloat acosh<UFloat>(const UFloat&);
template UDouble acosh<UDouble>(const UDouble&);

template UFloat atanh<UFloat>(const UFloat&);
template UDouble atanh<UDouble>(const UDouble&);

} // namespace sigma