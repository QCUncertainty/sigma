#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <sigma/detail/pow.hpp>
#include <sigma/taylor/operations/series_detail.hpp>
#include <sigma/taylor/taylor.hpp>
#include <utility>

/** @file taylor_model.hpp
 *  @brief Defines the TaylorModel class
 */

namespace sigma {

/** @brief Pairs a Taylor polynomial with a remainder interval to guarantee
 *         Eq. \f$\eqref{eq:tm-inclusion}\f$.
 *
 * @tparam ValueType The numeric type of the constant term, coefficients, and
 *                    remainder bounds.
 *
 * Where Taylor alone makes no claim to enclose anything, `TaylorModel` does:
 * for every @f$\delta \in [-1,1]^v@f$, `range()` -- @f$B(P) + I@f$ -- encloses
 * the modeled function. Every operation below exists to preserve that
 * property (docs/taylor.md's "Every operation defined below exists to
 * preserve Eq. \f$\eqref{eq:tm-inclusion}\f$"): where Taylor's own arithmetic
 * would silently discard a term it cannot represent, TaylorModel instead
 * bounds that term with Taylor::bound() and folds it into the remainder.
 *
 * Structurally this follows ThresholdedAffine: composition rather than
 * inheritance, forwarding accessors, a `polynomial()` accessor exposing the
 * underlying Taylor, and mutating operators that delegate and then repair
 * the remainder. Unlike ThresholdedAffine, nothing here is auto-applied after
 * every operation -- sweep_to_order() and sweep_small() are explicit,
 * user-invoked tools for controlling term count, matching the discipline Eq.
 * \f$\eqref{eq:tm-inclusion}\f$ demands: dropping a term without bounding it
 * would invalidate the model, not merely loosen it.
 *
 * Elementary functions (`sqrt`, `exp`, `log`, `pow`) and division are
 * implemented as free functions and a `multiplicative_inverse` member,
 * respectively, using the private `compose_` hook: it evaluates the
 * polynomial part exactly as `Taylor::compose_` does, then bounds the
 * discarded tail of the outer series with the Lagrange remainder of Eq.
 * \f$\eqref{eq:tm-outer-rem}\f$ and folds it into the returned model's
 * remainder, so Eq. \f$\eqref{eq:tm-inclusion}\f$ survives the operation
 * (see docs/taylor.md's "Elementary functions" section). Antiderivation is
 * still deferred; see docs/taylor.md's note on Eq.
 * \f$\eqref{eq:tm-antideriv}\f$ -- nothing on this page depends on it.
 */
template<typename ValueType>
class TaylorModel {
public:
    /// Type used for indexing, offsets, and truncation order
    using size_type = std::size_t;

    /// Type used for storing floating point values
    using value_t = ValueType;

    /// Type of the underlying Taylor polynomial
    using taylor_t = Taylor<ValueType>;

    /// Opaque type used to identify an expansion variable
    using deviation_t = typename taylor_t::deviation_t;

    /// Sparse exponent multi-index type
    using monomial_t = typename taylor_t::monomial_t;

    /// Type used to map monomials to their coefficients
    using coeffs_t = typename taylor_t::coeffs_t;

    /// Type of an interval
    using interval_t = typename taylor_t::interval_t;

    /// Tag type used to pass the truncation order to constructors; see
    /// Taylor::Order.
    using Order = typename taylor_t::Order;

    /// The truncation order used when none is specified.
    static constexpr Order default_max_order() {
        return taylor_t::default_max_order();
    }

    // --- Constructors and Assignment ----------------------------------------

    /** @brief Constructs an empty TaylorModel.
     *
     *  An empty TaylorModel represents the empty set, exactly as an empty
     *  Taylor does.
     *
     *  @throw none No throw guarantee
     */
    TaylorModel() = default;

    /** @brief Constructs a TaylorModel from a constant value.
     *
     *  The resulting model has no non-constant coefficients, an empty
     *  remainder, and represents the single value given by @p center.
     *
     *  @param[in] center The constant term of the polynomial.
     *  @param[in] order  The truncation order, defaults to default_max_order().
     *
     *  @throw none No throw guarantee
     */
    TaylorModel(value_t center, Order order = default_max_order()) :
      TaylorModel(interval_t(center, center), order) {}

    /** @brief Constructs a TaylorModel from a lower and upper bound.
     *
     *  @param[in] lo    The lower bound.
     *  @param[in] hi    The upper bound.
     *  @param[in] order The truncation order, defaults to default_max_order().
     *
     *  @throw std::bad_alloc If memory allocation for the coefficient fails.
     *                        Strong throw guarantee.
     */
    TaylorModel(value_t lo, value_t hi, Order order = default_max_order()) :
      TaylorModel(interval_t(lo, hi), order) {}

