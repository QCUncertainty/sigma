#pragma once
#include <atomic>
#include <optional>
#include <sigma/interval/interval.hpp>
#include <sstream>
#include <unordered_map>

/** @file affine.hpp
 *  @brief Defines the Affine class
 */

namespace sigma {

/** @brief Implements affine arithmetic.
 *
 * @tparam ValueType The numeric type of the center and error term radii.
 *
 * Follows https://www.tuhh.de/ti3/paper/rump/RuKas14.pdf.
 *
 * Affine arithmetic is a way to represent a range of values (an interval) as a
 * center value and a set of error terms. Each error term has a symbol (an
 * opaque object that uniquely identifies the error term) and a radius (a value
 * that represents the maximum error from the center value for that error term).
 *
 * Mathematically, an affine form can be represented as:
 * @f[
 *   x = x_0 + \sum_{i=1}^n x_i \epsilon_i
 * @f]
 * where @f$x_0@f$ is the center, @f$x_i@f$ are the error term radii, and
 * @f$\epsilon_i@f$ are the error term symbols. The error term symbols are
 * assumed to be independent and can take on any value in the range [-1, 1].
 * Therefore, the affine form represents all values that can be obtained by
 * substituting any value in [-1, 1] for each error term symbol.
 *
 * Unlike traditional intervals, affine arithmetic can capture dependencies
 * between variables. For example, if we have an affine value x that represents
 * the range [1, 2], we can represent it as x = 1.5 + 0.5*e1, where e1 is an
 * error term with radius 0.5. If we then define y = x - x, we can represent y
 * as y = (1.5 + 0.5*e1) - (1.5 + 0.5*e1) = 0, which captures the fact that y is
 * always 0 regardless of the value of e1. In contrast, if we were to represent
 * x and y as traditional intervals, we would have x = [1, 2] and y = [-1, 1],
 * because intervals do not track the dependency between x and y.
 *
 * N.b. The current implementation of the Affine class does not track
 * uncertainty from floating point arithmetic.
 */
template<typename ValueType>
class Affine {
public:
    /// Type used for indexing and offsets
    using size_type = std::size_t;

    /// Type used for storing floating point values
    using value_t = ValueType;

    /// Opaque type used to store error term information
    using error_term_t = size_type;

    /// Type used to map error terms to their radii
    using error_terms_t = std::unordered_map<error_term_t, value_t>;

    /// Type of an interval
    using interval_t = Interval<value_t>;

    // --- Constructors and Assignment ----------------------------------------

    /** @brief Constructs an empty affine form.
     *
     *  An empty affine form represents the empty set.
     *
     *  @throw none No throw guarantee
     */
    Affine() = default;

    /** @brief Constructs an affine form from a center value.
     *
     *  The resulting affine form has no error terms and represents the single
     *  value given by the center.
     *
     *  @param[in] center The center value of the affine form.
     *
     *  @throw none No throw guarantee
     */
    Affine(value_t center) : Affine(interval_t(center, center)) {}

    /** @brief Constructs an affine form from a lower and upper bound.
     *
     *  The resulting affine form has a single error term and represents the
     *  interval defined by the lower and upper bounds.
     *
     *  @param[in] lo The lower bound of the interval represented by the affine
     * form.
     *  @param[in] hi The upper bound of the interval represented by the affine
     * form.
     *
     *  @throw std::bad_alloc If memory allocation for the error term fails.
     *                       Strong throw guarantee.
     */
    Affine(value_t lo, value_t hi) : Affine(interval_t(lo, hi)) {}

    /** @brief Constructs an affine form from an interval.
     *
     *  For a non-empty interval, the resulting affine form has a single error
     *  term and represents the interval defined by the lower and upper bounds
     *  of @p interval. If @p interval is empty, the resulting affine form is
     *  also empty.
     *
     *  @param[in] interval The interval represented by the affine form.
     *
     *  @throw std::bad_alloc If memory allocation for the error term fails.
     *                        Strong throw guarantee.
     */
    explicit Affine(const interval_t& interval) {
        if(interval.empty()) { return; }
        m_center_ = interval.median();
        if(interval.radius() > 0) {
            m_error_terms_[make_error_term()] = interval.radius();
        }
    }

