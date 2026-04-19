#pragma once
#include <iomanip>
#include <sigma/interval.hpp>
#include <sstream>

namespace sigma {

/** @brief An interval that tracks contributions from dependencies.
 *
 *  @tparam IntervalType The type of the interval used internally.
 *
 * This class is adapted from Hassan El-Owny's work:
 * http://www2.math.uni-wuppertal.de/org/WRST/xsc-frame/preprints/prep_06_2.pdf.
 *
 * The Hansen form of the generalized interval [x] is given by:
 * @f{
 * [x] = [c] + [-1,1] * \sum_{i}^n r_i * v_i
 * @f}
 *
 * The idea here is that we are trying to bound a scalar-valued function
 * @f$f(x)@f$ where the input @f$x@f$ is a vector in an @f$n@f$-dimensional
 * space. The general interval then tracks the contributions to the final
 * interval in terms of which dimension they are associated with. For example,
 * in @f$\mathbb{R}^2@f$, every interval can be written as a linear combination
 * of an interval associated with the x-dimension and an interval associated
 * with the y-dimension. The general interval will track the x and y
 * contributions separately to avoid, say an x-contribution affecting an
 * interval in the y-dimension. In their work, Hassan assumes that @f$n@f$ is
 * fixed and known ahead of time for the problem at hand. In this case, the user
 * is responsible for decomposing the initial intervals into their respective
 * dimensions. Once that's done, the general interval will automatically
 * propagate the interval.
 *
 * Instead of requiring the user to manually decompose inputs into their
 * components, we use the same dependency tracking mechanism as the Uncertain
 * class, i.e., each GeneralInterval is assumed to represent an independent
 * dimension unless the user creates it by copying another GeneralInterval (in
 * which case the copy will be associated with the same dimensions as the
 * original).
 *
 * I'm a little unsure of the math at the moment and have coded the center and
 * weights as intervals, like Hassan does. I have yet to see a scenario where
 * the center has a non-zero radius. While the weights start as intevals of
 * zero-radius, they will acquire a radius after operations like multiplication
 * and division are performed. Furthermore, they will in general become non-
 * symmetric intervals meaning attempting to store only the absolute value of
 * the weights will not suffice.
 */
template<typename IntervalType>
class GeneralInterval {
public:
    using interval_t       = IntervalType;
    using value_t          = typename interval_t::value_t;
    using dep_radius_ptr   = std::shared_ptr<value_t>;
    using dep_radius_map_t = std::unordered_map<dep_radius_ptr, interval_t>;
    using size_type        = typename dep_radius_map_t::size_type;

    GeneralInterval() {}
    GeneralInterval(const value_t& value) : GeneralInterval(value, value) {}
    GeneralInterval(const value_t& low, const value_t& high) :
      GeneralInterval(interval_t(low, high)) {}
    GeneralInterval(const interval_t&);
    GeneralInterval(const interval_t& center, const dep_radius_map_t& dep,
                    const dep_radius_map_t& gradient);
    GeneralInterval(const GeneralInterval& other) = default;

    const auto& center() const { return m_midpoint_; }
    const auto& weights() const { return m_weights_; }
    bool contains(const value_t& value) const {
        return as_interval().contains(value);
    }

    const auto& gradient() const { return m_gradient_; }
    interval_t derivative(dep_radius_ptr radius) const {
        return m_gradient_.count(radius) ? m_gradient_.at(radius) :
                                           interval_t(0.0);
    }

    interval_t as_interval() const { return compute_interval_(); }

    std::string print_hansen_form() const;
    std::string print_gradient() const;
    std::string print_interval_form() const {
        return as_interval().print_interval_form();
    }

    GeneralInterval operator-() const {
        GeneralInterval result(*this);
        result *= -1.0;
        return result;
    }

    // -- GeneralInterval with GeneralInterval Arithmetic --
    GeneralInterval& operator+=(const GeneralInterval& other);
    GeneralInterval& operator-=(const GeneralInterval& other);
    GeneralInterval& operator*=(const GeneralInterval& other);
    GeneralInterval& operator/=(const GeneralInterval& other);

    GeneralInterval operator+(const GeneralInterval& other) const {
        GeneralInterval result(*this);
        result += other;
        return result;
    }

    GeneralInterval operator-(const GeneralInterval& other) const {
        GeneralInterval result(*this);
        result -= other;
        return result;
    }

    GeneralInterval operator*(const GeneralInterval& other) const {
        GeneralInterval result(*this);
        result *= other;
        return result;
    }

    GeneralInterval operator/(const GeneralInterval& other) const {
        GeneralInterval result(*this);
        result /= other;
        return result;
    }