    /** @brief Constructs a TaylorModel from an interval.
     *
     *  For a non-empty interval, the underlying polynomial is
     *  `Taylor(interval, order)`. If @p order is at least 1, that polynomial
     *  represents @p interval exactly (a single linear term on a freshly
     *  minted expansion variable), so the remainder is empty -- matching the
     *  `Affine`: order 1, empty remainder row of docs/taylor.md's
     *  equivalence table. If @p order is 0, Taylor has nowhere to put the
     *  linear term, so it is dropped from the polynomial and its full
     *  contribution becomes the remainder instead, preserving Eq.
     *  \f$\eqref{eq:tm-inclusion}\f$. If @p interval is empty, the resulting
     *  model is also empty.
     *
     *  @param[in] interval The interval represented by the model.
     *  @param[in] order    The truncation order, defaults to
     *                      default_max_order().
     *
     *  @throw std::bad_alloc If memory allocation for the coefficient fails.
     *                        Strong throw guarantee.
     */
    explicit TaylorModel(const interval_t& interval,
                         Order order = default_max_order()) :
      m_polynomial_(interval, order) {
        if(interval.empty()) { return; }
        auto radius = interval.radius();
        if(radius > 0 && m_polynomial_.coefficients().empty()) {
            m_remainder_ = interval_t(-radius, radius);
        } else {
            m_remainder_ = interval_t(value_t{0}, value_t{0});
        }
    }

    /** @brief Constructs a TaylorModel by re-wrapping a polynomial and a
     *         remainder.
     *
     *  Used internally by operations that produce a Taylor and a remainder
     *  interval separately and need to combine them into a TaylorModel. This
     *  ctor does not itself verify Eq. \f$\eqref{eq:tm-inclusion}\f$ --
     *  callers are responsible for choosing a @p remainder that makes the
     *  invariant hold.
     *
     *  @param[in] polynomial The polynomial part of the model.
     *  @param[in] remainder  The remainder interval.
     *
     *  @throw none No throw guarantee
     */
    TaylorModel(taylor_t polynomial, interval_t remainder) :
      m_polynomial_(std::move(polynomial)),
      m_remainder_(std::move(remainder)) {}

    /// Makes a deep copy of @p other.
    TaylorModel(const TaylorModel& other) = default;

    /// Constructs a TaylorModel by moving @p other.
    TaylorModel(TaylorModel&& other) noexcept = default;

    /// Assigns the value of @p other to this model.
    TaylorModel& operator=(const TaylorModel& other) = default;

    /// Moves the value of @p other to this model.
    TaylorModel& operator=(TaylorModel&& other) noexcept = default;

    // -- State Accessors -----------------------------------------------------

    /** @brief Returns the underlying polynomial.
     *
     *  @return Const reference to the polynomial part of *this.
     *
     *  @throw none No throw guarantee
     */
    const taylor_t& polynomial() const noexcept { return m_polynomial_; }

    /** @brief Returns the remainder interval.
     *
     *  @return Const reference to the remainder interval @f$I@f$.
     *
     *  @throw none No throw guarantee
     */
    const interval_t& remainder() const noexcept { return m_remainder_; }

    /// Forwards to `polynomial().max_order()`.
    size_type max_order() const noexcept { return m_polynomial_.max_order(); }

    /// Forwards to `polynomial().constant()`.
    value_t constant() const { return m_polynomial_.constant(); }

    /// Forwards to `polynomial().coefficients()`.
    const coeffs_t& coefficients() const noexcept {
        return m_polynomial_.coefficients();
    }

    /// Forwards to `polynomial().n_terms()`.
    size_type n_terms() const noexcept { return m_polynomial_.n_terms(); }

    /** @brief Checks if this TaylorModel represents the empty set.
     *
     *  @return True if *this represents the empty set, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool empty() const noexcept { return m_polynomial_.empty(); }

    /** @brief Returns an interval enclosing the values *this models.
     *
     *  This is @f$B(P) + I@f$: the polynomial's bound (Taylor::bound(),
     *  currently the quadratic fast bounder) plus the remainder. Together
     *  with the constructors and arithmetic below, this is what guarantees
     *  Eq. \f$\eqref{eq:tm-inclusion}\f$.
     *
     *  @return An interval enclosing the values of *this.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     */
    interval_t range() const { return m_polynomial_.bound() + m_remainder_; }

    /** @brief Checks if *this contains @p value.
     *
     *  @param[in] value The value to check.
     *
     *  @return True if range() contains @p value, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool contains(value_t value) const {
        if(empty()) { return false; }
        return range().contains(value);
    }

    /** @brief Checks if *this contains @p interval.
     *
     *  @param[in] interval The interval to check.
     *
     *  @return True if range() contains @p interval, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool contains(const interval_t& interval) const {
        if(interval.empty()) { return true; }
        if(empty()) { return false; }
        return range().contains(interval);
    }

    /** @brief Checks if *this contains @p other.
     *
     *  @param[in] other The TaylorModel to check.
     *
     *  @return True if range() contains @p other's range, false otherwise.
     *          True (vacuously) if @p other is empty.
     *
     *  @throw none No throw guarantee
     */
    bool contains(const TaylorModel& other) const {
        if(other.empty()) { return true; }
        return contains(other.range());
    }

