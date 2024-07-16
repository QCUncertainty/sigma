#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>
#include <math.h>

namespace sigma {

// -- Numeric Derivative -------------------------------------------------------

template<typename FunctionType, typename NumericType>
NumericType numeric_derivative(FunctionType f, NumericType a) {
    // Step and step size chosen for consistency with uncertainties package
    auto step_size = std::sqrt(std::numeric_limits<float>::epsilon());
    auto step      = step_size * std::abs(a);
    auto a_plus    = f(a + step);
    auto a_minus   = f(a - step);
    auto dxda      = (a_plus - a_minus) / (2 * step);
    return dxda;
}

// -- Definitions --------------------------------------------------------------
template<typename UncertainType>
UncertainType erf(const UncertainType& u) {
    auto dxda = std::exp(-std::pow(u.mean(), 2)) * (2 / std::sqrt(M_PI));
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::erf(u.mean()));
    c_setter.update_derivatives(dxda);
    return c;
}

template<typename UncertainType>
UncertainType erfc(const UncertainType& u) {
    auto dxda = -std::exp(-std::pow(u.mean(), 2)) * (2 / std::sqrt(M_PI));
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::erfc(u.mean()));
    c_setter.update_derivatives(dxda);
    return c;
}

template<typename UncertainType>
UncertainType tgamma(const UncertainType& u) {
    auto func = [](decltype(u.mean()) a) { return std::tgamma(a); };
    auto dxda = numeric_derivative(func, u.mean());
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::tgamma(u.mean()));
    c_setter.update_derivatives(dxda);
    return c;
}

template<typename UncertainType>
UncertainType lgamma(const UncertainType& u) {
    auto func = [](decltype(u.mean()) a) { return std::lgamma(a); };
    auto dxda = numeric_derivative(func, u.mean());
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::lgamma(u.mean()));
    c_setter.update_derivatives(dxda);
    return c;
}

// -- Explicit Instantiation ---------------------------------------------------
template UFloat erf<UFloat>(const UFloat&);
template UDouble erf<UDouble>(const UDouble&);

template UFloat erfc<UFloat>(const UFloat&);
template UDouble erfc<UDouble>(const UDouble&);

template UFloat tgamma<UFloat>(const UFloat&);
template UDouble tgamma<UDouble>(const UDouble&);

template UFloat lgamma<UFloat>(const UFloat& u);
template UDouble lgamma<UDouble>(const UDouble& u);

} // namespace sigma