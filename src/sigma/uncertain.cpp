#include "sigma/uncertain.hpp"
#include <utility>

namespace sigma {

#define UNCERTAIN Uncertain<ValueType>

// -- Ctors --------------------------------------------------------------------

template<typename ValueType>
UNCERTAIN::Uncertain(value_t mean, value_t std) : m_mean_(mean), m_std_(std) {
    m_deps_.emplace(
      std::make_pair(std::make_shared<ind_var_t>(mean, std), 1.0));
}

// -- Operations ---------------------------------------------------------------

template<typename ValueType>
typename UNCERTAIN::my_t UNCERTAIN::operator-() const {
    my_t c(*this);
    c.m_mean_ = -mean();
    return c;
}

template<typename ValueType>
typename UNCERTAIN::my_t UNCERTAIN::operator+(const my_t& b) const {
    my_t c;
    c.m_mean_ = mean() + b.mean();
    c.update_dependencies(deps(), 1.0);
    c.update_dependencies(b.deps(), 1.0);
    c.calculate_std();
    return c;
}

template<typename ValueType>
typename UNCERTAIN::my_t UNCERTAIN::operator-(const my_t& b) const {
    my_t c;
    c.m_mean_ = mean() - b.mean();
    c.update_dependencies(deps(), 1.0);
    c.update_dependencies(b.deps(), -1.0);
    c.calculate_std();
    return c;
}

template<typename ValueType>
typename UNCERTAIN::my_t UNCERTAIN::operator*(const my_t& b) const {
    my_t c;
    c.m_mean_ = mean() * b.mean();
    c.update_dependencies(deps(), b.mean());
    c.update_dependencies(b.deps(), mean());
    c.calculate_std();
    return c;
}

template<typename ValueType>
typename UNCERTAIN::my_t UNCERTAIN::operator/(const my_t& b) const {
    my_t c;
    c.m_mean_ = mean() / b.mean();
    c.update_dependencies(deps(), 1.0 / b.mean());
    c.update_dependencies(b.deps(), -mean() / std::pow(b.mean(), 2.0));
    c.calculate_std();
    return c;
}

// -- Private Functions --------------------------------------------------------

template<typename ValueType>
void UNCERTAIN::update_dependency(ind_var_ptr var, value_t deriv) {
    if(m_deps_.count(var) != 0) {
        m_deps_[var] += deriv;
    } else {
        m_deps_.emplace(std::make_pair(std::move(var), deriv));
    }
}

template<typename ValueType>
void UNCERTAIN::update_dependencies(const deps_map_t& deps, value_t dydx) {
    for(const auto& [dep, deriv] : deps) {
        update_dependency(dep, dydx * deriv);
    }
}

template<typename ValueType>
void UNCERTAIN::calculate_std() {
    m_std_ = 0.0;
    for(const auto& [dep, deriv] : m_deps_) {
        m_std_ += std::pow(dep.get()->std() * deriv, 2.0);
    }
    m_std_ = std::sqrt(m_std_);
}

#undef UNCERTAIN

template class Uncertain<double>;

} // namespace sigma