    /** @brief Construct an affine form from a center value and a map of errors.
     *
     *  This constructor allows for the creation of an affine form by specifying
     *  the full state of the affine form (the center and the error terms).
     *  This ctor is meant primarily for use by other parts of Sigma, but can be
     *  used by users as well.
     *
     *  @param[in] center The center value of the affine form.
     *  @param[in] radii A map of error terms to their radii.
     *
     *  @throw none No throw guarantee
     */
    Affine(value_t center, error_terms_t radii) :
      m_center_(center), m_error_terms_(std::move(radii)) {}

    /** @brief Makes a deep copy of @p other.
     *
     *  @param[in] other The affine form to copy.
     *
     *  @throw std::bad_alloc If memory allocation for the error terms fails.
     *                        Strong throw guarantee.
     */
    Affine(const Affine& other) = default;

    /** @brief Constructs an affine form by moving @p other.
     *
     *  @param[in] other The affine form to move.
     *
     *  @throw none No throw guarantee
     */
    Affine(Affine&& other) noexcept = default;

    /** @brief Assigns the value of @p other to this affine form.
     *
     *  @param[in] other The affine form to assign.
     *
     *  @return Reference to this affine form after assignment.
     *
     *  @throw std::bad_alloc If memory allocation for the error terms fails.
     *                        Strong throw guarantee.
     */
    Affine& operator=(const Affine& other) = default;

    /** @brief Moves the value of @p other to this affine form.
     *
     *  @param[in] other The affine form to move.
     *
     *  @return Reference to this affine form after assignment.
     *
     *  @throw none No throw guarantee.
     */
    Affine& operator=(Affine&& other) noexcept = default;

    // -- State Accessors -----------------------------------------------------

    /** @brief Returns the interval represented by the affine form.
     *
     *  Note that the interval returned by this method is always closed on both
     *  ends.
     *
     *  @return The interval represented by the affine form.
     *
     *  @throw none No throw guarantee
     */
    interval_t range() const;

    /** @brief Returns the center of the affine form.
     *
     *  @return The center of the affine form.
     *
     *  @throw std::domain_error If the affine form is empty (i.e., it has no
     *                      center and no error terms). Strong throw guarantee.
     */
    value_t center() const {
        assert_not_empty_();
        return *m_center_;
    }

    /** @brief Returns the error terms of the affine form.
     *
     *  The error terms are returned a map where the keys are symbolic
     *  representations of the error terms and and the values are the radii of
     *  the error terms. The symbols are opaque objects that uniquely identify
     *  each error source. The symbols themselves have no meaning outside of
     *  the context of the affine form except that they satisfy the property
     *  that if two error symbols are equal, then they represent the same error
     *  source.
     *
     *  @return The error terms of the affine form.
     *
     *  @throw None No throw guarantee.
     */
    const error_terms_t& error_terms() const { return m_error_terms_; }

    /** @brief Returns the radius of the affine form.
     *
     *  The radius is the sum of the absolute values of the error terms.
     *
     *  @return The radius of the affine form.
     *
     *  @throw std::domain_error If the affine form is empty (i.e., it has no
     *                      center and no error terms). Strong throw guarantee.
     */
    value_t radius() const;

    /** @brief Sets the center of the affine form.
     *
     *  This method allows the center of the affine form to be updated. The
     *  error terms are not modified by this method, so the radius of the affine
     *  form is not changed by this method.
     *
     *  @param[in] center The new center of the affine form.
     *
     *  @throw None No throw guarantee.
     */
    void set_center(value_t center) { m_center_ = center; }

    /** @brief Adds an error term to the affine form.
     *
     *  This method allows a new error term to be added to the affine form. The
     *  radius of the affine form is increased by the radius of the new error
     *  term. If the affine form is empty, the center is set to 0 before
     *  adding the error term (consistent with the idea that the resulting
     *  affine form should represent the interval [-radius, radius]).
     *
     *  @param[in] error_term The symbol representing the new error term.
     *  @param[in] radius The radius of the new error term.
     *
     *  @throw None No throw guarantee.
     */
    void add_error_term(error_term_t error_term, value_t radius) {
        if(empty()) set_center(value_t{0});
        m_error_terms_[error_term] = radius;
    }

