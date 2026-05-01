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
    using size_type     = std::size_t;
    using value_t       = ValueType;
    using error_term_t  = std::shared_ptr<size_type>;
    using error_terms_t = std::unordered_map<error_term_t, value_t>;
    using interval_t    = Interval<value_t>;
    Affine()            = default;
    Affine(value_t center) : Affine(interval_t(center, center)) {}
    Affine(value_t lo, value_t hi) : Affine(interval_t(lo, hi)) {}

    Affine(const interval_t& interval) : m_center_(interval.median()) {
        if(interval.radius() > 0) {
            m_error_terms_[make_error_term()] = interval.radius();
        }
    }

    Affine(value_t center, error_terms_t radii) :
      m_center_(center), m_error_terms_(std::move(radii)) {}

    Affine(const Affine& other)     = default;
    Affine(Affine&& other) noexcept = default;

    Affine& operator=(const Affine& other)     = default;
    Affine& operator=(Affine&& other) noexcept = default;

    interval_t range() const;

    value_t center() const { return m_center_; }
    error_terms_t error_terms() const { return m_error_terms_; }
    value_t radius() const;
    void set_center(value_t center) { m_center_ = center; }
    void add_error_term(error_term_t error_term, value_t radius) {
        m_error_terms_[error_term] = radius;
    }

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
        for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
            error_term_i *= value;
        }
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
        for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
            error_term_i /= value;
        }
        return *this;
    }

    Affine& operator/=(const Affine& other);

    Affine operator/(value_t value) const { return Affine(*this) /= value; }

    Affine operator/(const Affine& other) const {
        return Affine(*this) /= other;
    }

    bool operator==(const Affine& other) const {
        return m_center_ == other.m_center_ &&
               m_error_terms_ == other.m_error_terms_;
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
    Affine apply_affine_transform(value_t alpha, value_t zeta,
                                  value_t delta) const {
        value_t new_center = alpha * m_center_ + zeta;
        error_terms_t new_error_terms;
        for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
            new_error_terms[error_symbol] = alpha * error_term_i;
        }
        new_error_terms[make_error_term()] = delta;
        return Affine(new_center, std::move(new_error_terms));
    }

private:
    /// Creates an opaque object that uniquely identifies an error term.
    /// At present it's the address of the object that is important;
    // could switch to a hash or uuid.
    error_term_t make_error_term() const {
        return std::make_shared<size_type>(m_error_terms_.size());
    }

    value_t affpy_correction_(const Affine& other) const;

    /// Dispatches based on m_correction_method_
    value_t multiplication_correction_(const Affine& other) const;

    value_t m_center_ = value_t(0.0);

    error_terms_t m_error_terms_;
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
    return interval_t(m_center_ - r, m_center_ + r);
}

template<typename ValueType>
auto Affine<ValueType>::radius() const -> value_t {
    value_t r = 0;
    for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
        r += std::fabs(error_term_i);
    }
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
    for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
        ss << error_term_i << " + ";
    }
    auto temp = ss.str();
    temp.erase(temp.end() - 2, temp.end());
    return temp;
}

template<typename ValueType>
auto Affine<ValueType>::operator-() const -> Affine {
    value_t new_center = -m_center_;
    error_terms_t new_error_terms;
    for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
        new_error_terms[error_symbol] = -error_term_i;
    }
    return Affine(new_center, new_error_terms);
}

template<typename ValueType>
auto Affine<ValueType>::operator+=(const Affine& other) -> Affine& {
    value_t new_center            = m_center_ + other.m_center_;
    error_terms_t new_error_terms = m_error_terms_;
    for(auto&& [error_symbol, error_term_i] : other.m_error_terms_) {
        new_error_terms[error_symbol] += error_term_i;
    }
    return *this = Affine(new_center, new_error_terms);
}

template<typename ValueType>
auto Affine<ValueType>::operator-=(const Affine& other) -> Affine& {
    value_t new_center            = m_center_ - other.m_center_;
    error_terms_t new_error_terms = m_error_terms_;
    for(auto&& [error_symbol, error_term_i] : other.m_error_terms_) {
        new_error_terms[error_symbol] -= error_term_i;
    }
    return *this = Affine(new_center, new_error_terms);
}

template<typename ValueType>
auto Affine<ValueType>::operator*=(const Affine& other) -> Affine& {
    value_t new_center = m_center_ * other.m_center_;
    error_terms_t new_error_terms;
    value_t new_radius = 0;
    for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
        new_error_terms[error_symbol] = error_term_i * other.m_center_;
        new_radius += std::fabs(new_error_terms[error_symbol]);
    }
    for(auto&& [error_symbol, error_term_j] : other.m_error_terms_) {
        new_error_terms[error_symbol] += error_term_j * m_center_;
        new_radius += std::fabs(new_error_terms[error_symbol]);
    }
    auto correction                    = multiplication_correction_(other);
    new_error_terms[make_error_term()] = correction;
    return *this                       = Affine(new_center, new_error_terms);
}

template<typename ValueType>
auto Affine<ValueType>::operator/=(const Affine& other) -> Affine& {
    if(other.contains(0)) { throw std::domain_error("Division by zero"); }
    // Compute the affine transformation which transforms other to 1 / other
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

    // Multiply by this by 1 / other
    return *this *= other.apply_affine_transform(alpha, zeta, delta);
}

template<typename ValueType>
auto Affine<ValueType>::affpy_correction_(const Affine& other) const
  -> value_t {
    return other.radius() * radius();
}

template<typename ValueType>
auto Affine<ValueType>::multiplication_correction_(const Affine& other) const
  -> value_t {
    return affpy_correction_(other);
}
} // namespace sigma
