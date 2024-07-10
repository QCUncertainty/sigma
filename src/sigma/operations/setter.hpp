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

    /** @brief Update the mean of the wrapped variable
     *
     *  @param mean The new mean value of the variable
     *
     *  @throw none No throw guarantee
     */
    void update_mean(value_t mean) { m_u_.m_mean_ = mean; }

    /** @brief Unary update of derivatives
     *
     *  @param dxda The partial derivative of the variable
     *
     *  @throw none No throw guarantee
     */
    void update_derivatives(value_t dxda) {
        for(const auto& [dep, deriv] : m_u_.m_deps_) {
            m_u_.m_deps_[dep] *= dxda;
        }
        update_std();
    }

    /** @brief Binary update of derivatives
     *
     *  @param a_deps The dependencies of a
     *  @param dxda The partial derivative of the variable with respect to a
     *  @param b_deps The dependencies of b
     *  @param dxdb The partial derivative of the variable with respect to b
     *
     *  @throw none No throw guarantee
     */
    void update_derivatives(const deps_map_t& a_deps, value_t dxda,
                            const deps_map_t& b_deps, value_t dxdb) {
        update_dependencies(a_deps, dxda);
        update_dependencies(b_deps, dxdb);
        update_std();
    }

private:
    /// The variable being modified
    uncertain_t& m_u_;

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
            auto new_deriv = dydx * deriv;
            if(m_u_.m_deps_.count(dep) != 0) {
                m_u_.m_deps_[dep] += new_deriv;
            } else {
                m_u_.m_deps_.emplace(std::make_pair(std::move(dep), new_deriv));
            }
        }
    }

    /** @brief Calculate the standatd deviation of m_u_ based on the
     *         uncertainty of its dependencies.
     *
     *  @throw none No throw guarantee
     */
    void update_std() {
        m_u_.m_std_ = 0.0;
        for(const auto& [dep, deriv] : m_u_.m_deps_) {
            m_u_.m_std_ += std::pow(dep.get()->std() * deriv, 2.0);
        }
        m_u_.m_std_ = std::sqrt(m_u_.m_std_);
    }
};

} // namespace sigma