#pragma once
#include <cmath>
#include <sigma/affine/affine.hpp>

/** @file thresholded_affine.hpp
 *  @brief Defines the ThresholdedAffine class
 */

namespace sigma {

/** @brief Implements affine arithmetic with small-term lumping.
 *
 * @tparam ValueType The numeric type of the center and error term radii.
 *
 * ThresholdedAffine extends standard affine arithmetic with a mechanism to
 * control the number of error symbols carried in an affine form. Each
 * non-affine operation can introduce new error symbols (e.g. multiplication
 * creates a nonlinearity symbol); without pruning, the symbol count grows
 * unboundedly.
 *
 * This class reduces symbol proliferation by replacing small error terms with
 * fresh independent symbols. After each operation, any error term whose
 * relative contribution to the total radius falls below a user-specified
 * threshold @f$t@f$ is replaced by a new unique noise symbol carrying the same
 * magnitude:
 * @f[
 *   \frac{|x_i|}{\sum_j |x_j|} < t \implies x_i \epsilon_i \to |x_i|
 * \epsilon_\text{fresh}
 * @f]
 *
 * Each absorption event creates a globally unique symbol, so absorbed terms
 * from independent sources can never cancel each other. The resulting interval
 * is always a superset of the interval produced by an unthresholded Affine
 * form (conservative/sound). The only loss relative to full affine arithmetic
 * is that small terms whose symbols were shared with other forms will no longer
 * cancel exactly after absorption.
 *
 * Constructors accept a @ref Threshold tag type to distinguish the threshold
 * parameter from ordinary value parameters and avoid constructor ambiguity.
 */
template<typename ValueType>
class ThresholdedAffine {
public:
    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /// Type used for storing floating point values
    using value_t = ValueType;

    /// Opaque type used to store error term information
    using error_term_t = typename Affine<ValueType>::error_term_t;

    /// Type used to map error terms to their radii
    using error_terms_t = typename Affine<ValueType>::error_terms_t;

    /// Type of an interval
    using interval_t = typename Affine<ValueType>::interval_t;

    /// Type of the underlying Affine form
    using affine_t = Affine<ValueType>;

    /** @brief Tag type used to pass the relative threshold to constructors.
     *
     *  Wrapping the threshold in this type avoids overload ambiguity between
     *  `ThresholdedAffine(value_t lo, value_t hi)` and
     *  `ThresholdedAffine(value_t center, value_t threshold)`.
     *
     *  Example:
     *  @code
     *  ThresholdedAffine<double> x(1.0, 2.0, Threshold{0.05});
     *  @endcode
     */
    struct Threshold {
        /// The relative threshold value (e.g. 0.01 for 1%)
        value_t value;
        /// Constructs a Threshold with the given value.
        explicit Threshold(value_t v) : value(v) {}
    };

    // --- Constructors and Assignment ----------------------------------------

    /** @brief Constructs an empty ThresholdedAffine.
     *
     *  The resulting ThresholdedAffine represents the empty set. The lump term
     *  is initialized but has zero radius, and the threshold is set to the
     *  default value.
     *
     *  @throw none No throw guarantee
     */
    ThresholdedAffine() : m_threshold_(default_threshold().value) {}

    /** @brief Constructs a ThresholdedAffine from a center value.
     *
     *  @param[in] center The center value.
     *  @param[in] t      The relative threshold, defaults to
     *                    default_threshold().
     *
     *  @throw none No throw guarantee
     */
    explicit ThresholdedAffine(value_t center,
                               Threshold t = default_threshold()) :
      ThresholdedAffine(interval_t(center), t) {}

    /** @brief Constructs a ThresholdedAffine from a lower and upper bound.
     *
     *  @param[in] lo The lower bound.
     *  @param[in] hi The upper bound.
     *  @param[in] t  The relative threshold, defaults to
     *                default_threshold().
     *
     *  @throw std::bad_alloc If memory allocation for the error term fails.
     */
    ThresholdedAffine(value_t lo, value_t hi,
                      Threshold t = default_threshold()) :
      ThresholdedAffine(interval_t(lo, hi), t) {
        apply_threshold_();
    }

