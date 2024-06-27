#pragma once
#include "independent_variable.hpp"
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

namespace sigma {

template<typename ValueType>
class Uncertain : public IndependentVariable<ValueType> {
public:
    /// Type of the instance
    using my_t = Uncertain<ValueType>;

    using value_t = ValueType;

    using ind_var_t = IndependentVariable<value_t>;

    using ind_var_ptr = std::shared_ptr<ind_var_t>;

    using deps_map_t = std::map<ind_var_ptr, value_t>;

    /// @brief Default ctor
    Uncertain() noexcept = default;

    Uncertain(value_t mean, value_t std) : m_mean_(mean), m_std_(std) {
        m_deps_.emplace(
          std::make_pair(std::make_shared<ind_var_t>(mean, std), 1.0));
    }

    /** @brief Get the mean value of the variable
     *
     *  @return The value of the mean
     *
     *  @throw none No throw guarantee
     */
    value_t mean() const { return m_mean_; }

    /** @brief Get the standard deviation of the variable
     *
     *  @return The value of the standard deviation
     *
     *  @throw none No throw guarantee
     */
    value_t std() const { return m_std_; }

    deps_map_t m_deps_;

private:
    /// Mean value of the variable
    value_t m_mean_;

    /// Standard deviation of the variable
    value_t m_std_;

    template<typename T>
    friend Uncertain<T> operator+(const Uncertain<T>&, const Uncertain<T>&);

}; // class Uncertain

template<typename ValueType>
Uncertain<ValueType> operator+(const Uncertain<ValueType>& a,
                               const Uncertain<ValueType>& b) {
    auto rv    = Uncertain<ValueType>();
    rv.m_mean_ = a.m_mean_ + b.m_mean_;
    for(const auto& [dep, contrib] : a.m_deps_) {
        rv.m_std_ += std::pow(dep.get()->std() * contrib, 2.0);
        rv.m_deps_.emplace(std::make_pair(dep, contrib));
    }
    for(const auto& [dep, contrib] : b.m_deps_) {
        rv.m_std_ += std::pow(dep.get()->std() * contrib, 2.0);
        if(rv.m_deps_.count(dep) != 0) {
            rv.m_deps_[dep] += contrib;
        } else {
            rv.m_deps_.emplace(std::make_pair(dep, contrib));
        }
    }
    rv.m_std_ = std::sqrt(rv.m_std_);
    return rv;
}

/** @relates Uncertain
 *  @brief Overload stream insertion to print uncertain variable
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param os The ostream to write to
 *  @param u The uncertain variable to write
 *
 *  @return The modified ostream instance
 *
 *  @throws std::ios_base::failure if anything goes wrong while writing.
 *          Weak throw guarantee.
 */
template<typename ValueType>
std::ostream& operator<<(std::ostream& os, const Uncertain<ValueType>& u) {
    os << u.mean() << "+/-" << u.std();
    return os;
}

extern template class Uncertain<double>;

using UDouble = Uncertain<double>;

} // namespace sigma