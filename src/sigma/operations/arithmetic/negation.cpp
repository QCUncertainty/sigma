#include "../setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

// -- Definitions --------------------------------------------------------------
template<typename UncertainType>
UncertainType operator-(const UncertainType& u) {
    UncertainType c(u);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(-u.mean());
    c_setter.update_derivatives(-1.0);
    return c;
}

// -- Explicit Instantiation ---------------------------------------------------
template UFloat operator- <UFloat>(const UFloat&);
template UDouble operator- <UDouble>(const UDouble&);

} // namespace sigma