    /** @brief Constructs a ThresholdedAffine from an interval.
     *
     *  @param[in] interval The interval.
     *  @param[in] t        The relative threshold, defaults to
     *                      default_threshold().
     *
     *  @throw std::bad_alloc If memory allocation for the error term fails.
     */
    explicit ThresholdedAffine(const interval_t& interval,
                               Threshold t = default_threshold()) :
      m_affine_(interval), m_threshold_(t.value) {
        apply_threshold_();
    }

    /** @brief Constructs a ThresholdedAffine from a center and error terms.
     *
     *  @param[in] center The center value.
     *  @param[in] radii  Map of error symbols to their radii.
     *  @param[in] t      The relative threshold, defaults to
     *                    default_threshold().
     *
     *  @throw none No throw guarantee
     */
    ThresholdedAffine(value_t center, error_terms_t radii,
                      Threshold t = default_threshold()) :
      m_affine_(center, std::move(radii)), m_threshold_(t.value) {
        apply_threshold_();
    }

    /** @brief Constructs a ThresholdedAffine by wrapping an Affine form.
     *
     *  Used internally by operations that produce an Affine result and need to
     *  re-wrap it as a ThresholdedAffine.
     *
     *  @param[in] a         The Affine form to wrap.
     *  @param[in] threshold The relative threshold.
     *
     *  @throw std::bad_alloc If memory allocation for the error term fails.
     *                        Strong throw guarantee.
     */
    ThresholdedAffine(affine_t a, value_t threshold) :
      m_affine_(std::move(a)), m_threshold_(threshold) {
        apply_threshold_();
    }

    /** @brief Copy constructor.
     *
     *  @param[in] other The ThresholdedAffine to copy.
     *
     *  @throw std::bad_alloc If memory allocation for the error terms fails.
     *                        Strong throw guarantee.
     */
    ThresholdedAffine(const ThresholdedAffine& other) = default;

    /** @brief Move constructor.
     *
     *  @param[in] other The ThresholdedAffine to move.
     *  @throw none No throw guarantee
     */
    ThresholdedAffine(ThresholdedAffine&& other) noexcept = default;

    /** @brief Copy assignment operator.
     *
     *  @param[in] other The ThresholdedAffine to copy.
     *
     *  @return Reference to this ThresholdedAffine after assignment.
     *
     *  @throw std::bad_alloc If memory allocation for the error terms fails.
     *                        Strong throw guarantee.
     */
    ThresholdedAffine& operator=(const ThresholdedAffine& other) = default;

    /** @brief Move assignment operator.
     *
     *  @param[in] other The ThresholdedAffine to move.
     *
     *  @return Reference to this ThresholdedAffine after assignment.
     *
     *  @throw none No throw guarantee
     */
    ThresholdedAffine& operator=(ThresholdedAffine&& other) noexcept = default;

    // -- State Accessors -----------------------------------------------------

    /** @brief Returns the default threshold value.
     *
     *  This is a state method that ensures the default threshold is defined
     *  in a single place and can be easily changed if needed. The default
     *  threshold is currently set to 0.01 (1%).
     *
     *  @return The default threshold.
     *  @throw none No throw guarantee
     */
    static constexpr Threshold default_threshold() { return Threshold(0.01); }

    /** @brief Returns the interval represented by *this.
     *
     *  @return The interval [center - radius, center + radius].
     *  @throw none No throw guarantee
     */
    interval_t range() const { return m_affine_.range(); }

    /** @brief Returns the center of the affine form.
     *
     *  @return The center value.
     *  @throw std::domain_error If *this is empty.
     */
    value_t center() const { return m_affine_.center(); }

