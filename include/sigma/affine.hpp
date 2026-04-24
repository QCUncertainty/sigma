#pragma once
#include <sigma/interval.hpp>
#include <sstream>
#include <unordered_map>

/** @file affine.hpp
 *  @brief Defines the Affine class
 */

namespace sigma {

/** @brief Implements an affine interval.
 *
 * Follows https://www.tuhh.de/ti3/paper/rump/RuKas14.pdf
 */
template<typename ValueType>
class Affine {
public:
    enum class CorrectionMethod { AFFPY, EQ26, EQ27 };

    using size_type    = std::size_t;
    using value_t      = ValueType;
    using error_term_t = std::shared_ptr<size_type>;
    using radii_t      = std::unordered_map<error_term_t, value_t>;
    using interval_t   = Interval<value_t>;
    Affine()           = default;
    Affine(value_t center) : Affine(interval_t(center, center)) {}
    Affine(value_t lo, value_t hi) : Affine(interval_t(lo, hi)) {}

    Affine(const interval_t& interval) :
      m_center_(interval.median()), m_interval_(interval) {
        if(interval.radius() > 0) {
            m_radii_[make_error_term()] = interval.radius();
        }
    }

    Affine(value_t center, radii_t radii, interval_t interval) :
      m_center_(center),
      m_radii_(std::move(radii)),
      m_interval_(std::move(interval)) {}

    Affine(const Affine& other)     = default;
    Affine(Affine&& other) noexcept = default;

    Affine& operator=(const Affine& other)     = default;
    Affine& operator=(Affine&& other) noexcept = default;

    interval_t range() const;

    /// Returns the result using traditional interval arithmetic
    interval_t traditional_interval() const { return m_interval_; }

    value_t center() const { return m_center_; }
    radii_t radii() const { return m_radii_; }
    value_t radius() const;

    bool contains(value_t value) const;
    bool contains(const interval_t& interval) const;
    bool contains(const Affine& affine) const {
        return contains(affine.range());
    }

    bool strictly_less(value_t value) const {
        return strictly_less(interval_t(value, value));
    }
    bool strictly_greater(value_t value) const {
        return strictly_greater(interval_t(value, value));
    }
    bool strictly_less(const Affine& other) const {
        return range() < other.range();
    }
    bool strictly_greater(const Affine& other) const {
        return range() > other.range();
    }

    std::string print_affine_form() const;
    std::string print_interval_form() const {
        return range().print_interval_form();
    }

    /// Additive inverse
    Affine operator-() const;

    /// Addition
    Affine& operator+=(value_t value) {
        m_center_ += value;
        m_interval_ += value;
        return *this;
    }
    Affine& operator+=(const Affine& other);

    Affine operator+(value_t value) const { return Affine(*this) += value; }

    Affine operator+(const Affine& other) const {
        return Affine(*this) += other;
    }

    /// Subtraction
    Affine& operator-=(value_t value) {
        m_center_ -= value;
        m_interval_ -= value;
        return *this;
    }

    Affine& operator-=(const Affine& other);

    Affine operator-(value_t value) const { return Affine(*this) -= value; }

    Affine operator-(const Affine& other) const {
        return Affine(*this) -= other;
    }

    /// Multiplication
    Affine& operator*=(value_t value) {
        m_center_ *= value;
        for(auto&& [error_term, radius_i] : m_radii_) { radius_i *= value; }
        m_interval_ *= value;
        return *this;
    }

    Affine operator*(value_t value) const { return Affine(*this) *= value; }

    Affine operator*(const Affine& other) const {
        return Affine(*this) *= other;
    }

    Affine& operator*=(const Affine& other);

    /// Division
    Affine& operator/=(value_t value) {
        if(value == 0) { throw std::domain_error("Division by zero"); }
        m_center_ /= value;
        for(auto&& [error_term, radius_i] : m_radii_) { radius_i /= value; }
        m_interval_ /= value;
        return *this;
    }

    Affine& operator/=(const Affine& other);

    Affine operator/(value_t value) const { return Affine(*this) /= value; }

    Affine operator/(const Affine& other) const {
        return Affine(*this) /= other;
    }

    bool operator==(const Affine& other) const {
        return m_center_ == other.m_center_ && m_radii_ == other.m_radii_ &&
               m_interval_ == other.m_interval_;
    }

    bool operator!=(const Affine& other) const { return !(*this == other); }