    /** @brief Creates a string of the polynomial form of *this.
     *
     *  This is a convenience method for calling
     *  polynomial().print_polynomial_form(); the remainder is not included.
     *
     *  @return A string representation of the polynomial part of *this.
     *
     *  @throw std::bad_alloc If memory allocation for the string fails.
     *                        Strong throw guarantee.
     */
    std::string print_polynomial_form() const {
        return m_polynomial_.print_polynomial_form();
    }

    /** @brief Creates a string of the interval form of *this.
     *
     *  This is a convenience method for calling range().print_interval_form().
     *
     *  @return A string representation of the interval form of *this.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     *  @throw std::bad_alloc If memory allocation for the string fails.
     *                        Strong throw guarantee.
     */
    std::string print_interval_form() const {
        return range().print_interval_form();
    }

    /// Forwards to `Taylor::make_deviation()`.
    static deviation_t make_deviation() { return taylor_t::make_deviation(); }

    // -- Arithmetic Operators ------------------------------------------------

    /** @brief Returns the additive inverse of *this.
     *
     *  Negates both the polynomial and the remainder: if
     *  @f$f(x) \in P(\delta) + I@f$ then @f$-f(x) \in -P(\delta) + (-I)@f$.
     *
     *  @return The additive inverse of *this.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel operator-() const {
        if(empty()) { return *this; }
        return TaylorModel(-m_polynomial_, -m_remainder_);
    }

    /** @brief Overwrites *this with the sum of *this and @p value.
     *
     *  Adding a scalar shifts the constant term only; it introduces no new
     *  uncertainty, so the remainder is unaffected (or, if *this was empty,
     *  set to the empty interval @f$[0,0]@f$).
     *
     *  @param[in] value The value to add to *this.
     *
     *  @return Reference to this model after addition.
     *
     *  @throw none No throw guarantee
     */
    TaylorModel& operator+=(value_t value) {
        if(empty()) {
            m_polynomial_ = taylor_t(value, Order(m_polynomial_.max_order()));
            m_remainder_  = interval_t(value_t{0}, value_t{0});
            return *this;
        }
        m_polynomial_ += value;
        return *this;
    }

    /** @brief Overwrites *this with the sum of *this and @p other.
     *
     *  Implements Eq. \f$\eqref{eq:tm-add}\f$: @f$(P_1+P_2,\, I_1+I_2)@f$. If
     *  *this and @p other have different truncation orders, the
     *  higher-order operand is first reduced with sweep_to_order() -- which
     *  bounds its excess terms into its own remainder rather than dropping
     *  them -- so the two polynomials being added are always at equal order
     *  and the sum never has terms to discard (see docs/taylor.md's "Mixed
     *  orders resolve downward").
     *
     *  @param[in] other The model to add to *this.
     *
     *  @return Reference to this model after addition.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel& operator+=(const TaylorModel& other);

    /** @brief Returns the sum of *this and @p value.
     *
     *  This is a convenience method for calling `TaylorModel(*this) += value`.
     *
     *  @param[in] value The value to add to *this.
     *
     *  @return The sum of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel operator+(value_t value) const {
        return TaylorModel(*this) += value;
    }

    /** @brief Returns the sum of *this and @p other.
     *
     *  This is a convenience method for calling `TaylorModel(*this) += other`.
     *
     *  @param[in] other The model to add to *this.
     *
     *  @return The sum of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel operator+(const TaylorModel& other) const {
        return TaylorModel(*this) += other;
    }

    /** @brief Overwrites *this with the difference of *this and @p value.
     *
     *  @param[in] value The value to subtract from *this.
     *
     *  @return Reference to this model after subtraction.
     *
     *  @throw none No throw guarantee
     */
    TaylorModel& operator-=(value_t value) {
        if(empty()) {
            m_polynomial_ = taylor_t(-value, Order(m_polynomial_.max_order()));
            m_remainder_  = interval_t(value_t{0}, value_t{0});
            return *this;
        }
        m_polynomial_ -= value;
        return *this;
    }

    /** @brief Overwrites *this with the difference of *this and @p other.
     *
     *  This is implemented as `*this += -other`. See the documentation for
     *  operator+=(TaylorModel) and operator-() for details.
     *
     *  @param[in] other The model to subtract from *this.
     *
     *  @return Reference to this model after subtraction.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel& operator-=(const TaylorModel& other) {
        return *this += -other;
    }

    /** @brief Returns the difference of *this and @p value.
     *
     *  This is a convenience method for calling `TaylorModel(*this) -= value`.
     *
     *  @param[in] value The value to subtract from *this.
     *
     *  @return The difference of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel operator-(value_t value) const {
        return TaylorModel(*this) -= value;
    }

    /** @brief Returns the difference of *this and @p other.
     *
     *  This is a convenience method for calling `TaylorModel(*this) -= other`.
     *
     *  @param[in] other The model to subtract from *this.
     *
     *  @return The difference of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel operator-(const TaylorModel& other) const {
        return TaylorModel(*this) -= other;
    }

    /** @brief Overwrites *this with the product of *this and @p value.
     *
     *  Scalar multiplication is linear, so both the polynomial and the
     *  remainder are scaled by @p value.
     *
     *  @param[in] value The value to multiply *this by.
     *
     *  @return Reference to this model after multiplication.
     *
     *  @throw none No throw guarantee
     */
    TaylorModel& operator*=(value_t value) {
        if(empty()) { return *this; }
        m_polynomial_ *= value;
        m_remainder_ *= value;
        return *this;
    }