    /** @brief Returns the error terms, including the lump term.
     *
     *  @return Const reference to the error terms map.
     *  @throw None No throw guarantee.
     */
    const error_terms_t& error_terms() const { return m_affine_.error_terms(); }

    /** @brief Returns the sum of absolute values of all error term radii.
     *
     *  @return The radius.
     *  @throw std::domain_error If *this is empty.
     */
    value_t radius() const { return m_affine_.radius(); }

    /** @brief Sets the center of the affine form.
     *
     *  @param[in] c The new center.
     *  @throw None No throw guarantee.
     */
    void set_center(value_t c) { m_affine_.set_center(c); }

    /** @brief Adds an error term to the affine form and applies thresholding.
     *
     *  @param[in] error_term The error symbol.
     *  @param[in] r          The radius of the new term.
     *  @throw None No throw guarantee.
     */
    void add_error_term(error_term_t error_term, value_t r) {
        m_affine_.add_error_term(error_term, r);
        apply_threshold_();
    }

    /** @brief Returns whether *this contains a scalar value @p v.
     *
     *  @param[in] v The value to test.
     *  @return True if the value is in range().
     *  @throw none No throw guarantee
     */
    bool contains(value_t v) const { return m_affine_.contains(v); }

    /** @brief Returns whether *this contains an interval.
     *
     *  @param[in] i The interval to test.
     *  @return True if every point in @p i is in range().
     *  @throw none No throw guarantee
     */
    bool contains(const interval_t& i) const { return m_affine_.contains(i); }

    /** @brief Returns whether *this contains another ThresholdedAffine's range.
     *
     *  @param[in] other The ThresholdedAffine to test.
     *  @return True if every point in other.range() is in range().
     *  @throw none No throw guarantee
     */
    bool contains(const ThresholdedAffine& other) const {
        return m_affine_.contains(other.m_affine_.range());
    }

    /** @brief Returns whether *this is empty.
     *
     *  @return True if *this represents the empty set.
     *  @throw none No throw guarantee
     */
    bool empty() const noexcept { return m_affine_.empty(); }

    /** @brief Returns a string of the affine form.
     *
     *  @return Affine form string.
     *  @throw std::bad_alloc If memory allocation for the string fails.
     *                        Strong throw guarantee.
     */
    std::string print_affine_form() const {
        return m_affine_.print_affine_form();
    }

    /** @brief Returns a string of the interval form.
     *
     *  @return Interval form string, e.g. [lo, hi].
     *  @throw std::bad_alloc
     */
    std::string print_interval_form() const {
        return m_affine_.print_interval_form();
    }

    /** @brief Returns the underlying Affine form (read-only).
     *
     *  Used by operations that need to call Affine-specific functions (e.g.
     *  apply_affine_transform) and then re-wrap the result.
     *
     *  @return Const reference to the internal Affine form.
     *  @throw none No throw guarantee
     */
    const affine_t& affine() const { return m_affine_; }

    /** @brief Returns the relative threshold.
     *
     *  @return The threshold @f$t@f$ such that terms with
     *          @f$|x_i|/\text{radius} < t@f$ are lumped.
     *  @throw none No throw guarantee
     */
    value_t threshold() const { return m_threshold_; }

    // -- Arithmetic Operators ------------------------------------------------

    /** @brief Returns the additive inverse of *this.
     *
     *  Negation preserves relative error-term magnitudes, so no re-thresholding
     *  is needed.
     *
     *  @return The negated ThresholdedAffine.
     *  @throw std::bad_alloc If memory allocation fails.
     */
    ThresholdedAffine operator-() const {
        auto result      = *this;
        result.m_affine_ = -m_affine_;
        return result;
    }

    /** @brief Adds a scalar to *this.
     *
     *  @param[in] value The scalar to add.
     *  @return Reference to *this.
     *  @throw None No throw guarantee.
     */
    ThresholdedAffine& operator+=(value_t value) {
        m_affine_ += value;
        return *this;
    }

