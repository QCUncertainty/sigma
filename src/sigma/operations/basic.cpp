#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

// -- Definitions --------------------------------------------------------------
template<typename UncertainType>
UncertainType abs(const UncertainType& a) {
    auto dcda = (a.mean() >= 0) ? 1.0 : -1.0;
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::abs(a.mean()));
    c_setter.update_derivatives(dcda);
    return c;
}

template<typename UncertainType>
UncertainType ceil(const UncertainType& a) {
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::ceil(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType floor(const UncertainType& a) {
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::floor(a.mean()));
    return c;
}

template<typename UncertainType>
UncertainType fmod(const UncertainType& a, const UncertainType& b) {
    auto dcda = 1.0;
    auto dcdb = -std::floor(a.mean() / b.mean());
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::fmod(a.mean(), b.mean()));
    c_setter.update_derivatives(dcda, false);
    c_setter.update_derivatives(b.deps(), dcdb);
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

template<typename UncertainType>
UncertainType copysign(const UncertainType& a, const UncertainType& b) {
    return copysign(a, b.mean());
}

template<typename UncertainType>
UncertainType copysign(const UncertainType& a, double b) {
    auto b_sign = std::copysign(1.0, b);
    auto dcda   = (a.mean() >= 0) ? b_sign : -b_sign;
    UncertainType c(a);
    Setter<UncertainType> c_setter(c);
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
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::trunc(a.mean()));
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

template UFloat copysign<UFloat>(const UFloat& a, const UFloat& b);
template UDouble copysign<UDouble>(const UDouble& a, const UDouble& b);

template UFloat copysign<UFloat>(const UFloat& a, double b);
template UDouble copysign<UDouble>(const UDouble& a, double b);

template double copysign<UFloat>(double a, const UFloat& b);
template double copysign<UDouble>(double a, const UDouble& b);

template UFloat trunc<UFloat>(const UFloat&);
template UDouble trunc<UDouble>(const UDouble&);

} // namespace sigma