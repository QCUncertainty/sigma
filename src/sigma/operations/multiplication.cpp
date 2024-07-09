#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType operator*(const UncertainType& lhs, const UncertainType& rhs) {
    UncertainType c;
    Setter<UncertainType> c_setter(c);
    c_setter.mean() = lhs.mean() * rhs.mean();
    c_setter.update_dependencies(lhs.deps(), rhs.mean());
    c_setter.update_dependencies(rhs.deps(), lhs.mean());
    c_setter.calculate_std();
    return c;
}

template<typename UncertainType>
UncertainType operator*(const UncertainType& lhs, double rhs) {
    UncertainType c(lhs);
    Setter<UncertainType> c_setter(c);

    c_setter.mean() = rhs * lhs.mean();
    for(const auto& [dep, deriv] : c.deps()) { c_setter.deps()[dep] *= rhs; }
    c_setter.calculate_std();
    return c;
}

template<typename UncertainType>
UncertainType operator*(double lhs, const UncertainType& rhs) {
    return rhs * lhs;
}

// Explicit Instantiation
template UFloat operator* <UFloat>(const UFloat&, const UFloat&);
template UDouble operator* <UDouble>(const UDouble&, const UDouble&);
template UFloat operator* <UFloat>(const UFloat&, double);
template UDouble operator* <UDouble>(const UDouble&, double);
template UFloat operator* <UFloat>(double, const UFloat&);
template UDouble operator* <UDouble>(double, const UDouble&);

} // namespace sigma