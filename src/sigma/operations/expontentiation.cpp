#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType pow(const UncertainType& u, double exp) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);

    c_setter.mean() = std::pow(u.mean(), exp);
    auto dydx       = exp * std::pow(u.mean(), exp - 1);
    for(const auto& [dep, deriv] : c.deps()) { c_setter.deps()[dep] *= dydx; }
    c_setter.calculate_std();
    return c;
}

// Explicit Instantiation
template UFloat pow<UFloat>(const UFloat&, double);
template UDouble pow<UDouble>(const UDouble&, double);

} // namespace sigma