    /** @brief Adds another ThresholdedAffine to *this.
     *
     *  @param[in] other The form to add.
     *  @return Reference to *this.
     *  @throw None No throw guarantee.
     */
    ThresholdedAffine& operator+=(const ThresholdedAffine& other) {
        m_affine_ += other.m_affine_;
        apply_threshold_();
        return *this;
    }

    /** @brief Returns the sum of *this and a scalar.
     *
     *  @param[in] value The scalar to add.
     *  @return The sum of *this and @p value.
     *  @throw std::bad_alloc If memory allocation for the new ThresholdedAffine
     *                        fails. Strong throw guarantee.
     */
    ThresholdedAffine operator+(value_t value) const {
        return ThresholdedAffine(*this) += value;
    }

    /** @brief Returns the sum of *this and another ThresholdedAffine.
     *
     *  @param[in] other The ThresholdedAffine to add.
     *  @return The sum of *this and @p other.
     *  @throw std::bad_alloc If memory allocation for the new ThresholdedAffine
     *                        fails. Strong throw guarantee.
     */
    ThresholdedAffine operator+(const ThresholdedAffine& other) const {
        return ThresholdedAffine(*this) += other;
    }

    /** @brief Subtracts a scalar from *this.
     *
     *  @param[in] value The scalar to subtract.
     *  @return Reference to *this.
     *  @throw None No throw guarantee.
     */
    ThresholdedAffine& operator-=(value_t value) {
        m_affine_ -= value;
        return *this;
    }

    /** @brief Subtracts another ThresholdedAffine from *this.
     *
     *  @param[in] other The form to subtract.
     *  @return Reference to *this.
     *  @throw None No throw guarantee.
     */
    ThresholdedAffine& operator-=(const ThresholdedAffine& other) {
        m_affine_ -= other.m_affine_;
        apply_threshold_();
        return *this;
    }

    /** @brief Returns the difference of *this and a scalar.
     *
     *  @param[in] value The scalar to subtract.
     *  @return The difference of *this and @p value.
     *  @throw std::bad_alloc If memory allocation for the new ThresholdedAffine
     *                        fails. Strong throw guarantee.
     */
    ThresholdedAffine operator-(value_t value) const {
        return ThresholdedAffine(*this) -= value;
    }

    /** @brief Returns the difference of *this and another ThresholdedAffine.
     *
     *  @param[in] other The ThresholdedAffine to subtract.
     *  @return The difference of *this and @p other.
     *  @throw std::bad_alloc If memory allocation for the new ThresholdedAffine
     *                        fails. Strong throw guarantee.
     */
    ThresholdedAffine operator-(const ThresholdedAffine& other) const {
        return ThresholdedAffine(*this) -= other;
    }

    /** @brief Multiplies *this by a scalar.
     *
     *  @param[in] value The scalar to multiply by.
     *  @return Reference to *this.
     *  @throw None No throw guarantee.
     */
    ThresholdedAffine& operator*=(value_t value) {
        m_affine_ *= value;
        apply_threshold_();
        return *this;
    }

    /** @brief Multiplies *this by another ThresholdedAffine.
     *
     *  Multiplication introduces a new nonlinearity error term; thresholding
     *  is applied afterward so that small terms (including the new one, if its
     *  relative contribution is negligible) are folded into the lump.
     *
     *  @param[in] other The form to multiply by.
     *  @return Reference to *this.
     *  @throw None No throw guarantee.
     */
    ThresholdedAffine& operator*=(const ThresholdedAffine& other) {
        m_affine_ *= other.m_affine_;
        apply_threshold_();
        return *this;
    }

    /** @brief Multiplies *this by a scalar.
     *
     *  @param[in] value The scalar to multiply by.
     *  @return The product of *this and @p value.
     *  @throw std::bad_alloc If memory allocation for the new ThresholdedAffine
     *                        fails. Strong throw guarantee.
     */
    ThresholdedAffine operator*(value_t value) const {
        return ThresholdedAffine(*this) *= value;
    }