    /** @brief Checks if the interval represented by *this contains @p value.
     *
     *  This method checks if a single value is contained within this->range().
     *  By definition, if *this is empty, this method will return false for all
     *  values.
     *
     *  @param[in] value The value to check.
     *
     *  @return True if the interval contains the value, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool contains(value_t value) const;

    /** @brief Checks if @p interval is contained within *this.
     *
     *  This method will ensure that each point in @p interval is contained
     *  in *this. Conceptually, this is equivalent to looping over values in
     *  @p interval and calling contains(value) for each value. In practice,
     *  this method only checks the endpoints of @p interval.
     *
     *  @param[in] interval The interval to check.
     *
     *  @return True if the interval contains the other interval, false
     * otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool contains(const interval_t& interval) const;

    /** @brief Checks if @p affine is contained within *this.
     *
     *  This method will ensure that each point in @p affine is contained
     *  in *this. In practice, this method is a convenience method for
     *  calling contains(affine.range()).
     *
     *  @param[in] affine The affine form to check.
     *
     *  @return True if the affine form contains the other affine form, false
     * otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool contains(const Affine& affine) const {
        return contains(affine.range());
    }

    /** @brief Checks if this affine form is representing an empty interval.
     *
     *  An affine form representing an empty interval has no center and no
     *  error terms. Viewed as an interval, it represents the empty set.
     *
     *  @return True if this affine form represents an empty interval, false
     *          otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool empty() const noexcept { return !m_center_; }

    /** @brief Creates a string of the affine form of *this.
     *
     *  The affine form is written as the center value +/- each of the error
     *  terms. If *this contains multiple error terms, the order in which the
     *  error terms are printed is NOT guaranteed to be the same across
     *  multiple runs of the program.
     *
     *  @return A string representation of the affine form of *this.
     *
     *  @throw std::bad_alloc If memory allocation for the string fails.
     *                      Strong throw guarantee.
     */
    std::string print_affine_form() const;

    /** @brief Creates a string of the interval form of *this.
     *
     *  The interval form of *this is the same as the affine form, except that
     *  the error terms are combined into a single radius term. Following,
     *  usual convention, the interval form is written in the form [center -
     *  radius, center + radius].
     *
     *  This is a convenience method for calling range().print_interval_form().
     *
     *  @return A string representation of the interval form of *this.
     *
     *  @throw std::bad_alloc If memory allocation for the string fails.
     *                      Strong throw guarantee.
     */
    std::string print_interval_form() const {
        return range().print_interval_form();
    }

    // -- Arithmetic Operators ------------------------------------------------

    /** @brief Returns the additive inverse of *this.
     *
     *  The additive inverse of an affine form is the affine form:
     *  @f[
     *   -x = -x_0 + \sum_{i=1}^n -x_i \epsilon_i
     *  @f]
     *
     *  @return The additive inverse of *this.
     *
     *  @throw std::bad_alloc If memory allocation for the new affine form
     *                        fails. Strong throw guarantee.
     */
    Affine operator-() const;

    /** @brief Overwrites *this with the sum of *this and @p value.
     *
     *  If *this is empty, the resulting affine form will have a center of
     *  @p value and no error terms. Otherwise this method will add @p value to
     *  the center of *this and leave the error terms unchanged.
     *
     *  @param[in] value The value to add to *this.
     *
     *  @return Reference to this affine form after addition.
     *
     *  @throw None No throw guarantee.
     */
    Affine& operator+=(value_t value) {
        if(empty()) { return *this = Affine(value); }
        (*m_center_) += value;
        return *this;
    }

    /** @brief Overwrites *this with the sum of *this and @p other.
     *
     *  Addition of affine forms @f$x@f$ and @f$y@f$ is defined as:
     *  @f[
     *  x + y = (x_0 + y_0) + \sum_{i=1}^n (x_i + y_i) \epsilon_i
     *  @f]
     *  Critically, the error terms are only summed if they have the same
     *  symbol. If an error term appears in one affine form but not the other,
     *  the resulting affine form will have an error term with the same symbol
     *  and the same radius as the original affine form.
     *
     *  @param[in] other The affine form to add to *this.
     *
     *  @return Reference to this affine form after addition.
     *
     *  @throw None No throw guarantee.
     */
    Affine& operator+=(const Affine& other);

    /** @brief Returns the sum of *this and @p value.
     *
     *  This is a convenience method for calling Affine(*this) += value. See
     *  the documentation for operator+=(value_t) for details on how addition
     *  with a scalar works.
     *
     *  @param[in] value The value to add to *this.
     *
     *  @return The sum of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new affine form
     *                        fails. Strong throw guarantee.
     */
    Affine operator+(value_t value) const { return Affine(*this) += value; }

