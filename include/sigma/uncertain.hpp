#pragma once
#include "independent_variable.hpp"
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <utility>

namespace sigma {

/** @brief Models an unceratin variable.
 *
 *  In this context, an uncertain variable is one that is dependent on one or
 *  more independent variables. It has a mean value and standard deviation. This
 *  class also keeps track of the variables that each instance is dependent
 *  upon and the contribution of that variable to the uncertainty of this
 *  instance.
 *
 *  @tparam ValueType The type of the value and standard deviation
 *
 */
template<typename ValueType>
class Uncertain {
public:
    /// Type of the instance
    using my_t = Uncertain<ValueType>;

    /// The numeric type of the variable
    using value_t = ValueType;

    /// The type of a variable that this instance depends on
    using ind_var_t = IndependentVariable<value_t>;

    /// A pointer to a dependency of this variable
    using ind_var_ptr = std::shared_ptr<ind_var_t>;

    /// A map of dependencies and their contributions to the uncertainty
    using deps_map_t = std::map<ind_var_ptr, value_t>;

    /// @brief Default ctor
    Uncertain() noexcept = default;

    /** @brief Construct an uncertain value from mean and standard deviation
     *
     *  Effectively, this creates a value that is a function of a single
     *  independent variable.
     *
     *  @param mean The average value of the variable
     *  @param std The standard deviation of the variable
     *
     *  @throw none No throw guarantee
     */
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
     *  @return The dependencies map
     *
     *  @throw none No throw guarantee
     */
    const deps_map_t& deps() const { return m_deps_; }

    /** @brief Negation Operation
     *
     *  @return A copy of this variable, but with the sign of the mean reversed
     *
     *  @throw none No throw guarantee
     */
    my_t operator-() const {
        my_t c(*this);
        c.m_mean_ = -mean();
        return c;
    }

    /** @brief Addition Operation
     *
     *  @return A variable that is the sum of *this and @p b
     *
     *  @throw none No throw guarantee
     */
    my_t operator+(const my_t& b) const {
        my_t c;
        c.m_mean_ = mean() + b.mean();
        c.update_dependencies(deps(), 1.0);
        c.update_dependencies(b.deps(), 1.0);
        c.calculate_std();
        return c;
    }

    /** @brief Subtraction Operation
     *
     *  @return A variable that is the difference of *this and @p b
     *
     *  @throw none No throw guarantee
     */
    my_t operator-(const my_t& b) const {
        my_t c;
        c.m_mean_ = mean() - b.mean();
        c.update_dependencies(deps(), 1.0);
        c.update_dependencies(b.deps(), -1.0);
        c.calculate_std();
        return c;
    }

    /** @brief Multiplication Operation
     *
     *  @return A variable that is the product of *this and @p b
     *
     *  @throw none No throw guarantee
     */
    my_t operator*(const my_t& b) const {
        my_t c;
        c.m_mean_ = mean() * b.mean();
        c.update_dependencies(deps(), b.mean());
        c.update_dependencies(b.deps(), mean());
        c.calculate_std();
        return c;
    }

    /** @brief Division Operation
     *
     *  @return A variable that is the quotient of *this and @p b
     *
     *  @throw none No throw guarantee
     */
    my_t operator/(const my_t& b) const {
        my_t c;
        c.m_mean_ = mean() / b.mean();
        c.update_dependencies(deps(), 1.0 / b.mean());
        c.update_dependencies(b.deps(), -mean() / std::pow(b.mean(), 2.0));
        c.calculate_std();
        return c;
    }

    /** @brief Scale Operation
     *
     *  @return A variable that is *this scaled by @p v
     *
     *  @throw none No throw guarantee
     */
    // TODO: Work this out
    // my_t operator*(value_t v) const {
    //     my_t c(*this);
    //     c.m_mean_ = v * mean();
    //     c.update_dependencies(deps(), 1.0 / b.mean());
    //     return c;
    // }

private:
    /// Mean value of the variable
    value_t m_mean_;

    /// Standard deviation of the variable
    value_t m_std_;

    /** Map of the variables this value is dependent on to their partial
     *  derivatives with respect to this value
     */
    deps_map_t m_deps_ = {};

    /** @brief Update the partial derivative of a dependency
     *
     *  Updates this variables list of dependencies with the provided values. If
     *  @p var is already in the list of dependencies, the value of its partial
     *  derivative is updated with the addition of @p deriv. Otherwise, @p var
     *  is added to the list of dependencies with @p deriv as its partial
     *  derivative.
     *
     *  @param var The dependency to update
     *  @param deriv The partial derivative of this variable with respect to
     *               the dependency
     *
     *  @throw none No throw guarantee
     */
    void update_dependency(ind_var_ptr var, value_t deriv) {
        if(m_deps_.count(var) != 0) {
            m_deps_[var] += deriv;
        } else {
            m_deps_.emplace(std::make_pair(std::move(var), deriv));
        }
    }

    /** @brief Update the partial derivatives of a set of dependencies
     *
     *  Given a map @p deps of dependencies and their partial derivatives,
     *  updates each dependency of this variable with the partial derivatives
     *  scaled by @p dydx.
     *
     *  @param deps The dependencies to update
     *  @param dydx The partial derivative of this variable with respect to
     *              the dependency
     *
     *  @throw none No throw guarantee
     */
    void update_dependencies(const deps_map_t& deps, value_t dydx) {
        for(const auto& [dep, deriv] : deps) {
            update_dependency(dep, dydx * deriv);
        }
    }

    /** @brief Calculate the standatd deviation of this variables based on the
     *         uncertainty of its dependencies.
     *
     *  @throw none No throw guarantee
     */
    void calculate_std() {
        m_std_ = 0.0;
        for(const auto& [dep, deriv] : m_deps_) {
            m_std_ += std::pow(dep.get()->std() * deriv, 2.0);
        }
        m_std_ = std::sqrt(m_std_);
    }

}; // class Uncertain

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

/// Typedef for an uncertain double
using UDouble = Uncertain<double>;

} // namespace sigma