    /** @brief Overwrites *this with the product of *this and @p other.
     *
     *  Implements Eq. \f$\eqref{eq:tm-mul}\f$. As in operator+=(TaylorModel),
     *  mixed orders are first resolved by sweeping the higher-order operand
     *  down with sweep_to_order(), so the base formula only ever has to
     *  handle two equal-order polynomials:
     *  @f[
     *  \left(P_1, I_1\right) \cdot \left(P_2, I_2\right) =
     *  \left(
     *    \left(P_1 P_2\right)_{\leq n},\;
     *    B\left(\left(P_1 P_2\right)_{>n}\right) +
     *    B\left(P_1\right) I_2 +
     *    B\left(P_2\right) I_1 +
     *    I_1 I_2
     *  \right)
     *  @f]
     *  Unlike Taylor::operator*=(Taylor), which simply discards
     *  @f$(P_1 P_2)_{>n}@f$, this bounds it with Taylor::bound() and folds
     *  it into the remainder, so Eq. \f$\eqref{eq:tm-inclusion}\f$ survives
     *  the operation.
     *
     *  @param[in] other The model to multiply *this by.
     *
     *  @return Reference to this model after multiplication.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel& operator*=(const TaylorModel& other);

    /** @brief Returns the product of *this and @p value.
     *
     *  This is a convenience method for calling `TaylorModel(*this) *= value`.
     *
     *  @param[in] value The value to multiply *this by.
     *
     *  @return The product of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel operator*(value_t value) const {
        return TaylorModel(*this) *= value;
    }

    /** @brief Returns the product of *this and @p other.
     *
     *  This is a convenience method for calling `TaylorModel(*this) *= other`.
     *
     *  @param[in] other The model to multiply *this by.
     *
     *  @return The product of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel operator*(const TaylorModel& other) const {
        return TaylorModel(*this) *= other;
    }

    /** @brief Overwrites *this with the quotient of *this and @p value.
     *
     *  @param[in] value The value to divide *this by.
     *
     *  @return Reference to this model after division.
     *
     *  @throw std::domain_error If @p value is zero.
     */
    TaylorModel& operator/=(value_t value) {
        if(value == 0) { throw std::domain_error("Division by zero"); }
        return *this *= value_t(1.0 / value);
    }

    /** @brief Overwrites *this with the quotient of *this and @p other.
     *
     *  Implemented as the product of *this and
     *  other.multiplicative_inverse(). See the documentation for
     *  operator*=(TaylorModel) and multiplicative_inverse() for details.
     *
     *  @param[in] other The model to divide *this by.
     *
     *  @return Reference to this model after division.
     *
     *  @throw std::domain_error If @p other is empty or 0 is in its range().
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel& operator/=(const TaylorModel& other) {
        return *this *= other.multiplicative_inverse();
    }

    /** @brief Returns the quotient of *this and @p value.
     *
     *  This is a convenience method for calling `TaylorModel(*this) /= value`.
     *
     *  @param[in] value The value to divide *this by.
     *
     *  @return The quotient of *this and @p value.
     *
     *  @throw std::domain_error If @p value is zero.
     */
    TaylorModel operator/(value_t value) const {
        return TaylorModel(*this) /= value;
    }