    /** @brief Applies an affine transformation to *this.
     *
     *  Affine transformations are linear transformations characterized by two
     *  parameters: a scale factor and a shift. Often we will want to apply a
     *  non-affine transformation. To do this, we find an affine transformation
     *  that approximates the non-affine transformation (i.e., a scale factor
     *  and a shift for the approximate transformation) and an estimate of the
     *  error from using the approximate transformation.
     *
     *  @param[in] alpha The scale factor.
     *  @param[in] zeta The shift.
     *  @param[in] delta The error term.
     *
     *  @return The new center and radii.
     */
    auto apply_affine_transform(value_t alpha, value_t zeta,
                                value_t delta) const {
        value_t new_center = alpha * m_center_ + zeta;
        radii_t new_radii;
        for(auto&& [error_term, radius_i] : m_radii_) {
            new_radii[error_term] = alpha * radius_i;
        }
        new_radii[make_error_term()] = delta;
        return std::make_pair(new_center, new_radii);
    }

    void set_correction_method(CorrectionMethod method) {
        m_correction_method_ = method;
    }

private:
    /// Creates an opaque object that uniquely identifies an error term.
    /// At present it's the address of the object that is important;
    // could switch to a hash or uuid.
    error_term_t make_error_term() const {
        return std::make_shared<size_type>(m_radii_.size());
    }

    value_t affpy_correction_(const Affine& other) const;

    /// Eq. 26 in  https://www.tuhh.de/ti3/paper/rump/RuKas14.pdf
    value_t eq26_correction_(const Affine& other) const;

    /// Eq. 27 in  https://www.tuhh.de/ti3/paper/rump/RuKas14.pdf
    value_t eq27_correction_(const Affine& other) const;

    /// Dispatches based on m_correction_method_
    value_t multiplication_correction_(const Affine& other) const;

    value_t m_center_ = value_t(0.0);

    radii_t m_radii_;

    interval_t m_interval_;