    /** @brief Returns the sum of *this and @p other.
     *
     *  This is a convenience method for calling Affine(*this) += other. See
     *  the documentation for operator+=(Affine) for details on how addition
     *  with another affine form works.
     *
     *  @param[in] other The affine form to add to *this.
     *
     *  @return The sum of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new affine form
     *                        fails. Strong throw guarantee.
     */
    Affine operator+(const Affine& other) const {
        return Affine(*this) += other;
    }

    /** @brief Overwrites *this with the difference of *this and @p value.
     *
     *  If *this is empty, the resulting affine form will have a center of
     *  @p value and no error terms. Otherwise this method will subtract
     *  @p value from the center of *this and leave the error terms unchanged.
     *
     *  @param[in] value The value to subtract from *this.
     *
     *  @return Reference to this affine form after subtraction.
     *
     *  @throw None No throw guarantee.
     */
    Affine& operator-=(value_t value) {
        if(empty()) { return *this = Affine(-value); }
        (*m_center_) -= value;
        return *this;
    }

    /** @brief Overwrites *this with the difference of *this and @p other.
     *
     *  Subtraction of affine forms @f$x@f$ and @f$y@f$ is defined as:
     *  @f[
     *  x - y = (x_0 - y_0) + \sum_{i=1}^n (x_i - y_i) \epsilon_i
     *  @f]
     *  Critically, the error terms are only subtracted if they have the same
     *  symbol. If an error term appears in one affine form but not the other,
     *  the resulting affine form will have an error term with the same symbol
     *  and the same radius as the original affine form.
     *
     *  @param[in] other The affine form to subtract from *this.
     *
     *  @return Reference to this affine form after subtraction.
     *
     *  @throw None No throw guarantee.
     */
    Affine& operator-=(const Affine& other);

    /** @brief Returns the difference of *this and @p value.
     *
     *  This is a convenience method for calling `Affine(*this) -= value`. See
     *  the documentation for operator-=(value_t) for details on how subtraction
     *  with a scalar works.
     *
     *  @param[in] value The value to subtract from *this.
     *
     *  @return The difference of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new affine form
     *                        fails. Strong throw guarantee.
     */
    Affine operator-(value_t value) const { return Affine(*this) -= value; }

    /** @brief Returns the difference of *this and @p other.
     *
     *  This is a convenience method for calling `Affine(*this) -= other`. See
     *  the documentation for operator-=(Affine) for details on how subtraction
     *  with another affine form works.
     *
     *  @param[in] other The affine form to subtract from *this.
     *
     *  @return The difference of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new affine form
     *                        fails. Strong throw guarantee.
     */
    Affine operator-(const Affine& other) const {
        return Affine(*this) -= other;
    }

    /** @brief Overwrites *this with the product of *this and @p value.
     *
     *  The product of an affine form @f$x@f$ and a scalar @f$\alpha@f$ is
     *  defined as:
     *  @f[
     *  \alpha x = \alpha x_0 + \sum_{i=1}^n \alpha x_i \epsilon_i
     *  @f]
     *
     *  @param[in] value The value to multiply *this by.
     *
     *  @return Reference to this affine form after multiplication.
     *
     *  @throw None No throw guarantee.
     */
    Affine& operator*=(value_t value) {
        if(empty()) { return *this; }
        (*m_center_) *= value;
        for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
            error_term_i *= value;
        }
        return *this;
    }

    /** @brief Overwrites *this with the product of *this and @p other.
     *
     *  The product of two affine forms @f$x@f$ and @f$y@f$ is defined as:
     *  @f[
     *  x y = x_0 y_0 + \sum_{i=1}^n (x_0 y_i + x_i y_0) \epsilon_i +
     *        xy_{n+1} \epsilon_{n+1}
     *  @f]
     *  where @f$xy_{n+1}@f$ is an error term that captures the error from the
     *  nonlinearity of multiplication. The current implementation of the Affine
     *  class uses:
     *  @f[
     *  xy_{n+1} = \sum_{i=1}^n \sum_{j=1}^n |x_i y_j|
     *  @f]
     *  which is the product of the radii of the error terms of @f$x@f$ and
     *  @f$y@f$.
     *
     *  @param[in] other The affine form to multiply *this by.
     *
     *  @return Reference to this affine form after multiplication.
     *
     *  @throw None No throw guarantee.
     */
    Affine& operator*=(const Affine& other);