    /** @brief Multiplies *this by another ThresholdedAffine.
     *
     *  @param[in] other The ThresholdedAffine to multiply by.
     *  @return The product of *this and @p other.
     *  @throw std::bad_alloc If memory allocation for the new ThresholdedAffine
     *                        fails. Strong throw guarantee.
     */
    ThresholdedAffine operator*(const ThresholdedAffine& other) const {
        return ThresholdedAffine(*this) *= other;
    }

    /** @brief Divides *this by a scalar.
     *
     *  @param[in] value The scalar to divide by.
     *  @return Reference to *this.
     *  @throw std::domain_error If @p value is zero.
     */
    ThresholdedAffine& operator/=(value_t value) {
        m_affine_ /= value;
        apply_threshold_();
        return *this;
    }

    /** @brief Divides *this by another ThresholdedAffine.
     *
     *  @param[in] other The form to divide by.
     *  @return Reference to *this.
     *  @throw std::domain_error If @p other is empty or contains zero.
     */
    ThresholdedAffine& operator/=(const ThresholdedAffine& other) {
        m_affine_ /= other.m_affine_;
        apply_threshold_();
        return *this;
    }

    /** @brief Divides *this by a scalar.
     *
     *  @param[in] value The scalar to divide by.
     *  @return The quotient of *this and @p value.
     *  @throw std::domain_error If @p value is zero.
     *  @throw std::bad_alloc If memory allocation for the new ThresholdedAffine
     *                        fails. Strong throw guarantee.
     */
    ThresholdedAffine operator/(value_t value) const {
        return ThresholdedAffine(*this) /= value;
    }

    /** @brief Divides *this by another ThresholdedAffine.
     *
     *  @param[in] other The ThresholdedAffine to divide by.
     *  @return The quotient of *this and @p other.
     *  @throw std::domain_error If @p other is empty or contains zero.
     *  @throw std::bad_alloc If memory allocation for the new ThresholdedAffine
     *                        fails. Strong throw guarantee.
     */
    ThresholdedAffine operator/(const ThresholdedAffine& other) const {
        return ThresholdedAffine(*this) /= other;
    }

    // -- Comparison Operators ------------------------------------------------

    /** @brief Checks equality of two ThresholdedAffine forms.
     *
     *  Two ThresholdedAffine forms are equal if they have the same internal
     *  Affine form and the same threshold.
     *
     *  @param[in] other The form to compare.
     *  @return True if equal.
     *  @throw none No throw guarantee.
     */
    bool operator==(const ThresholdedAffine& other) const {
        return m_affine_ == other.m_affine_ &&
               m_threshold_ == other.m_threshold_;
    }

    /** @brief Checks inequality of two ThresholdedAffine forms.
     *
     *  @param[in] other The form to compare.
     *  @return True if not equal.
     *  @throw none No throw guarantee.
     */
    bool operator!=(const ThresholdedAffine& other) const {
        return !(*this == other);
    }

private:
    /** @brief Replaces error terms below the threshold with fresh symbols.
     *
     *  After each arithmetic operation, scans all error terms. Terms with
     *  @f$|x_i| / \text{total\_radius} < t@f$ are replaced by a new globally
     *  unique symbol carrying @f$|x_i|@f$. Because each absorption event
     *  produces a distinct symbol, absorbed terms from independent sources
     *  can never cancel each other in subsequent operations.
     */
    void apply_threshold_() {
        if(m_affine_.empty()) return;
        auto total_r = m_affine_.radius();

        auto terms = m_affine_.error_terms();
        if(terms.empty()) return;

        error_terms_t new_terms;
        for(auto&& [sym, coeff] : terms) {
            if(std::fabs(coeff) < std::numeric_limits<value_t>::epsilon()) {
                continue;
            } else if(total_r != value_t(0) &&
                      std::fabs(coeff) / total_r < m_threshold_) {
                new_terms[affine_t::make_error_term()] = std::fabs(coeff);
            } else {
                new_terms[sym] = coeff;
            }
        }
        m_affine_ = affine_t(m_affine_.center(), std::move(new_terms));
    }

