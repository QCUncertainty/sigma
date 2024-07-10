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

// Explicit Instantiation
template UFloat pow<UFloat>(const UFloat&, double);
template UDouble pow<UDouble>(const UDouble&, double);

} // namespace sigma