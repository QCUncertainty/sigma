#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

// -- Definitions --------------------------------------------------------------
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

// -- Explicit Instantiation ---------------------------------------------------
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

} // namespace sigma