    /** @brief Returns the quotient of *this and @p other.
     *
     *  This is a convenience method for calling `TaylorModel(*this) /= other`.
     *
     *  @param[in] other The model to divide *this by.
     *
     *  @return The quotient of *this and @p other.
     *
     *  @throw std::domain_error If @p other is empty or 0 is in its range().
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel operator/(const TaylorModel& other) const {
        return TaylorModel(*this) /= other;
    }

    /** @brief Returns the multiplicative inverse of *this.
     *
     *  Composes the outer series of @f$1/x@f$ about constant() with *this via
     *  compose_(), which folds the Lagrange remainder into the result. See
     *  compose_() for details.
     *
     *  @return The multiplicative inverse of *this.
     *
     *  @throw std::domain_error If *this is empty or 0 is in range().
     *                           Strong throw guarantee.
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel multiplicative_inverse() const;

    /** @brief Evaluates a Taylor-model outer series about constant(), with a
     *         Lagrange remainder bound.
     *
     *  This is the TaylorModel analogue of Taylor::compose_ -- the hook every
     *  elementary function is built from. It computes the polynomial part
     *  exactly as `polynomial().compose_(outer_coeffs)` does (see
     *  Taylor::compose_), then bounds the discarded tail of the outer series
     *  with the Lagrange remainder of Eq. \f$\eqref{eq:tm-outer-rem}\f$
     *
     *  @f[
     *  R_{n} \in
     *  \frac{1}{\left(n+1\right)!}
     *  \left(B\left(\bar{P}\right) + I\right)^{n+1}
     *  \frac{d^{n+1}g}{df^{n+1}}\Big(
     *    c_f + \left[0, 1\right] \cdot \left(B\left(\bar{P}\right) +
     *    I\right)
     *  \Big)
     *  @f]
     *
     *  @param[in] outer_coeffs The Taylor coefficients of the outer function
     *                          @f$g@f$ about constant(), starting from k=0;
     *                          forwarded verbatim to
     *                          `polynomial().compose_`.
     *  @param[in] nth_derivative A callable computing @f$\frac{d^{n+1}g}
     *                            {df^{n+1}}@f$, evaluated in interval
     *                            arithmetic over the argument range in Eq.
     *                            \f$\eqref{eq:tm-outer-rem}\f$ above, where
     *                            @f$n@f$ = max_order().
     *
     *  @return A TaylorModel of @f$g(f)@f$, where @f$f@f$ is the function
     *          modeled by *this.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     *  @throw std::bad_alloc If memory allocation for the new model fails.
     *                        Strong throw guarantee.
     */
    TaylorModel compose_(
      const std::vector<value_t>& outer_coeffs,
      const std::function<interval_t(const interval_t&)>& nth_derivative) const;

    // -- Rigorous Term Control (no Affine analogue) ---------------------------

    /** @brief Reduces *this to a lower truncation order, bounding the
     *         removed terms into the remainder.
     *
     *  Every coefficient whose monomial has degree above @p new_order is
     *  removed from the polynomial; their combined contribution is bounded
     *  with Taylor::bound() and added to the remainder. Unlike
     *  Taylor::truncate(), which simply discards those terms, this preserves
     *  Eq. \f$\eqref{eq:tm-inclusion}\f$ -- the enclosure gets wider, never
     *  invalid. If @p new_order is greater than or equal to max_order(),
     * nothing is removed and the remainder is unchanged.
     *
     *  @param[in] new_order The truncation order of the result.
     *
     *  @return *this, reduced to @p new_order.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     */
    TaylorModel sweep_to_order(size_type new_order) const;

    /** @brief Removes individually small coefficients, bounding them into
     *         the remainder.
     *
     *  Serves the purpose that `ThresholdedAffine`'s automatic thresholding
     *  serves -- controlling how many terms are carried -- but rigorously:
     *  a coefficient @f$c_\beta@f$ is removed when
     *  @f$|c_\beta| / \mathrm{radius}(\mathrm{range}()) < \f$ @p threshold
     *  (every monomial is bounded in magnitude by 1 over @f$[-1,1]^v@f$, so
     *  @f$|c_\beta|@f$ is exactly that term's maximum contribution), and its
     *  contribution is bounded with Taylor::bound() and added to the
     *  remainder rather than discarded.
     *
     *  @param[in] threshold The relative threshold below which a
     *                       coefficient is swept into the remainder.
     *
     *  @return *this, with small coefficients swept into the remainder.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     */
    TaylorModel sweep_small(value_t threshold) const;

    // -- Comparison Operators ------------------------------------------------

