#pragma once
#include "sigma/uncertain/uncertain.hpp"
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

    /// The type of a vector of dependencies of this variable
    using deps_vector_t = typename std::vector<dep_sd_ptr>;

    /** @brief Construct a Setter for a variable
     *
     *  @param u The uncertain variable *this will modify.
     *
     *  @throw none No throw guarantee
     */
    Setter(uncertain_t& u) : m_x_(u) {}

    /** @brief Update the mean of the wrapped variable
     *
     *  If the difference between the new mean and the current mean is below the
     *  zero threshold, the update is ignored. Otherwise, the mean is updated to
     *  the new value.
     *
     *  @param mean The new mean value of the variable
     *
     *  @throw none No throw guarantee
     */
    void update_mean(value_t mean) {
        if(std::abs(m_x_.m_mean_ - mean) >= m_x_.threshold())
            m_x_.m_mean_ = mean;
    }

    /** @brief Recalculate the standard deviation of the wrapped variable from
     *         its dependencies.
     *
     *  @throw none No throw guarantee
     */
    void recalculate_sd() {
        m_x_.m_sd_ = 0.0;
        for(const auto& [dep, deriv] : m_x_.m_deps_) {
            m_x_.m_sd_ += std::pow(*dep.get() * deriv, 2.0);
        }
        m_x_.m_sd_ = std::sqrt(m_x_.m_sd_);
    }

    /** @brief Update of existing derivatives
     *
     *  Updates the partial derivatives of the variable with respect to its
     *  dependencies, taking into account that some may be reduced below the
     *  zero threshold and need to be removed from the dependencies map. If so,
     *  the standard deviation will be updated accordingly.
     *
     *  @param dxda The partial derivative of the variable
     *
     *  @throw none No throw guarantee
     */
    void update_derivatives(value_t dxda) {
        // If the derivative is effectively zero, we can just clear the
        // dependencies and set the standard deviation to zero.
        if(std::abs(dxda) < m_x_.threshold() || dxda == 0.0) {
            m_x_.m_deps_.clear();
            m_x_.m_sd_ = 0.0;
            return;
        }
        // If the derivative is one, we don't need to do anything since the
        // derivatives are unchanged.
        if(dxda == 1.0) return;
        // Update the derivatives in place, tracking any that are reduced below
        // our threshold and will need to be removed from the dependencies map.
        m_removed_deps_.clear();
        for(const auto& [dep, deriv] : m_x_.m_deps_) {
            m_x_.m_deps_[dep] *= dxda;
            if(std::abs(m_x_.m_deps_[dep]) < m_x_.threshold()) {
                m_removed_deps_.emplace_back(dep);
            }
        }
        // If no dependencies need to be removed, we can just update the
        // standard deviation by multiplying by the absolute value of the
        // partial derivative. If the partial derivate is negative one, we don't
        // need to update the standard deviation since it is unchanged. If any
        // dependencies were reduced below our threshold, we need to remove them
        // from the dependencies map and recalculate the standard deviation.
        if(m_removed_deps_.size() == 0) {
            if(dxda != -1.0) m_x_.m_sd_ *= std::abs(dxda);
        } else {
            for(const auto& dep : m_removed_deps_) m_x_.m_deps_.erase(dep);
            recalculate_sd();
        }
    }

    /** @brief Update/addition of derivatives
     *
     *  Updates the map of dependencies and their derivatives with the provided
     *  map, while ignoring any new dependencies or contributions that are below
     *  the zero threshold. If any existing dependencies are reduced below the
     *  zero threshold, they will be removed from the dependencies map and the
     *  standard deviation will be updated accordingly.
     *
     *  @param deps The dependencies to update
     *  @param dxda The partial derivative of this variable with respect to
     *              the dependency
     *
     *  @throw none No throw guarantee
     */
    void update_derivatives(const deps_map_t& deps, value_t dxda) {
        // If the derivative is zero, we can skip the update since it won't
        // change anything.
        if(std::abs(dxda) < m_x_.threshold() || dxda == 0.0) { return; }
        // Update the map of contributions and their derivatives, keeping track
        // of any that are reduced to zero and will need to be removed. If the
        // uncertainty or contribution of a new dependency is below the
        // threshold, we can skip adding it to the map.
        m_removed_deps_.clear();
        size_t n_updated = 0;
        for(const auto& [dep, deriv] : deps) {
            if(m_x_.m_deps_.count(dep)) {
                m_x_.m_deps_[dep] += dxda * deriv;
                if(std::abs(m_x_.m_deps_[dep]) < m_x_.threshold() ||
                   m_x_.m_deps_[dep] == 0.0) {
                    m_removed_deps_.emplace_back(dep);
                }
                n_updated++;
            } else {
                if(std::abs(dxda * deriv) < m_x_.threshold()) continue;
                if(std::abs(*dep.get()) < m_x_.threshold()) continue;
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
            for(const auto& dep : m_removed_deps_) m_x_.m_deps_.erase(dep);
            recalculate_sd();
        } else {
            // Return to the variance so we can update the sum.
            m_x_.m_sd_ = std::pow(m_x_.m_sd_, 2.0);
            // Step through the updated dependencies and update the variance.
            // accordingly.
            for(const auto& [dep, deriv] : deps) {
                // Skip any dependencies that aren't in the map at this point.
                if(!m_x_.m_deps_.count(dep)) continue;
                // Calculate the previous value of the derivative for this
                // dependency.
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
            m_x_.m_sd_ = std::sqrt(m_x_.m_sd_);
            for(const auto& dep : m_removed_deps_) m_x_.m_deps_.erase(dep);
        }
    }

private:
    /// The variable being modified.
    uncertain_t& m_x_;

    /// Buffer for dependencies that are reduced to zero during an update, so we
    /// can remove them from the dependencies map.
    deps_vector_t m_removed_deps_{};
};

} // namespace sigma::detail_
