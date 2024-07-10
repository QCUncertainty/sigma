#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType exp(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(std::exp(u.mean()));
    c_setter.update_derivatives(std::exp(u.mean()));
    return c;
}

// Explicit Instantiation
template UFloat exp<UFloat>(const UFloat&);
template UDouble exp<UDouble>(const UDouble&);

} // namespace sigma