    /** @brief Returns the product of *this and @p value.
     *
     *  This is a convenience method for calling `Affine(*this) *= value`. See
     *  the documentation for `operator*=(value_t)` for details on how
     *  multiplication with a scalar works.
     *
     *  @param[in] value The value to multiply *this by.
     *
     *  @return The product of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new affine form
     *                        fails. Strong throw guarantee.
     */
    Affine operator*(value_t value) const { return Affine(*this) *= value; }

    /** @brief Returns the product of *this and @p other.
     *
     *  This is a convenience method for calling `Affine(*this) *= other`. See
     *  the documentation for `operator*=(Affine)` for details on how
     *  multiplication with another affine form works.
     *
     *  @param[in] other The affine form to multiply *this by.
     *
     *  @return The product of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new affine form
     *                        fails. Strong throw guarantee.
     */
    Affine operator*(const Affine& other) const {
        return Affine(*this) *= other;
    }

    /** @brief Overwrites *this with the quotient of *this and @p value.
     *
     *  The quotient of an affine form @f$x@f$ and a scalar @f$\alpha@f$ is
     *  defined as:
     *  @f[
     *    \frac{x}{\alpha} = \frac{x_0}{\alpha} +
     *    \sum_{i=1}^n \frac{x_i}{\alpha} \epsilon_i
     *  @f]
     *
     *  @param[in] value The value to divide *this by.
     *
     *  @return Reference to this affine form after division.
     *
     *  @throw std::domain_error If @p value is zero.
     */
    Affine& operator/=(value_t value) {
        if(value == 0) { throw std::domain_error("Division by zero"); }
        return *this *= value_t(1.0 / value);
    }

    /** @brief Overwrites *this with the quotient of *this and @p other.
     *
     *  This method is implemented as the product of *this and
     *  other.multiplicative_inverse(). See the documentation for
     *  `operator*=(Affine)` and multiplicative_inverse() for more details on
     *  how these operations work.
     *
     *  @param[in] other The affine form to divide *this by.
     *
     *  @return Reference to this affine form after division.
     *
     *  @throw std::domain_error If @p other is empty or has a zero center.
     *                           Strong throw guarantee.
     *  @throw std::bad_alloc If memory allocation for the multiplicative
     *                        inverse of @p other fails. Strong throw guarantee.
     */
    Affine& operator/=(const Affine& other);

    /** @brief Returns the quotient of *this and @p value.
     *
     *  This is a convenience method for calling `Affine(*this) /= value`. See
     *  the documentation for operator/=(value_t) for details on how division
     *  with a scalar works.
     *
     *  @param[in] value The value to divide *this by.
     *
     *  @return The quotient of *this and @p value.
     *
     *  @throw std::domain_error If @p value is zero.
     */
    Affine operator/(value_t value) const { return Affine(*this) /= value; }

    /** @brief Returns the quotient of *this and @p other.
     *
     *  This is a convenience method for calling `Affine(*this) /= other`. See
     *  the documentation for operator/=(Affine) for details on how division
     *  with another affine form works.
     *
     *  @param[in] other The affine form to divide *this by.
     *
     *  @return The quotient of *this and @p other.
     *
     *  @throw std::domain_error If @p other is empty or has a zero center.
     *                           Strong throw guarantee.
     *  @throw std::bad_alloc If memory allocation for the multiplicative
     *                        inverse of @p other fails. Strong throw guarantee.
     */
    Affine operator/(const Affine& other) const {
        return Affine(*this) /= other;
    }