    // -- GeneralInterval with value_t Arithmetic --
    GeneralInterval& operator*=(const value_t& other) {
        m_midpoint_ *= other;
        for(auto&& [radius, weight] : m_weights_) {
            weight *= other;
            m_gradient_[radius] *= other;
        }
        return *this;
    }

    GeneralInterval& operator/=(const value_t& other) {
        return *this *= 1.0 / other;
    }

    GeneralInterval operator*(const value_t& other) const {
        GeneralInterval result(*this);
        result *= other;
        return result;
    }

    // -- GeneralInterval with GeneralInterval Comparison --
    bool operator==(const GeneralInterval& other) const {
        return m_midpoint_ == other.m_midpoint_ &&
               m_weights_ == other.m_weights_ &&
               m_gradient_ == other.m_gradient_;
    }

    bool operator!=(const GeneralInterval& other) const {
        return !(*this == other);
    }

    bool operator<(const GeneralInterval& other) const {
        return as_interval() < other.as_interval();
    }

    bool operator>(const GeneralInterval& other) const {
        return as_interval() > other.as_interval();
    }

    bool operator<=(const GeneralInterval& other) const {
        return as_interval() <= other.as_interval();
    }

    bool operator>=(const GeneralInterval& other) const {
        return as_interval() >= other.as_interval();
    }

private:
    /// "Flattens" *this to a normal interval
    interval_t compute_interval_() const;

    /// Computes the derivative of *this with respect to a given radius
    interval_t compute_derivative_(dep_radius_ptr radius) const;

    /// The midpoint of the generalized interval (Hassan calls it [c])
    interval_t m_midpoint_;

    /// Weights of each radius
    dep_radius_map_t m_weights_;

    /// Gradient stored as a map from radii to partial derivative
    dep_radius_map_t m_gradient_;
};

template<typename IntervalType>
std::ostream& operator<<(std::ostream& os,
                         const GeneralInterval<IntervalType>& gi) {
    os << gi.as_interval();
    return os;
}

template<typename IntervalType>
GeneralInterval<IntervalType> operator*(
  const typename IntervalType::value_t& lhs,
  const GeneralInterval<IntervalType>& rhs) {
    return rhs * lhs;
}

template<typename IntervalType>
GeneralInterval<IntervalType>::GeneralInterval(const interval_t& interval) :
  m_midpoint_(interval.median()) {
    // Is the interval a scalar? If so, we don't need to track the dependencies
    if(interval.radius() == 0.0) return;
    auto pradius         = std::make_shared<value_t>(interval.radius());
    m_weights_[pradius]  = interval_t(-1.0);
    m_gradient_[pradius] = interval_t(1.0);
}

template<typename IntervalType>
GeneralInterval<IntervalType>::GeneralInterval(
  const interval_t& center, const dep_radius_map_t& dep,
  const dep_radius_map_t& gradient) :
  m_midpoint_(center), m_weights_(dep), m_gradient_(gradient) {}

template<typename IntervalType>
std::string GeneralInterval<IntervalType>::print_hansen_form() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(10);
    ss << m_midpoint_.print_interval_form() << " + [-1,1] * (";
    for(auto&& [radius, weight] : m_weights_) {
        ss << (*radius) << "*" << weight.print_interval_form() << " + ";
    }
    auto temp = ss.str();
    temp.erase(temp.size() - 3); // Remove the last " + "
    return temp + ")";
}

