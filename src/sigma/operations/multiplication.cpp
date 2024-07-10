#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType operator*(const UncertainType& lhs, const UncertainType& rhs) {
    auto dxda = rhs.mean();
    auto dxdb = lhs.mean();
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(lhs.mean() * rhs.mean());
    c_setter.update_derivatives(lhs.deps(), dxda, rhs.deps(), dxdb);
    return c;
}

template<typename UncertainType>
UncertainType operator*(const UncertainType& lhs, double rhs) {
    UncertainType c(lhs);
    Setter<UncertainType> c_setter(c);
    c_setter.update_mean(rhs * lhs.mean());
    c_setter.update_derivatives(rhs);
    return c;
}

template<typename UncertainType>
UncertainType operator*(double lhs, const UncertainType& rhs) {
    return rhs * lhs;
}

// Explicit Instantiation
template UFloat operator* <UFloat>(const UFloat&, const UFloat&);
template UFloat operator* <UFloat>(const UFloat&, double);
template UFloat operator* <UFloat>(double, const UFloat&);
template UDouble operator* <UDouble>(const UDouble&, const UDouble&);
template UDouble operator* <UDouble>(const UDouble&, double);
template UDouble operator* <UDouble>(double, const UDouble&);

} // namespace sigma