#include "setter.hpp"
#include "sigma/operations/operations.hpp"
#include <cmath>

namespace sigma {

template<typename UncertainType>
UncertainType sqrt(const UncertainType& u) {
    return pow(std::move(u), 0.5);
}

// Explicit Instantiation
template UFloat sqrt<UFloat>(const UFloat&);
template UDouble sqrt<UDouble>(const UDouble&);

} // namespace sigma