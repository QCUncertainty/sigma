#pragma once
#include "sigma/uncertain.hpp"
#include <vector>

/** @file setter.hpp
 *  @brief Defines the Setter class
 */

namespace sigma::detail_ {

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

    /// The type of a standard deviation that this instance depends on
    using dep_sd_t = typename uncertain_t::dep_sd_t;

    /// A pointer to a dependency of this variable
    using dep_sd_ptr = typename uncertain_t::dep_sd_ptr;

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

    /** @brief Update of existing derivatives
     *
     *  @param dxda The partial derivative of the variable
     *
     *  @throw none No throw guarantee
     */
    void update_derivatives(value_t dxda) {
        // If the derivative is one, we don't need to do anything since the
        // derivatives are unchanged
        if(dxda == 1.0) return;
        // If the derivative is zero, we can just clear the dependencies and set
        // the standard deviation to zero
        if(dxda == 0.0) {
            m_x_.m_deps_.clear();
            m_x_.m_sd_ = 0.0;
            return;
        }
        // Update the derivatives in place
        for(const auto& [dep, deriv] : m_x_.m_deps_) {
            m_x_.m_deps_[dep] *= dxda;
        }
        // If the derivative is not negative one, we need to update the
        // standard deviation as well.
        if(dxda != -1.0) { m_x_.m_sd_ *= std::abs(dxda); }
    }

    /** @brief Update/addition of derivatives
     *
     *  @param deps The dependencies to update
     *  @param dxda The partial derivative of this variable with respect to
     *              the dependency
     *
     *  @throw none No throw guarantee
     */
    void update_derivatives(const deps_map_t& deps, value_t dxda) {
        // If the derivative is zero, we can skip the update since it won't
        // change anything
        if(dxda == 0.0) { return; }

        // Update the map of contributions and their derivatives, keeping track
        // of any that are reduced to zero and will need to be removed.
        std::vector<dep_sd_ptr> zero_contributions{};
        size_t n_updated = 0;
        for(const auto& [dep, deriv] : deps) {
            if(m_x_.m_deps_.count(dep)) {
                m_x_.m_deps_[dep] += dxda * deriv;
                if(m_x_.m_deps_[dep] == 0.0) {
                    zero_contributions.emplace_back(dep);
                }
                n_updated++;
            } else {
                m_x_.m_deps_.emplace(dep, dxda * deriv);
            }
        }

        // If more than half of the existing dependencies are being updated,
        // it's more efficient to update the derivatives and then recalculate
        // the standard deviation from scratch. Otherwise, we can update the
        // standard deviation in place by removing the contribution of the
        // zeroed and updated dependencies and then adding the new contribution
        // of the updated dependencies.
        if(n_updated > (m_x_.m_deps_.size() / 2)) {
            // Remove from m_deps_ any dependency that was reduced to zero.
            for(const auto& dep : zero_contributions) {
                m_x_.m_deps_.erase(dep);
            }

            // Recalculate the variance.
            m_x_.m_sd_ = 0.0;
            for(const auto& [dep, deriv] : m_x_.m_deps_) {
                m_x_.m_sd_ += std::pow(*dep.get() * deriv, 2.0);
            }
        } else {
            // Return to the variance so we can update the sum.
            m_x_.m_sd_ = std::pow(m_x_.m_sd_, 2.0);

            // Step through the updated dependencies and update the variance
            // accordingly.
            for(const auto& [dep, deriv] : deps) {
                auto old_deriv = m_x_.m_deps_[dep] - dxda * deriv;
                // As long as the dependency hasn't been reduce to zero, we need
                // to add its contribution to the variance.
                if(m_x_.m_deps_[dep] != 0.0) {
                    m_x_.m_sd_ += std::pow(*dep.get() * m_x_.m_deps_[dep], 2.0);
                }
                // As long as the dependency isn't new, we need to remove its
                // previous contribution to the variance.
                if(old_deriv != 0.0) {
                    m_x_.m_sd_ -= std::pow(*dep.get() * old_deriv, 2.0);
                }
            }

            // Remove from m_deps_ any dependency that was reduced to zero.
            for(const auto& dep : zero_contributions) {
                m_x_.m_deps_.erase(dep);
            }
        }
        // Take the square root of the variance to get the standard deviation.
        m_x_.m_sd_ = std::sqrt(m_x_.m_sd_);
    }

private:
    /// The variable being modified
    uncertain_t& m_x_;
};

} // namespace sigma::detail_