    /// The internal affine form holding all error terms.
    affine_t m_affine_;

    /// Relative threshold: terms with |x_i| / radius < m_threshold_ are
    /// replaced.
    value_t m_threshold_;
};

// -- Non-member functions ----------------------------------------------------

/** @brief Outputs the range of a ThresholdedAffine to a stream.
 *
 *  @related ThresholdedAffine
 *  @tparam ValueType The value type.
 */
template<typename ValueType>
std::ostream& operator<<(std::ostream& os,
                         const ThresholdedAffine<ValueType>& a) {
    os << a.range();
    return os;
}

/** @brief Multiplies a scalar by a ThresholdedAffine.
 *
 *  @related ThresholdedAffine
 *  @tparam ValueType The value type.
 */
template<typename ValueType>
ThresholdedAffine<ValueType> operator*(ValueType value,
                                       const ThresholdedAffine<ValueType>& a) {
    return a * value;
}

// -- Operations --------------------------------------------------------------
// These delegate to the corresponding Affine operations and re-wrap the result.

/** @brief Absolute value of a ThresholdedAffine.
 *
 *  @related ThresholdedAffine
 *  @tparam T The value type.
 */
template<typename T>
ThresholdedAffine<T> abs(const ThresholdedAffine<T>& a) {
    return ThresholdedAffine<T>(abs(a.affine()), a.threshold());
}

/** @brief Absolute value of a ThresholdedAffine (alias for abs).
 *
 *  @related ThresholdedAffine
 *  @tparam T The value type.
 */
template<typename T>
ThresholdedAffine<T> fabs(const ThresholdedAffine<T>& a) {
    return ThresholdedAffine<T>(fabs(a.affine()), a.threshold());
}

/** @brief Square root of a ThresholdedAffine.
 *
 *  @related ThresholdedAffine
 *  @tparam T The value type.
 *  @throw std::domain_error If the range contains negative values.
 */
template<typename T>
ThresholdedAffine<T> sqrt(const ThresholdedAffine<T>& a) {
    return ThresholdedAffine<T>(sqrt(a.affine()), a.threshold());
}

/** @brief Exponential of a ThresholdedAffine.
 *
 *  @related ThresholdedAffine
 *  @tparam T The value type.
 */
template<typename T>
ThresholdedAffine<T> exp(const ThresholdedAffine<T>& a) {
    return ThresholdedAffine<T>(exp(a.affine()), a.threshold());
}

/** @brief Natural logarithm of a ThresholdedAffine.
 *
 *  @related ThresholdedAffine
 *  @tparam T The value type.
 *  @throw std::domain_error If the range contains non-positive values.
 */
template<typename T>
ThresholdedAffine<T> log(const ThresholdedAffine<T>& a) {
    return ThresholdedAffine<T>(log(a.affine()), a.threshold());
}

/** @brief Power of a ThresholdedAffine.
 *
 *  @related ThresholdedAffine
 *  @tparam T The value type.
 *  @tparam U The exponent type.
 *  @throw std::domain_error See Affine::pow for conditions.
 */
template<typename T, typename U>
ThresholdedAffine<T> pow(const ThresholdedAffine<T>& a, const U& exp) {
    return ThresholdedAffine<T>(pow(a.affine(), exp), a.threshold());
}

/// Typedef for a thresholded affine form of floats
using TAFloat = ThresholdedAffine<float>;

/// Typedef for a thresholded affine form of doubles
using TADouble = ThresholdedAffine<double>;

} // namespace sigma
