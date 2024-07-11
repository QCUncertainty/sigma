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

// -- Explicit Instantiation ---------------------------------------------------
template UFloat abs<UFloat>(const UFloat&);
template UDouble abs<UDouble>(const UDouble&);

} // namespace sigma