template<typename IntervalType>
std::string GeneralInterval<IntervalType>::print_gradient() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(10);
    ss << "(";
    for(auto&& [radius, weight] : m_gradient_) {
        ss << weight.print_interval_form() << ", ";
    }
    auto temp = ss.str();
    temp.erase(temp.size() - 2); // Remove the last ", "
    return temp + ")";
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::operator+=(const GeneralInterval& other)
  -> GeneralInterval& {
    m_midpoint_ += other.m_midpoint_;
    for(auto&& [radius, weight] : other.m_weights_) {
        m_weights_[radius] += weight;
        m_gradient_[radius] += other.m_gradient_.at(radius);
    }
    return *this;
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::operator-=(const GeneralInterval& other)
  -> GeneralInterval& {
    m_midpoint_ -= other.m_midpoint_;
    for(auto&& [radius, weight] : other.m_weights_) {
        m_weights_[radius] -= weight;
        m_gradient_[radius] -= other.m_gradient_.at(radius);
    }
    return *this;
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::operator*=(const GeneralInterval& other)
  -> GeneralInterval& {
    // [c^z] = [c^x][c^y] + \sum_i [0, r_i^2][v_i^x][v_i^y]
    // [v_i^z] = [c^y][v_i^x] + [c^x][v_i^y] + [-1,1]|v_i^x|\sum_j
    // |r_j||v_j^y|

    // ydx/dzi + xdy/dzi

    interval_t unit(-1, 1);
    interval_t new_midpoint(m_midpoint_);
    dep_radius_map_t new_radius_to_weight;
    dep_radius_map_t new_gradient;

    // center update [c^x][c^y]
    new_midpoint *= other.m_midpoint_;

    auto other_interval = other.as_interval();

    auto compute_radius = [&](const auto& weights, const auto& radius) {
        value_t sum = 0.0;
        for(auto&& [other_radius, other_weight] : weights) {
            if(radius == other_radius) continue;
            sum += (*other_radius) * other_weight.abs();
        }
        return sum * unit;
    };

    for(auto&& [radius, weight] : m_weights_) {
        // [v_i^z] update [v_i^x][c^y]
        new_radius_to_weight[radius] = weight * other.m_midpoint_;
        new_gradient[radius] += other_interval * m_gradient_.at(radius);

        // [v_i^z] update [-1, 1]|v_i^x|\sum_j |r_j||v_j^y|
        const auto vi = weight.abs();
        const auto rj = compute_radius(other.m_weights_, radius);
        new_radius_to_weight[radius] += vi * rj;

        // If radius is only in *this, then [v_i^y] = [0,0],
        // i.e. no center update and [v_i^z] = [c^y][v_i^x]
        if(other.m_weights_.count(radius) == 0) continue;

        auto other_weight = other.m_weights_.at(radius);

        // center update [0, r_i^2][v_i^x][v_i^y]
        auto r2 = (*radius) * (*radius);
        new_midpoint += interval_t(0, r2) * weight * other_weight;

        // [v_i^z] update [c^x][v_i^y]
        new_radius_to_weight[radius] += other_weight * m_midpoint_;
    }

    // We have now covered all i in *this and all i in other shared with
    // *this. we thus need i that is in other but not in *this.
    auto this_interval = as_interval();
    for(auto&& [radius, weight] : other.m_weights_) {
        new_gradient[radius] += this_interval * other.m_gradient_.at(radius);

        if(new_radius_to_weight.count(radius) == 1) continue;
        new_radius_to_weight[radius] = weight * m_midpoint_;

        const auto vi = weight.abs();
        const auto rj = compute_radius(m_weights_, radius);
        new_radius_to_weight[radius] += vi * rj;
    }

    m_midpoint_ = std::move(new_midpoint);
    m_weights_  = std::move(new_radius_to_weight);
    m_gradient_ = std::move(new_gradient);
    return *this;
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::operator/=(const GeneralInterval& other)
  -> GeneralInterval& {
    // [c^z] = [c^x]/[c^y]
    // [v_i^z] = [v_i^x][c^y] - [c^x][v_i^y] /
    //    ([c^y]([c^y]+ [-1,1]|v_i^x|\sum_j|r_j||v_j^y|))

    if(other.contains(0.0)) { throw std::runtime_error("Division by zero"); }

    interval_t new_midpoint(m_midpoint_);
    new_midpoint /= other.m_midpoint_;

    dep_radius_map_t new_radius_to_weight;
    dep_radius_map_t new_gradient;

    // Compute the denominator for [v_i^z]
    interval_t unit(-1, 1);
    value_t sum         = 0.0;
    auto other_interval = other.as_interval();
    auto other_squared  = other_interval * other_interval;

    for(auto&& [radius, weight] : other.m_weights_) {
        sum += (*radius) * weight.abs();
    }
    auto denominator = other.m_midpoint_ * (other.m_midpoint_ + unit * sum);

    for(auto&& [radius, weight] : m_weights_) {
        new_gradient[radius] += m_gradient_.at(radius) / other_interval;
        interval_t numerator = weight * other.m_midpoint_;
        if(other.m_weights_.count(radius) == 1) {
            auto other_weight = other.m_weights_.at(radius);
            numerator -= m_midpoint_ * other_weight;
        }
        new_radius_to_weight[radius] = numerator / denominator;
    }

    auto this_interval = as_interval();
    for(auto&& [radius, weight] : other.m_weights_) {
        new_gradient[radius] -=
          this_interval * other.m_gradient_.at(radius) / other_squared;
        // Skip common radii
        if(m_weights_.count(radius) == 1) continue;
        auto numerator               = -weight * m_midpoint_;
        new_radius_to_weight[radius] = numerator / denominator;
    }

    m_midpoint_ = std::move(new_midpoint);
    m_weights_  = std::move(new_radius_to_weight);
    m_gradient_ = std::move(new_gradient);
    return *this;
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::compute_interval_() const -> interval_t {
    value_t sum = 0.0;
    for(auto&& [radius, weight] : m_weights_) {
        sum += (*radius) * weight.abs();
    }
    interval_t unit(-1, 1);
    return m_midpoint_ + sum * unit;
}

} // namespace sigma
