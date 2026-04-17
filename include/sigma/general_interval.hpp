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
    GeneralInterval(const interval_t&);
    GeneralInterval(const interval_t& center, const dep_radius_map_t& dep);
    GeneralInterval(const GeneralInterval& other) = default;

    const auto& center() const { return m_midpoint_; }
    const auto& dep() const { return m_radius_to_weight_; }

    interval_t as_interval() const { return compute_interval_(); }

    std::string print_hansen_form() const;
    std::string print_interval_form() const {
        return as_interval().print_interval_form();
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

    // -- GeneralInterval with value_t Arithmetic --
    GeneralInterval& operator*=(const value_t& other) {
        m_midpoint_ *= other;
        for(auto&& [radius, weight] : m_radius_to_weight_) { weight *= other; }
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

private:
    /// "Flattens" *this to a normal interval
    interval_t compute_interval_() const;

    /// The midpoint of the generalized interval (Hassan calls it [c])
    interval_t m_midpoint_;

    /// Map from radii to weights (Hassan calls them respectively zeta and
    /// v)
    dep_radius_map_t m_radius_to_weight_;
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
  m_midpoint_(interval.median()),
  m_radius_to_weight_(
    {{std::make_shared<value_t>(interval.radius()), interval_t(1.0)}}) {}

template<typename IntervalType>
GeneralInterval<IntervalType>::GeneralInterval(const interval_t& center,
                                               const dep_radius_map_t& dep) :
  m_midpoint_(center), m_radius_to_weight_(dep) {}

template<typename IntervalType>
std::string GeneralInterval<IntervalType>::print_hansen_form() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(10);
    ss << m_midpoint_.print_interval_form() << " + [-1,1] * (";
    for(auto&& [radius, weight] : m_radius_to_weight_) {
        ss << (*radius) << "*" << weight.print_interval_form() << " + ";
    }
    auto temp = ss.str();
    temp.erase(temp.size() - 3); // Remove the last " + "
    return temp + ")";
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::operator+=(const GeneralInterval& other)
  -> GeneralInterval& {
    m_midpoint_ += other.m_midpoint_;
    for(auto&& [radius, weight] : other.m_radius_to_weight_) {
        m_radius_to_weight_[radius] += weight;
    }
    return *this;
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::operator-=(const GeneralInterval& other)
  -> GeneralInterval& {
    m_midpoint_ -= other.m_midpoint_;
    for(auto&& [radius, weight] : other.m_radius_to_weight_) {
        m_radius_to_weight_[radius] -= weight;
    }
    return *this;
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::operator*=(const GeneralInterval& other)
  -> GeneralInterval& {
    // [c^z] = [c^x][c^y] + \sum_i [0, r_i^2][v_i^x][v_i^y]
    // [v_i^z] = [c^y][v_i^x] + [c^x][v_i^y] + [-1,1]|v_i^x|\sum_j
    // |r_j||v_j^y|

    interval_t unit(-1, 1);
    interval_t new_midpoint(m_midpoint_);
    dep_radius_map_t new_radius_to_weight;

    // center update [c^x][c^y]
    new_midpoint *= other.m_midpoint_;

    for(auto&& [radius, weight] : m_radius_to_weight_) {
        // [v_i^z] update [v_i^x][c^y]
        new_radius_to_weight[radius] = weight * other.m_midpoint_;

        // [v_i^z] update [-1, 1]|v_i^x|\sum_j |r_j||v_j^y|
        const auto vi = weight.abs();
        value_t sum   = 0.0;
        for(auto&& [other_radius, other_weight] : other.m_radius_to_weight_) {
            if(radius == other_radius) continue;
            const auto rj = (*other_radius);
            const auto vj = other_weight.abs();
            sum += rj * vj;
        }
        new_radius_to_weight[radius] += unit * vi * sum;

        // If radius is only in *this, then [v_i^y] = [0,0],
        // i.e. no center update and [v_i^z] = [c^y][v_i^x]
        if(other.m_radius_to_weight_.count(radius) == 0) continue;

        auto other_weight = other.m_radius_to_weight_.at(radius);

        // center update [0, r_i^2][v_i^x][v_i^y]
        auto r2 = (*radius) * (*radius);
        new_midpoint += interval_t(0, r2) * weight * other_weight;

        // [v_i^z] update [c^x][v_i^y]
        new_radius_to_weight[radius] += other_weight * m_midpoint_;
    }

    // We have now covered all i in *this and all i in other shared with
    // *this. we thus need i that is in other but not in *this.
    for(auto&& [radius, weight] : other.m_radius_to_weight_) {
        if(new_radius_to_weight.count(radius) == 1) continue;
        new_radius_to_weight[radius] = weight * m_midpoint_;

        const auto vi = weight.abs();
        value_t sum   = 0.0;
        for(auto&& [other_radius, other_weight] : m_radius_to_weight_) {
            if(radius == other_radius) continue;
            const auto rj = (*other_radius);
            const auto vj = other_weight.abs();
            sum += rj * vj;
        }
        new_radius_to_weight[radius] += unit * vi * sum;
    }

    m_midpoint_         = std::move(new_midpoint);
    m_radius_to_weight_ = std::move(new_radius_to_weight);
    return *this;
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::operator/=(const GeneralInterval& other)
  -> GeneralInterval& {
    // [c^z] = [c^x]/[c^y]
    // [v_i^z] = [v_i^x][c^y] - [c^x][v_i^y] /
    //    ([c^y]([c^y]+ [-1,1]|v_i^x|\sum_j|r_j||v_j^y|))

    interval_t new_midpoint(m_midpoint_);
    new_midpoint /= other.m_midpoint_;

    dep_radius_map_t new_radius_to_weight;

    // Compute the denominator for [v_i^z]
    interval_t unit(-1, 1);
    value_t sum = 0.0;
    for(auto&& [radius, weight] : other.m_radius_to_weight_) {
        sum += (*radius) * weight.abs();
    }
    auto denominator = other.m_midpoint_ * (other.m_midpoint_ + unit * sum);

    for(auto&& [radius, weight] : m_radius_to_weight_) {
        interval_t numerator = weight * other.m_midpoint_;
        if(other.m_radius_to_weight_.count(radius) == 1) {
            auto other_weight = other.m_radius_to_weight_.at(radius);
            numerator -= m_midpoint_ * other_weight;
        }
        new_radius_to_weight[radius] = numerator / denominator;
    }
    for(auto&& [radius, weight] : other.m_radius_to_weight_) {
        // Skip common radii
        if(m_radius_to_weight_.count(radius) == 1) continue;
        auto numerator               = -weight * m_midpoint_;
        new_radius_to_weight[radius] = numerator / denominator;
    }

    m_midpoint_         = std::move(new_midpoint);
    m_radius_to_weight_ = std::move(new_radius_to_weight);
    return *this;
}

template<typename IntervalType>
auto GeneralInterval<IntervalType>::compute_interval_() const -> interval_t {
    value_t sum = 0.0;
    for(auto&& [radius, weight] : m_radius_to_weight_) {
        sum += (*radius) * weight.abs();
    }
    interval_t unit(-1, 1);
    return m_midpoint_ + sum * unit;
}

} // namespace sigma