    CorrectionMethod m_correction_method_ = CorrectionMethod::AFFPY;
};

template<typename ValueType>
std::ostream& operator<<(std::ostream& os, const Affine<ValueType>& a) {
    os << a.range();
    return os;
}

template<typename ValueType>
Affine<ValueType> operator*(ValueType value, const Affine<ValueType>& a) {
    return a * value;
}

template<typename ValueType>
auto Affine<ValueType>::range() const -> interval_t {
    auto r = radius();
    // Return the intersection *this and m_interval_
    auto lo = std::max(m_center_ - r, m_interval_.lower());
    auto hi = std::min(m_center_ + r, m_interval_.upper());
    return interval_t(lo, hi);
}

template<typename ValueType>
auto Affine<ValueType>::radius() const -> value_t {
    value_t r = 0;
    for(auto&& [error_term, radius_i] : m_radii_) { r += std::fabs(radius_i); }
    return r;
}

template<typename ValueType>
auto Affine<ValueType>::contains(value_t value) const -> bool {
    return range().contains(value);
}

template<typename ValueType>
auto Affine<ValueType>::contains(const interval_t& interval) const -> bool {
    auto lo = interval.lower();
    auto hi = interval.upper();
    auto a  = range();
    return a.contains(lo) && a.contains(hi);
}

template<typename ValueType>
std::string Affine<ValueType>::print_affine_form() const {
    std::stringstream ss;
    ss << m_center_ << " + ";
    for(auto&& [error_term, radius_i] : m_radii_) { ss << radius_i << " + "; }
    auto temp = ss.str();
    temp.erase(temp.end() - 2, temp.end());
    return temp;
}

template<typename ValueType>
auto Affine<ValueType>::operator-() const -> Affine {
    value_t new_center = -m_center_;
    radii_t new_radii;
    for(auto&& [error_term, radius_i] : m_radii_) {
        new_radii[error_term] = -radius_i;
    }
    interval_t new_interval = -m_interval_;
    return Affine(new_center, new_radii, new_interval);
}

template<typename ValueType>
auto Affine<ValueType>::operator+=(const Affine& other) -> Affine& {
    value_t new_center = m_center_ + other.m_center_;
    radii_t new_radii  = m_radii_;
    for(auto&& [error_term, radius_i] : other.m_radii_) {
        new_radii[error_term] += radius_i;
    }
    interval_t new_interval = m_interval_ + other.m_interval_;
    return *this            = Affine(new_center, new_radii, new_interval);
}

template<typename ValueType>
auto Affine<ValueType>::operator-=(const Affine& other) -> Affine& {
    value_t new_center = m_center_ - other.m_center_;
    radii_t new_radii  = m_radii_;
    for(auto&& [error_term, radius_i] : other.m_radii_) {
        new_radii[error_term] -= radius_i;
    }
    interval_t new_interval = m_interval_ - other.m_interval_;
    return *this            = Affine(new_center, new_radii, new_interval);
}

template<typename ValueType>
auto Affine<ValueType>::operator*=(const Affine& other) -> Affine& {
    value_t new_center = m_center_ * other.m_center_;
    radii_t new_radii;

    for(auto&& [error_term, radius_i] : m_radii_) {
        new_radii[error_term] = radius_i * other.m_center_;
    }
    for(auto&& [error_term, radius_j] : other.m_radii_) {
        new_radii[error_term] += radius_j * m_center_;
    }
    new_radii[make_error_term()] = multiplication_correction_(other);
    interval_t new_interval      = m_interval_ * other.m_interval_;
    return *this                 = Affine(new_center, new_radii, new_interval);
}

template<typename ValueType>
auto Affine<ValueType>::operator/=(const Affine& other) -> Affine& {
    if(other.contains(0)) { throw std::domain_error("Division by zero"); }
    auto other_range = other.range();
    auto abs_inf     = std::fabs(other_range.lower());
    auto abs_sup     = std::fabs(other_range.upper());
    auto a           = std::min(abs_inf, abs_sup);
    auto b           = std::max(abs_inf, abs_sup);
    auto alpha       = value_t(-1.0) / (b * b);
    auto lo          = value_t(1.0) / a - alpha * a;
    auto hi          = value_t(2.0) / b;
    interval_t interval(std::min(lo, hi), std::max(lo, hi));
    auto zeta  = std::fabs(interval.median());
    auto delta = interval.radius();
    auto [new_center, new_radii] =
      other.apply_affine_transform(alpha, zeta, delta);
    auto new_interval = interval_t(1.0, 1.0) / other.m_interval_;
    Affine one_over_other(new_center, new_radii, new_interval);
    return *this *= one_over_other;
}

template<typename ValueType>
auto Affine<ValueType>::affpy_correction_(const Affine& other) const
  -> value_t {
    return other.radius() * radius();
}

template<typename ValueType>
auto Affine<ValueType>::eq27_correction_(const Affine& other) const -> value_t {
    value_t lhsq = 0.0;
    value_t rhsq = 0.0;
    for(auto&& [error_term, radius_i] : m_radii_) {
        lhsq += radius_i * radius_i;
    }
    for(auto&& [error_term, radius_j] : other.m_radii_) {
        rhsq += radius_j * radius_j;
    }
    auto lpr = (*this) + other;
    auto lmr = (*this) - other;
    return std::sqrt(lhsq) * std::sqrt(rhsq) *
           std::max(lpr.radius(), lmr.radius());
}

template<typename ValueType>
auto Affine<ValueType>::eq26_correction_(const Affine& other) const -> value_t {
    value_t new_error_term = 0;
    value_t pos_sum        = 0;
    value_t neg_sum        = 0;
    std::vector<error_term_t> common_error_terms;

    for(auto&& [error_term, radius_i] : m_radii_) {
        // computes the v_i = gamma_i delta_i terms
        if(other.m_radii_.count(error_term)) {
            common_error_terms.push_back(error_term);
            auto r_ij = radius_i * m_radii_.at(error_term);
            if(r_ij > 0) {
                pos_sum += r_ij;
            } else {
                neg_sum += std::fabs(r_ij);
            }
            continue;
        }
        // Computes the gamma_i delta_j for errors unique to *this and other
        for(auto&& [error_term_j, radius_j] : other.m_radii_) {
            if(m_radii_.count(error_term_j)) continue;
            new_error_term += std::fabs(radius_i * radius_j);
        }
    }

    // We missed pairs gamma_i delta_j (i!=j) where i and j are shared errors
    for(std::size_t i = 0; i < common_error_terms.size(); i++) {
        auto gi = m_radii_.at(common_error_terms[i]);
        auto di = other.m_radii_.at(common_error_terms[i]);
        for(std::size_t j = i + 1; j < common_error_terms.size(); j++) {
            auto gj = m_radii_.at(common_error_terms[j]);
            auto dj = other.m_radii_.at(common_error_terms[j]);
            new_error_term += std::fabs(gi * dj + gj * di);
        }
    }
    return new_error_term + std::max(pos_sum, neg_sum);
}

template<typename ValueType>
auto Affine<ValueType>::multiplication_correction_(const Affine& other) const
  -> value_t {
    if(m_correction_method_ == CorrectionMethod::AFFPY) {
        return affpy_correction_(other);
    } else if(m_correction_method_ == CorrectionMethod::EQ26) {
        return eq26_correction_(other);
    } else if(m_correction_method_ == CorrectionMethod::EQ27) {
        return eq27_correction_(other);
    }
    throw std::runtime_error("Unknown correction method");
}
} // namespace sigma