    /** @brief Applies an affine transformation to *this.
     *
     *  Affine transformations are linear transformations characterized by two
     *  parameters: a scale factor and a shift. Often we will want to apply a
     *  non-affine transformation. To do this, we find an affine transformation
     *  that approximates the non-affine transformation (i.e., a scale factor
     *  and a shift for the approximate transformation) and an estimate of the
     *  error from using the approximate transformation.
     *
     *  N.b. applying an affine transformation to an empty affine form will
     *  result in the affine form @f$\zeta \pm \delta@f$.
     *
     *  @param[in] alpha The scale factor.
     *  @param[in] zeta The shift.
     *  @param[in] delta The error term.
     *
     *  @return The affine form resulting from applying the affine
     *          transformation to *this.
     *
     *  @throw std::bad_alloc If memory allocation for the new affine form
     *                        fails. Strong throw guarantee.
     */
    Affine apply_affine_transform(value_t alpha, value_t zeta,
                                  value_t delta) const {
        value_t new_center = empty() ? zeta : alpha * center() + zeta;
        error_terms_t new_error_terms;
        for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
            new_error_terms[error_symbol] = alpha * error_term_i;
        }
        new_error_terms[make_error_term()] = delta;
        return Affine(new_center, std::move(new_error_terms));
    }

    /** @brief Returns the multiplicative inverse of *this.
     *
     *  The multiplicative inverse of an affine form @f$x@f$ is obtained by
     *  applying an affine transformation to *this. The parameters of this
     *  transformation are given by:
     *  @f[
     *  \begin{align}
     *    a      &= \min(|lo|, |hi|) \\
     *    b      &= \max(|lo|, |hi|) \\
     *    \alpha &= -\frac{1}{b^2} \\
     *    i &= \left[\frac{1}{a} - \alpha a, \frac{2}{b}\right]\\
     *    \zeta &= |midpoint(i)|\\
     *    \delta &= radius(i)
     *   \end{align}
     *  @f]
     *
     *  Where @f$lo@f$ and @f$hi@f$ are the lower and upper bounds of *this.
     *
     *  @return The multiplicative inverse of *this.
     *
     *  @throw std::domain_error If *this is empty or contains zero. Strong
     *                           throw guarantee.
     */
    Affine multiplicative_inverse() const;

    // -- Comparison Operators ------------------------------------------------

    /** @brief Checks if *this and @p other represent the same affine form.
     *
     *  Two affine forms are considered equal if they are both empty or if they
     *  have the same center and the same error terms. Note that the order in
     *  which the error terms are stored in the error_terms_t map is not
     *  guaranteed to be the same, but this method does not check the order of
     *  the error terms, only that each symbol is present.
     *
     *  @param[in] other The affine form to compare with *this.
     *
     *  @return True if *this and @p other represent the same affine form, false
     *          otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator==(const Affine& other) const {
        if(empty() != other.empty()) { return false; }
        if(empty()) { return true; }
        if(m_center_ != other.m_center_) { return false; }
        return m_error_terms_ == other.m_error_terms_;
    }

    /** @brief Checks if *this and @p other represent different affine
     * forms.
     *
     *  Two affine forms are considered different if they are not equal
     *  according to the definition of equality given in operator==. See the
     *  documentation for operator== for details on how affine forms are
     *  compared.
     *
     *  @param[in] other The affine form to compare with *this.
     *
     *  @return True if *this and @p other represent different affine forms,
     *          false otherwise.
     *
     *  @throw none No throw guarantee.
     */
    bool operator!=(const Affine& other) const { return !(*this == other); }

    /// Returns a process-wide unique integer ID for a new error term symbol.
    static error_term_t make_error_term() {
        static std::atomic<size_type> s_next_id{0};
        return s_next_id.fetch_add(1);
    }

private:
    /// Asserts that *this is not empty and throws domain_error if it is.
    void assert_not_empty_() const {
        if(empty()) { throw std::domain_error("Affine form is empty"); }
    }

    /// This is the center of the affine form. If empty, m_center_ =
    /// nullopt.
    std::optional<value_t> m_center_;

    /// Map from error symbols to their radii. If empty, m_error_terms_ =
    /// {}.
    error_terms_t m_error_terms_;
};

// -- Non-member functions
// ---------------------------------------------------

/** @brief Outputs the range of an affine form to an output stream.
 *
 *  @related Affine
 *  @tparam ValueType The type of the values in the affine form.
 *
 *  This method will defer to the output operator for interval_t to print
 * the value of the affine form. See the documentation for interval_t for
 * details on how intervals are printed.
 *
 *  @param[in] os The output stream to write to.
 *  @param[in] a The affine form to output.
 *
 *  @return The output stream.
 *
 *  @throw none No throw guarantee.
 */
template<typename ValueType>
std::ostream& operator<<(std::ostream& os, const Affine<ValueType>& a) {
    os << a.range();
    return os;
}

