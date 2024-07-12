#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

// -- Definitions --------------------------------------------------------------
template<typename UncertainType>
UncertainType abs(const UncertainType& u) {
    auto dxda = (u.mean() >= 0) ? 1.0 : -1.0;
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::abs(u.mean()));
    c_setter.update_derivatives(dxda);
    return c;
}

template<typename UncertainType>
UncertainType ceil(const UncertainType& u) {
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::ceil(u.mean()));
    return c;
}

template<typename UncertainType>
UncertainType floor(const UncertainType& u) {
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::floor(u.mean()));
    return c;
}

template<typename UncertainType>
UncertainType fmod(const UncertainType& a, const UncertainType& b) {
    auto dxda = 1.0;
    auto dxdb = -std::floor(a.mean() / b.mean());
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::fmod(a.mean(), b.mean()));
    c_setter.update_derivatives(dxda, false);
    c_setter.update_derivatives(b.deps(), dxdb);
    return c;
}

template<typename UncertainType>
UncertainType fmod(const UncertainType& a, double b) {
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::fmod(a.mean(), b));
    c_setter.update_derivatives(1.0);
    return c;
}

template<typename UncertainType>
UncertainType fmod(double a, const UncertainType& b) {
    UncertainType c(b);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::fmod(a, b.mean()));
    c_setter.update_derivatives(-std::floor(a / b.mean()));
    return c;
}

// -- Explicit Instantiation ---------------------------------------------------
template UFloat abs<UFloat>(const UFloat&);
template UDouble abs<UDouble>(const UDouble&);

template UFloat ceil<UFloat>(const UFloat&);
template UDouble ceil<UDouble>(const UDouble&);

template UFloat floor<UFloat>(const UFloat&);
template UDouble floor<UDouble>(const UDouble&);

template UFloat fmod<UFloat>(const UFloat& a, const UFloat& b);
template UDouble fmod<UDouble>(const UDouble& a, const UDouble& b);

template UFloat fmod<UFloat>(const UFloat& a, double b);
template UDouble fmod<UDouble>(const UDouble& a, double b);

template UFloat fmod<UFloat>(double a, const UFloat& b);
template UDouble fmod<UDouble>(double a, const UDouble& b);

} // namespace sigma