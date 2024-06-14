#pragma once
#include <iostream>

namespace sigma {

template<typename ValueType>
class Uncertain {
public:
    using my_t      = Uncertain<ValueType>;
    using nominal_t = ValueType;
    using stddev_t  = ValueType;

    nominal_t nominal;
    stddev_t stddev;

    Uncertain() noexcept = default;
    Uncertain(nominal_t nom, stddev_t std) : nominal(nom), stddev(std){};
};

template<typename ValueType>
std::ostream& operator<<(std::ostream& os, const Uncertain<ValueType>& u) {
    os << u.nominal << "+/-" << u.stddev;
    return os;
}

extern template class Uncertain<double>;

using UDouble = Uncertain<double>;

} // namespace sigma