/** @brief Multiplies a scalar by an affine form.
 *
 *  @related Affine
 *  @tparam ValueType The type of the values in the affine form.
 *
 *  This method implements multiplication of a scalar by an affine form. It
 *  is a convenience method for calling `Affine(*this) *= value`. See the
 *  documentation for `operator*=(value_t)` for details on how multiplication
 * with a scalar works.
 *
 *  @param[in] value The scalar to multiply by.
 *  @param[in] a The affine form to multiply.
 *
 *  @return The product of the scalar and the affine form.
 *
 *  @throw std::bad_alloc If memory allocation for the new affine form
 * fails. Strong throw guarantee.
 */
template<typename ValueType>
Affine<ValueType> operator*(ValueType value, const Affine<ValueType>& a) {
    return a * value;
}

// -- Out-of-line definitions
// ------------------------------------------------

template<typename ValueType>
auto Affine<ValueType>::range() const -> interval_t {
    if(empty()) { return interval_t(); }
    auto r = radius();
    return interval_t(center() - r, center() + r);
}

template<typename ValueType>
auto Affine<ValueType>::radius() const -> value_t {
    assert_not_empty_();
    value_t r = 0;
    for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
        r += std::fabs(error_term_i);
    }
    return r;
}

template<typename ValueType>
auto Affine<ValueType>::contains(value_t value) const -> bool {
    if(empty()) { return false; }
    return range().contains(value);
}

template<typename ValueType>
auto Affine<ValueType>::contains(const interval_t& interval) const -> bool {
    if(interval.empty()) { return true; }
    if(empty()) { return false; }
    return range().contains(interval);
}

template<typename ValueType>
std::string Affine<ValueType>::print_affine_form() const {
    if(empty()) { return "∅"; }
    std::stringstream ss;
    ss << center();
    for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
        ss << " +/- " << error_term_i;
    }
    return ss.str();
}

template<typename ValueType>
auto Affine<ValueType>::operator-() const -> Affine {
    if(empty()) { return *this; }
    value_t new_center = -center();
    error_terms_t new_error_terms;
    for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
        new_error_terms[error_symbol] = -error_term_i;
    }
    return Affine(new_center, new_error_terms);
}

template<typename ValueType>
auto Affine<ValueType>::operator+=(const Affine& other) -> Affine& {
    if(empty()) { return *this = other; }
    if(other.empty()) { return *this; }
    value_t new_center            = center() + other.center();
    error_terms_t new_error_terms = m_error_terms_;
    for(auto&& [error_symbol, error_term_i] : other.m_error_terms_) {
        new_error_terms[error_symbol] += error_term_i;
    }
    return *this = Affine(new_center, new_error_terms);
}

template<typename ValueType>
auto Affine<ValueType>::operator-=(const Affine& other) -> Affine& {
    return *this += -other;
}

template<typename ValueType>
auto Affine<ValueType>::operator*=(const Affine& other) -> Affine& {
    if(empty() || other.empty()) { return *this = Affine(); }
    value_t new_center = center() * other.center();
    error_terms_t new_error_terms;
    value_t new_radius = 0;
    for(auto&& [error_symbol, error_term_i] : m_error_terms_) {
        new_error_terms[error_symbol] = error_term_i * other.center();
        new_radius += std::fabs(new_error_terms[error_symbol]);
    }
    for(auto&& [error_symbol, error_term_j] : other.m_error_terms_) {
        new_error_terms[error_symbol] += error_term_j * center();
        new_radius += std::fabs(new_error_terms[error_symbol]);
    }
    auto correction                    = radius() * other.radius();
    new_error_terms[make_error_term()] = correction;
    return *this                       = Affine(new_center, new_error_terms);
}

template<typename ValueType>
auto Affine<ValueType>::operator/=(const Affine& other) -> Affine& {
    // Multiply by this by 1 / other
    return *this *= other.multiplicative_inverse();
}

template<typename ValueType>
auto Affine<ValueType>::multiplicative_inverse() const -> Affine {
    assert_not_empty_();
    if(contains(0)) { throw std::domain_error("Division by zero"); }
    // Compute the affine transformation which transforms other to 1 / other
    auto other_range = range();
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
    return apply_affine_transform(alpha, zeta, delta);
}

/// Typedef for an affine form of floats
using AFloat = Affine<float>;

/// Typedef for an affine form of doubles
using ADouble = Affine<double>;

} // namespace sigma

#include <sigma/affine/eigen_compat.hpp>
#include <sigma/affine/operations/operations.hpp>
