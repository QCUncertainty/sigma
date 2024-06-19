#pragma once
#include <iostream>

namespace sigma {

template<typename ValueType>
class Uncertain {
public:
    using my_t      = Uncertain<ValueType>;
    using nominal_t = ValueType;
    using stddev_t  = ValueType;

    nominal_t m_nom_;
    stddev_t m_stddev_;

    Uncertain() noexcept = default;
    Uncertain(nominal_t nom, stddev_t std) : m_nom_(nom), m_stddev_(std) {};

    nominal_t nominal() const { return m_nom_; }
    stddev_t stddev() const { return m_stddev_; }
};

template<typename ValueType>
std::ostream& operator<<(std::ostream& os, const Uncertain<ValueType>& u) {
    os << u.nominal() << "+/-" << u.stddev();
    return os;
}

extern template class Uncertain<double>;

using UDouble = Uncertain<double>;

} // namespace sigma