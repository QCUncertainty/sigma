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
    c_setter.update_mean(lhs.mean() * rhs);
    c_setter.update_derivatives(rhs);
    return c;
}

template<typename UncertainType>
UncertainType operator*(double lhs, const UncertainType& rhs) {
    return rhs * lhs;
}

template<typename UncertainType>
UncertainType& operator*=(UncertainType& lhs, const UncertainType& rhs) {
    auto dxda = rhs.mean();
    auto dxdb = lhs.mean();
    Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() * rhs.mean());
    lhs_setter.update_derivatives(dxda, false);
    lhs_setter.update_derivatives(rhs.deps(), dxdb);
    return lhs;
}

template<typename UncertainType>
UncertainType& operator*=(UncertainType& lhs, double rhs) {
    Setter<UncertainType> lhs_setter(lhs);
    lhs_setter.update_mean(lhs.mean() * rhs);
    lhs_setter.update_derivatives(rhs);
    return lhs;
}

// Explicit Instantiation
template UFloat operator* <UFloat>(const UFloat&, const UFloat&);
template UFloat operator* <UFloat>(const UFloat&, double);
template UFloat operator* <UFloat>(double, const UFloat&);
template UFloat& operator*= <UFloat>(UFloat&, const UFloat&);
template UFloat& operator*= <UFloat>(UFloat&, double);
template UDouble operator* <UDouble>(const UDouble&, const UDouble&);
template UDouble operator* <UDouble>(const UDouble&, double);
template UDouble operator* <UDouble>(double, const UDouble&);
template UDouble& operator*= <UDouble>(UDouble&, const UDouble&);
template UDouble& operator*= <UDouble>(UDouble&, double);


} // namespace sigma