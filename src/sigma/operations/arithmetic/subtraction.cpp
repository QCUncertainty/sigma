#include "../setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

// -- Definitions --------------------------------------------------------------
template<typename UncertainType>
UncertainType operator-(const UncertainType& lhs, const UncertainType& rhs) {
    UncertainType c(lhs);
    c -= rhs;
    return c;
}

template<typename UncertainType>
UncertainType& operator-=(UncertainType& lhs, const UncertainType& rhs) {
    Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() - rhs.mean());
    lhs_setter.update_derivatives(rhs.deps(), -1.0);
    return lhs;
}

// -- Explicit Instantiation ---------------------------------------------------
template UFloat operator- <UFloat>(const UFloat&, const UFloat&);
template UDouble operator- <UDouble>(const UDouble&, const UDouble&);

template UFloat& operator-= <UFloat>(UFloat&, const UFloat&);
template UDouble& operator-= <UDouble>(UDouble&, const UDouble&);

} // namespace sigma