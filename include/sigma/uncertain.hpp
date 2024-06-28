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

    /** @brief Get the dependencies of the variable
     *
     *  @return The dependencies
     *
     *  @throw none No throw guarantee
     */
    const deps_map_t& deps() const { return m_deps_; }
    

private:
    /// Mean value of the variable
    value_t m_mean_;

    /// Standard deviation of the variable
    value_t m_std_;

    /** Map of the variables this value is dependent on to their partial
     *  derivatives with respect to this value
     */
    deps_map_t m_deps_ = {};

    Uncertain(value_t mean) : m_mean_(mean) {}

    void update_dependency(ind_var_ptr var, value_t deriv) {
        if(m_deps_.count(var) != 0) {
            m_deps_[var] += deriv;
        } else {
            m_deps_.emplace(std::make_pair(std::move(var), deriv));
        }
    }

    void update_dependencies(const deps_map_t& deps, value_t dydx) {
        for(const auto& [dep, deriv] : deps) {
            update_dependency(dep, dydx * deriv);
        }
    }

    void calculate_std() {
        m_std_ = 0.0;
        for(const auto& [dep, deriv] : m_deps_) {
            m_std_ += std::pow(dep.get()->std() * deriv, 2.0);
        }
        m_std_ = std::sqrt(m_std_);
    }

    template<typename T>
    friend Uncertain<T> operator+(const Uncertain<T>&, const Uncertain<T>&);
    template<typename T>
    friend Uncertain<T> operator-(const Uncertain<T>&, const Uncertain<T>&);
    template<typename T>
    friend Uncertain<T> operator*(const Uncertain<T>&, const Uncertain<T>&);
    template<typename T>
    friend Uncertain<T> operator/(const Uncertain<T>&, const Uncertain<T>&);

}; // class Uncertain

template<typename ValueType>
Uncertain<ValueType> operator+(const Uncertain<ValueType>& a,
                               const Uncertain<ValueType>& b) {
    Uncertain<ValueType> c(a.m_mean_ + b.m_mean_);
    c.update_dependencies(a.m_deps_, 1.0);
    c.update_dependencies(b.m_deps_, 1.0);
    c.calculate_std();
    return c;
}

template<typename ValueType>
Uncertain<ValueType> operator-(const Uncertain<ValueType>& a,
                               const Uncertain<ValueType>& b) {
    Uncertain<ValueType> c(a.m_mean_ - b.m_mean_);
    c.update_dependencies(a.m_deps_, 1.0);
    c.update_dependencies(b.m_deps_, -1.0);
    c.calculate_std();
    return c;
}

template<typename ValueType>
Uncertain<ValueType> operator*(const Uncertain<ValueType>& a,
                               const Uncertain<ValueType>& b) {
    Uncertain<ValueType> c(a.m_mean_ * b.m_mean_);
    c.update_dependencies(a.m_deps_, b.m_mean_);
    c.update_dependencies(b.m_deps_, a.m_mean_);
    c.calculate_std();
    return c;
}

template<typename ValueType>
Uncertain<ValueType> operator/(const Uncertain<ValueType>& a,
                               const Uncertain<ValueType>& b) {
    Uncertain<ValueType> c(a.m_mean_ / b.m_mean_);
    c.update_dependencies(a.m_deps_, 1.0 / b.m_mean_);
    c.update_dependencies(b.m_deps_, -a.m_mean_ / std::pow(b.m_mean_, 2.0));
    c.calculate_std();
    return c;
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