    /** @brief Checks if *this and @p other represent the same model.
     *
     *  Two TaylorModels are equal if they are both empty, or if they have
     *  the same polynomial and the same remainder.
     *
     *  @param[in] other The model to compare with *this.
     *
     *  @return True if *this and @p other represent the same model, false
     *          otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool operator==(const TaylorModel& other) const {
        if(empty() != other.empty()) { return false; }
        if(empty()) { return true; }
        return m_polynomial_ == other.m_polynomial_ &&
               m_remainder_ == other.m_remainder_;
    }

    /** @brief Checks if *this and @p other represent different models.
     *
     *  @param[in] other The model to compare with *this.
     *
     *  @return True if *this and @p other represent different models, false
     *          otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool operator!=(const TaylorModel& other) const {
        return !(*this == other);
    }

private:
    /// Asserts that *this is not empty and throws domain_error if it is.
    void assert_not_empty_() const {
        if(empty()) { throw std::domain_error("TaylorModel is empty"); }
    }

    /// The polynomial part of the model.
    taylor_t m_polynomial_;

    /// The remainder interval. Empty exactly when m_polynomial_ is empty.
    interval_t m_remainder_;
};

// -- Non-member functions -------------------------------------------------

/** @brief Outputs the range of a TaylorModel to an output stream.
 *
 *  @related TaylorModel
 *  @tparam ValueType The type of the values in the model.
 *
 *  For a non-empty model, this method defers to the output operator for
 *  interval_t to print range(). An empty model is printed as "&#8709;",
 *  matching Interval's own operator<<, rather than throwing (range() throws
 *  for an empty model). See TaylorModel::print_polynomial_form() for a
 *  string representation showing the individual terms instead.
 *
 *  @param[in] os The output stream to write to.
 *  @param[in] t The TaylorModel to output.
 *
 *  @return The output stream.
 *
 *  @throw none No throw guarantee
 */
template<typename ValueType>
std::ostream& operator<<(std::ostream& os, const TaylorModel<ValueType>& t) {
    if(t.empty()) {
        os << "∅";
        return os;
    }
    os << t.range();
    return os;
}

/** @brief Multiplies a scalar by a TaylorModel.
 *
 *  @related TaylorModel
 *  @tparam ValueType The type of the values in the model.
 *
 *  This is a convenience method for calling `t * value`.
 *
 *  @param[in] value The scalar to multiply by.
 *  @param[in] t The TaylorModel to multiply.
 *
 *  @return The product of the scalar and the model.
 *
 *  @throw std::bad_alloc If memory allocation for the new model fails.
 *                        Strong throw guarantee.
 */
template<typename ValueType>
TaylorModel<ValueType> operator*(ValueType value,
                                 const TaylorModel<ValueType>& t) {
    return t * value;
}

// -- Out-of-line definitions ------------------------------------------------

template<typename ValueType>
auto TaylorModel<ValueType>::operator+=(const TaylorModel& other)
  -> TaylorModel& {
    if(empty()) { return *this = other; }
    if(other.empty()) { return *this; }
    auto new_order = std::min(max_order(), other.max_order());
    auto lhs       = sweep_to_order(new_order);
    auto rhs       = other.sweep_to_order(new_order);
    return *this   = TaylorModel(lhs.m_polynomial_ + rhs.m_polynomial_,
                                 lhs.m_remainder_ + rhs.m_remainder_);
}

template<typename ValueType>
auto TaylorModel<ValueType>::operator*=(const TaylorModel& other)
  -> TaylorModel& {
    if(empty() || other.empty()) { return *this = TaylorModel(); }
    auto new_order = std::min(max_order(), other.max_order());
    auto lhs       = sweep_to_order(new_order);
    auto rhs       = other.sweep_to_order(new_order);

    const auto& p1 = lhs.m_polynomial_;
    const auto& p2 = rhs.m_polynomial_;

    value_t new_constant = p1.constant() * p2.constant();
    coeffs_t kept, tail;
    auto insert = [&](const monomial_t& mono, value_t coeff) {
        if(mono.degree() <= new_order) {
            kept[mono] += coeff;
        } else {
            tail[mono] += coeff;
        }
    };
    for(auto&& [mono, coeff] : p2.coefficients()) {
        insert(mono, p1.constant() * coeff);
    }
    for(auto&& [mono, coeff] : p1.coefficients()) {
        insert(mono, coeff * p2.constant());
    }
    for(auto&& [mono1, coeff1] : p1.coefficients()) {
        for(auto&& [mono2, coeff2] : p2.coefficients()) {
            insert(mono1 * mono2, coeff1 * coeff2);
        }
    }

    // Every term of tail has degree <= order(p1) + order(p2) == 2*new_order.
    taylor_t tail_poly(value_t{0}, std::move(tail), Order(2 * new_order));
    interval_t new_remainder =
      tail_poly.bound() + p1.bound() * rhs.m_remainder_ +
      p2.bound() * lhs.m_remainder_ + lhs.m_remainder_ * rhs.m_remainder_;
    taylor_t new_poly(new_constant, std::move(kept), Order(new_order));
    return *this = TaylorModel(std::move(new_poly), std::move(new_remainder));
}

template<typename ValueType>
auto TaylorModel<ValueType>::compose_(
  const std::vector<value_t>& outer_coeffs,
  const std::function<interval_t(const interval_t&)>& nth_derivative) const
  -> TaylorModel {
    assert_not_empty_();
    auto order           = max_order();
    taylor_t new_poly    = m_polynomial_.compose_(outer_coeffs);
    interval_t bar_bound = m_polynomial_.bound() - m_polynomial_.constant();
    interval_t enclosure = bar_bound + m_remainder_;
    interval_t xi_range =
      m_polynomial_.constant() + interval_t(value_t{0}, value_t{1}) * enclosure;
    interval_t deriv_val = nth_derivative(xi_range);
    value_t fact         = detail::factorial<value_t>(order + 1);
    interval_t new_remainder =
      deriv_val * pow(enclosure, static_cast<int>(order + 1)) / fact;
    return TaylorModel(std::move(new_poly), std::move(new_remainder));
}

template<typename ValueType>
auto TaylorModel<ValueType>::multiplicative_inverse() const -> TaylorModel {
    assert_not_empty_();
    if(range().contains(value_t{0})) {
        throw std::domain_error("Division by zero");
    }
    auto order = max_order();
    auto outer_coeffs =
      detail::reciprocal_outer_coeffs(m_polynomial_.constant(), order);
    return compose_(outer_coeffs, [order](const interval_t& xi) {
        value_t sign = ((order + 1) % 2 == 0) ? value_t{1} : value_t{-1};
        return sign * detail::factorial<value_t>(order + 1) /
               pow(xi, static_cast<int>(order + 2));
    });
}

template<typename ValueType>
auto TaylorModel<ValueType>::sweep_to_order(size_type new_order) const
  -> TaylorModel {
    assert_not_empty_();
    coeffs_t kept, tail;
    for(auto&& [mono, coeff] : m_polynomial_.coefficients()) {
        if(mono.degree() <= new_order) {
            kept[mono] = coeff;
        } else {
            tail[mono] = coeff;
        }
    }
    taylor_t tail_poly(value_t{0}, std::move(tail),
                       Order(m_polynomial_.max_order()));
    auto new_remainder = m_remainder_ + tail_poly.bound();
    taylor_t new_poly(m_polynomial_.constant(), std::move(kept),
                      Order(new_order));
    return TaylorModel(std::move(new_poly), std::move(new_remainder));
}

template<typename ValueType>
auto TaylorModel<ValueType>::sweep_small(value_t threshold) const
  -> TaylorModel {
    assert_not_empty_();
    auto total_r = range().radius();
    coeffs_t kept, swept;
    for(auto&& [mono, coeff] : m_polynomial_.coefficients()) {
        if(total_r != value_t{0} && std::fabs(coeff) / total_r < threshold) {
            swept[mono] = coeff;
        } else {
            kept[mono] = coeff;
        }
    }
    taylor_t swept_poly(value_t{0}, std::move(swept),
                        Order(m_polynomial_.max_order()));
    auto new_remainder = m_remainder_ + swept_poly.bound();
    taylor_t new_poly(m_polynomial_.constant(), std::move(kept),
                      Order(m_polynomial_.max_order()));
    return TaylorModel(std::move(new_poly), std::move(new_remainder));
}

// -- Elementary functions ---------------------------------------------------

/** @brief Absolute value of a TaylorModel.
 *
 *  @related TaylorModel
 *  @tparam T The value type of the model.
 *
 *  Unlike the elementary functions below, @f$|\cdot|@f$ has no Taylor series
 *  at points where its argument's range straddles 0, so it is not built from
 *  compose_(). If range() lies entirely on one side of 0, @f$|\cdot|@f$ is
 *  linear there and *this (or its negation) is returned unchanged. Otherwise,
 *  mirroring `Affine::abs` and `Taylor::abs`, the exact range of
 *  @f$|\cdot|@f$ is returned directly: for @f$f \in [\mathrm{lo},
 *  \mathrm{hi}]@f$ with @f$\mathrm{lo} < 0 < \mathrm{hi}@f$,
 *  @f$|f| \in [0, \max(|\mathrm{lo}|, \mathrm{hi})]@f$, which the interval
 *  constructor turns directly into a model (per Eq.
 *  \f$\eqref{eq:tm-inclusion}\f$'s `Interval` row), preserving inclusion
 *  rigorously.
 *
 *  @param[in] a The TaylorModel.
 *
 *  @return The absolute value of @p a.
 *
 *  @throw std::bad_alloc If memory allocation for the new model fails.
 *                        Strong throw guarantee.
 */
template<typename T>
TaylorModel<T> abs(const TaylorModel<T>& a) {
    if(a.empty()) { return a; }
    auto a_range = a.range();
    if(a_range.lower() >= T(0.0)) { return a; }
    if(a_range.upper() <= T(0.0)) { return -a; }
    auto m = std::max(std::fabs(a_range.lower()), a_range.upper());
    return TaylorModel<T>(typename TaylorModel<T>::interval_t(T(0.0), m),
                          typename TaylorModel<T>::Order(a.max_order()));
}

/** @brief Absolute value of a TaylorModel.
 *
 *  @related TaylorModel
 *  @tparam T The value type of the model.
 *
 *  This function is an alias for abs(const TaylorModel<T>&). See its
 *  documentation for details.
 *
 *  @param[in] a The TaylorModel.
 *
 *  @return The absolute value of @p a.
 *
 *  @throw std::bad_alloc If memory allocation for the new model fails.
 *                        Strong throw guarantee.
 */
template<typename T>
TaylorModel<T> fabs(const TaylorModel<T>& a) {
    return abs(a);
}

/** @brief Square root of a TaylorModel.
 *
 *  @related TaylorModel
 *  @tparam T The value type of the model.
 *
 *  Composes the binomial series of sqrt about constant() with @p a, per
 *  Eq. \f$\eqref{eq:tm-outer-split}\f$, folding the Lagrange remainder into
 *  the result. See TaylorModel::compose_.
 *
 *  @param[in] a The TaylorModel whose square root is computed.
 *
 *  @return A TaylorModel enclosing the square root of the function modeled
 *          by @p a.
 *
 *  @throw std::domain_error If @p a is empty or range() contains
 *                           non-positive values. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation for the new model fails.
 *                        Strong throw guarantee.
 */
template<typename T>
TaylorModel<T> sqrt(const TaylorModel<T>& a) {
    if(a.empty()) { return a; }
    if(a.range().lower() <= T(0.0)) {
        throw std::domain_error("TaylorModel has non-positive values.");
    }
    auto order        = a.max_order();
    auto outer_coeffs = detail::sqrt_outer_coeffs(a.constant(), order);
    using interval_t  = typename TaylorModel<T>::interval_t;
    return a.compose_(outer_coeffs, [order](const interval_t& xi) {
        T coeff = detail::half_falling_factorial<T>(order + 1);
        return coeff * pow(xi, T(0.5) - static_cast<T>(order + 1));
    });
}

/** @brief Exponential of a TaylorModel.
 *
 *  @related TaylorModel
 *  @tparam T The value type of the model.
 *
 *  Composes the Taylor series of exp about constant() with @p a, per Eq.
 *  \f$\eqref{eq:tm-exp}\f$, folding the Lagrange remainder of Eq.
 *  \f$\eqref{eq:tm-exp-rem}\f$ into the result. See TaylorModel::compose_.
 *
 *  @param[in] a The TaylorModel whose exponential is computed.
 *
 *  @return A TaylorModel enclosing the exponential of the function modeled by
 *          @p a.
 *
 *  @throw std::domain_error If @p a is empty. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation for the new model fails.
 *                        Strong throw guarantee.
 */
template<typename T>
TaylorModel<T> exp(const TaylorModel<T>& a) {
    if(a.empty()) { return a; }
    auto order        = a.max_order();
    auto outer_coeffs = detail::exp_outer_coeffs(a.constant(), order);
    using interval_t  = typename TaylorModel<T>::interval_t;
    return a.compose_(outer_coeffs,
                      [](const interval_t& xi) { return sigma::exp(xi); });
}

/** @brief Natural logarithm of a TaylorModel.
 *
 *  @related TaylorModel
 *  @tparam T The value type of the model.
 *
 *  Composes the series of log about constant() with @p a, folding the
 *  Lagrange remainder into the result. See TaylorModel::compose_.
 *
 *  @param[in] a The TaylorModel whose natural logarithm is computed.
 *
 *  @return A TaylorModel enclosing the natural logarithm of the function
 *          modeled by @p a.
 *
 *  @throw std::domain_error If @p a is empty or range() contains
 *                           non-positive values. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation for the new model fails.
 *                        Strong throw guarantee.
 */
template<typename T>
TaylorModel<T> log(const TaylorModel<T>& a) {
    if(a.empty()) { return a; }
    if(a.range().lower() <= T(0.0)) {
        throw std::domain_error("TaylorModel has non-positive values.");
    }
    auto order        = a.max_order();
    auto outer_coeffs = detail::log_outer_coeffs(a.constant(), order);
    using interval_t  = typename TaylorModel<T>::interval_t;
    return a.compose_(outer_coeffs, [order](const interval_t& xi) {
        T sign = (order % 2 == 0) ? T{1} : T{-1};
        return sign * detail::factorial<T>(order) /
               pow(xi, static_cast<int>(order + 1));
    });
}

/** @brief Power of a TaylorModel.
 *
 *  @related TaylorModel
 *  @tparam T The value type of the model.
 *  @tparam U The type of the exponent.
 *
 *  For a strictly one-signed range this method takes advantage of the fact
 *  that:
 *  @f[
 *  x^y = e^{y \log(x)}
 *  @f]
 *  mirroring Affine::pow. If range() straddles or touches 0 (and is not
 *  exactly the point 0, where @f$0^y = 0@f$ for any @f$y > 0@f$), neither
 *  @f$\log@f$ branch is valid, so @p exp must be a positive integer, and the
 *  result is computed by repeated multiplication instead -- which stays
 *  rigorous (Eq. \f$\eqref{eq:tm-mul}\f$) across a zero-containing range,
 *  unlike the @f$\log@f$/@f$\exp@f$ route.
 *
 *  @param[in] a The TaylorModel whose power is computed.
 *  @param[in] exp The exponent.
 *
 *  @return A TaylorModel enclosing @p a raised to @p exp.
 *
 *  @throw std::domain_error If range() contains 0 and @p exp is negative, or
 *                           if range() contains non-positive values (and is
 *                           not exactly the point 0) and @p exp is not an
 *                           integer. Strong throw guarantee.
 *  @throw std::bad_alloc If memory allocation for the new model fails.
 *                        Strong throw guarantee.
 */
template<typename T, typename U>
TaylorModel<T> pow(const TaylorModel<T>& a, const U& exp) {
    auto order = a.max_order();
    return detail::pow_impl(a, exp, [order](T v) {
        return TaylorModel<T>(v, typename TaylorModel<T>::Order(order));
    });
}

/// Typedef for a Taylor model of floats
using TMFloat = TaylorModel<float>;

/// Typedef for a Taylor model of doubles
using TMDouble = TaylorModel<double>;

} // namespace sigma
