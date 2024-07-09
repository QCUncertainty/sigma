#pragma once
#include "independent_variable.hpp"
#include "sigma/uncertain.hpp"

namespace sigma {

/** @brief Modifies an unceratin variable.
 *
 *  This class provides a handle for operations to modify the private members
 *  of Uncertain instances.
 *
 *  @tparam UncertainType The type of the variable this will modify
 *
 */
template<typename UncertainType>
class Setter {
public:
    /// Type of the instance
    using my_t = Setter<UncertainType>;

    /// The numeric type of the variable
    using uncertain_t = UncertainType;

    /// The type of the values of the variable
    using value_t = typename uncertain_t::value_t;

    /// The type of a variable that this instance depends on
    using ind_var_t = typename uncertain_t::ind_var_t;

    /// A pointer to a dependency of this variable
    using ind_var_ptr = typename uncertain_t::ind_var_ptr;

    /// The type of the map holding the variable's dependencies
    using deps_map_t = typename uncertain_t::deps_map_t;

    /** @brief Construct a Setter for a variable
     *
     *  @param u The uncertain variable *this will modify.
     *
     *  @throw none No throw guarantee
     */
    Setter(uncertain_t& u) : m_u_(u) {}

    /** @brief Update the partial derivative of a dependency
     *
     *  Updates m_u_'s list of dependencies with the provided values. If
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
        if(m_u_.m_deps_.count(var) != 0) {
            m_u_.m_deps_[var] += deriv;
        } else {
            m_u_.m_deps_.emplace(std::make_pair(std::move(var), deriv));
        }
    }

    /** @brief Update the partial derivatives of a set of dependencies
     *
     *  Given a map @p deps of dependencies and their partial derivatives,
     *  updates each dependency of m_u_ with the partial derivatives scaled by
     *  @p dydx.
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

    /** @brief Calculate the standatd deviation of m_u_ based on the
     *         uncertainty of its dependencies.
     *
     *  @throw none No throw guarantee
     */
    void calculate_std() {
        m_u_.m_std_ = 0.0;
        for(const auto& [dep, deriv] : m_u_.m_deps_) {
            m_u_.m_std_ += std::pow(dep.get()->std() * deriv, 2.0);
        }
        m_u_.m_std_ = std::sqrt(m_u_.m_std_);
    }

    /** @brief Access the mean of the wrapped variable
     *
     *  @throw none No throw guarantee
     */
    value_t& mean() { return m_u_.m_mean_; }

    /** @brief Access the mean of the dependencies of the wrapped variable
     *
     *  @throw none No throw guarantee
     */
    deps_map_t& deps() { return m_u_.m_deps_; }

private:
    uncertain_t& m_u_;
};

} // namespace sigma