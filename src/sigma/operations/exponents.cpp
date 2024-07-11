#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

// -- Definitions --------------------------------------------------------------
template<typename UncertainType>
UncertainType pow(const UncertainType& u, double exp) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::pow(u.mean(), exp));
    c_setter.update_derivatives(exp * std::pow(u.mean(), exp - 1));
    return c;
}

template<typename UncertainType>
UncertainType pow(const UncertainType& u, const UncertainType& exp) {
    auto dxda = exp.mean() * std::pow(u.mean(), exp.mean() - 1);
    auto dxdb = std::log(u.mean()) * std::pow(u.mean(), exp.mean());
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::pow(u.mean(), exp.mean()));
    c_setter.update_derivatives(u.deps(), dxda, exp.deps(), dxdb);
    return c;
}

template<typename UncertainType>
UncertainType sqrt(const UncertainType& u) {
    return pow(std::move(u), 0.5);
}

template<typename UncertainType>
UncertainType exp(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::exp(u.mean()));
    c_setter.update_derivatives(std::exp(u.mean()));
    return c;
}

template<typename UncertainType>
UncertainType log(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::log(u.mean()));
    c_setter.update_derivatives(1.0 / u.mean());
    return c;
}

template<typename UncertainType>
UncertainType log10(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::log10(u.mean()));
    c_setter.update_derivatives(1.0 / (u.mean() * std::log(10.0)));
    return c;
}

// -- Explicit Instantiation ---------------------------------------------------
template UFloat pow<UFloat>(const UFloat&, double);
template UFloat pow<UFloat>(const UFloat&, const UFloat&);

template UDouble pow<UDouble>(const UDouble&, double);
template UDouble pow<UDouble>(const UDouble&, const UDouble&);

template UFloat sqrt<UFloat>(const UFloat&);
template UDouble sqrt<UDouble>(const UDouble&);

template UFloat exp<UFloat>(const UFloat&);
template UDouble exp<UDouble>(const UDouble&);

template UFloat log<UFloat>(const UFloat&);
template UDouble log<UDouble>(const UDouble&);

template UFloat log10<UFloat>(const UFloat&);
template UDouble log10<UDouble>(const UDouble&);

} // namespace sigma