#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

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

// Explicit Instantiation
template UFloat pow<UFloat>(const UFloat&, double);
template UFloat pow<UFloat>(const UFloat&, const UFloat&);
template UDouble pow<UDouble>(const UDouble&, double);
template UDouble pow<UDouble>(const UDouble&, const UDouble&);

} // namespace sigma