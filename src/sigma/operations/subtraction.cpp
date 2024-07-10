#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType operator-(const UncertainType& lhs, const UncertainType& rhs) {
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(lhs.mean() - rhs.mean());
    c_setter.update_derivatives(lhs.deps(), 1.0, rhs.deps(), -1.0);
    return c;
}

// Explicit Instantiation
template UFloat operator- <UFloat>(const UFloat&, const UFloat&);
template UDouble operator- <UDouble>(const UDouble&, const UDouble&);

} // namespace sigma