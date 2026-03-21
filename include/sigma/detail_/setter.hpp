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

        // We need to determine which dependencies are new, which are zeroed
        // out, and which are updated. We can then update the standard deviation
        // accordingly.
        std::vector<dep_sd_ptr> zero_contributions{};
        std::vector<dep_sd_ptr> new_contributions{};
        std::vector<dep_sd_ptr> updated_contributions{};
        for(const auto& [dep, deriv] : deps) {
            if(m_x_.m_deps_.count(dep) == 0) {
                new_contributions.push_back(dep);
            } else {
                if(m_x_.m_deps_[dep] + (dxda * deriv) == 0.0) {
                    zero_contributions.push_back(dep);
                } else {
                    updated_contributions.push_back(dep);
                }
            }
        }

        // If more than half of the existing dependencies are being updated,
        // it's more efficient to update the derivatives and then recalculate
        // the standard deviation from scratch. Otherwise, we can update the
        // standard deviation in place by removing the contribution of the
        // zeroed and updated dependencies and then adding the new contribution
        // of the updated dependencies.
        if(updated_contributions.size() > (m_x_.m_deps_.size() / 2)) {
            for(const auto& dep : zero_contributions) {
                m_x_.m_deps_.erase(dep);
            }
            for(const auto& dep : updated_contributions) {
                m_x_.m_deps_[dep] += dxda * deps.at(dep);
            }
            m_x_.m_sd_ = 0.0;
            for(const auto& [dep, deriv] : m_x_.m_deps_) {
                m_x_.m_sd_ += std::pow(*dep.get() * deriv, 2.0);
            }
        } else {
            m_x_.m_sd_ = std::pow(m_x_.m_sd_, 2.0);
            for(const auto& dep : zero_contributions) {
                m_x_.m_sd_ -= std::pow(*dep.get() * m_x_.m_deps_[dep], 2.0);
                m_x_.m_deps_.erase(dep);
            }
            // While removing the contribution of the zeroed dependencies, we
            // can get minor numerical variations that cause the standard
            // deviation to become small and negative if the variable is now
            // independent of all other variables. In this case, we can just set
            // it to zero.
            if(m_x_.m_deps_.size() == 0) { m_x_.m_sd_ = 0.0; }
            for(const auto& dep : updated_contributions) {
                m_x_.m_sd_ -= std::pow(*dep.get() * m_x_.m_deps_[dep], 2.0);
                m_x_.m_deps_[dep] += dxda * deps.at(dep);
                m_x_.m_sd_ += std::pow(*dep.get() * m_x_.m_deps_[dep], 2.0);
            }
        }
        // Finally, we need to add the contributions of the new dependencies to
        // the standard deviation and take the square root.
        for(const auto& dep : new_contributions) {
            m_x_.m_deps_.emplace(std::make_pair(dep, dxda * deps.at(dep)));
            m_x_.m_sd_ += std::pow(*dep.get() * m_x_.m_deps_[dep], 2.0);
        }
        m_x_.m_sd_ = std::sqrt(m_x_.m_sd_);
    }

private:
    /// The variable being modified
    uncertain_t& m_x_;
};

} // namespace sigma::detail_
