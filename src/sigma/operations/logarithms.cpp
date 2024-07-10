#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

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

// Explicit Instantiation
template UFloat log<UFloat>(const UFloat&);
template UDouble log<UDouble>(const UDouble&);
template UFloat log10<UFloat>(const UFloat&);
template UDouble log10<UDouble>(const UDouble&);

} // namespace sigma