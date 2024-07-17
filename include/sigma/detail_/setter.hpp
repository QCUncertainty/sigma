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
    Setter(uncertain_t& u) : m_x_(u) {}

    /** @brief Update the mean of the wrapped variable
     *
     *  @param mean The new mean value of the variable
     *
     *  @throw none No throw guarantee
     */
    void update_mean(value_t mean) { m_x_.m_mean_ = mean; }

    /** @brief Calculate the standatd deviation of m_x_ based on the
     *         uncertainty of its dependencies.
     *
     *  @param clean Whether or not to remove dependencies with partial
     *               derivatives of 0. Default is false.
     *
     *  @throw none No throw guarantee
     */
    void update_sd(bool clean = false) {
        m_x_.m_sd_ = 0.0;
        for(const auto& [dep, deriv] : m_x_.m_deps_) {
            if(deriv == 0.0) {
                if(clean) m_x_.m_deps_.erase(dep);
                continue;
            }
            m_x_.m_sd_ += std::pow(dep.get()->sd() * deriv, 2.0);
        }
        m_x_.m_sd_ = std::sqrt(m_x_.m_sd_);
    }

    /** @brief Update of existing derivatives
     *
     *  @param dxda The partial derivative of the variable
     *  @param call_update_std Whether or not to update the standard deviation
     *                         after updating the dependencies. Primarily for
     *                         minimizing repetitive work when multiple updates
     *                         will be performed
     *
     *  @throw none No throw guarantee
     */
    void update_derivatives(value_t dxda, bool call_update_std = true) {
        for(const auto& [dep, deriv] : m_x_.m_deps_) {
            m_x_.m_deps_[dep] *= dxda;
        }
        if(call_update_std) update_sd();
    }

    /** @brief Update/addition of derivatives
     *
     *  @param deps The dependencies to update
     *  @param dxda The partial derivative of this variable with respect to
     *              the dependency
     *  @param call_update_std Whether or not to update the standard deviation
     *                         after updating the dependencies. Primarily for
     *                         minimizing repetitive work when multiple updates
     *                         will be performed
     *
     *  @throw none No throw guarantee
     */
    void update_derivatives(const deps_map_t& deps, value_t dxda,
                            bool call_update_std = true) {
        for(const auto& [dep, deriv] : deps) {
            auto new_deriv = dxda * deriv;
            if(m_x_.m_deps_.count(dep) != 0) {
                m_x_.m_deps_[dep] += new_deriv;
            } else {
                m_x_.m_deps_.emplace(std::make_pair(std::move(dep), new_deriv));
            }
        }
        if(call_update_std) update_sd();
    }

    /** @brief Binary update/addition of derivatives
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
        update_derivatives(a_deps, dxda, false);
        update_derivatives(b_deps, dxdb, false);
        update_sd();
    }

private:
    /// The variable being modified
    uncertain_t& m_x_;
};

} // namespace sigma