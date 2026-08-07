#pragma once
#include <algorithm>
#include <cmath>
#include <optional>
#include <sigma/interval/interval.hpp>
#include <sigma/taylor/monomial.hpp>
#include <sstream>
#include <utility>

/** @file taylor.hpp
 *  @brief Defines the Taylor class
 */

namespace sigma {

/** @brief Implements truncated multivariate polynomial algebra.
 *
 * @tparam ValueType The numeric type of the constant term and coefficients.
 *
 * Follows the design described in docs/taylor.md, which this class
 * implements Eq. \f$\eqref{eq:tm-poly}\f$ of:
 * @f[
 *   P(\delta) = \sum_{\left|\beta\right| \leq n} c_{\beta}\, \delta^{\beta}
 *   \qquad \delta \in [-1, 1]^v
 * @f]
 * where each expansion variable @f$\delta_i@f$ is normalized to
 * @f$[-1, 1]@f$, exactly as Affine's error symbols are.
 *
 * Taylor is a truncated polynomial and nothing else: it makes no claim to
 * enclose any particular function, and arithmetic that would produce terms
 * above the truncation order simply discards them (there is no remainder to
 * put them in). TaylorModel, which will pair a Taylor with a remainder
 * interval_t to restore that guarantee, is not yet implemented; nor are the
 * elementary functions (exp, log, sqrt, ...), the Eigen compatibility layer,
 * or the multiplicative inverse/division they require. Those arrive
 * together in a later pass.
 *
 * N.b. like Affine, an empty Taylor (default-constructed, or explicitly
 * constructed from an empty interval) represents the empty set, not the
 * constant 0. This is a different notion of "empty" than Monomial's, which
 * denotes its multiplicative identity.
 */
template<typename ValueType>
class Taylor {
public:
    /// Type used for indexing, offsets, and truncation order
    using size_type = std::size_t;

    /// Type used for storing floating point values
    using value_t = ValueType;

    /// Opaque type used to identify an expansion variable
    using deviation_t = Monomial::variable_t;

    /// Sparse exponent multi-index type
    using monomial_t = Monomial;

    /// Type used to map monomials to their coefficients
    using coeffs_t = std::map<monomial_t, value_t>;

    /// Type of an interval
    using interval_t = Interval<value_t>;

    /** @brief Tag type used to pass the truncation order to constructors.
     *
     *  Wrapping the order in this type avoids overload ambiguity between
     *  `Taylor(value_t center, Order order)` and
     *  `Taylor(value_t lo, value_t hi)`.
     *
     *  Example:
     *  @code
     *  Taylor<double> x(1.0, Taylor<double>::Order{4});
     *  @endcode
     */
    struct Order {
        /// The truncation order
        size_type value;
        /// Constructs an Order with the given value.
        explicit Order(size_type v) : value(v) {}
    };

    /// The truncation order used when none is specified.
    static constexpr Order default_max_order() { return Order(2); }

    // --- Constructors and Assignment ----------------------------------------

    /** @brief Constructs an empty Taylor polynomial.
     *
     *  An empty Taylor represents the empty set, exactly as an empty Affine
     *  does. Its order is set to default_max_order(), though this is moot until
     *  the object becomes non-empty.
     *
     *  @throw none No throw guarantee
     */
    Taylor() : m_order_(default_max_order().value) {}

    /** @brief Constructs a Taylor polynomial from a constant value.
     *
     *  The resulting polynomial has no non-constant coefficients and
     *  represents the single value given by @p center.
     *
     *  @param[in] center The constant term of the polynomial.
     *  @param[in] order  The truncation order, defaults to default_max_order().
     *
     *  @throw none No throw guarantee
     */
    Taylor(value_t center, Order order = default_max_order()) :
      Taylor(interval_t(center, center), order) {}

    /** @brief Constructs a Taylor polynomial from a lower and upper bound.
     *
     *  The resulting polynomial has a single non-constant coefficient (on a
     *  freshly minted expansion variable) and, evaluated over its domain,
     *  spans the interval defined by the lower and upper bounds. If
     *  @p order is 0, that coefficient cannot be represented and is
     *  discarded, so the polynomial collapses to its median.
     *
     *  @param[in] lo    The lower bound.
     *  @param[in] hi    The upper bound.
     *  @param[in] order The truncation order, defaults to default_max_order().
     *
     *  @throw std::bad_alloc If memory allocation for the coefficient fails.
     *                        Strong throw guarantee.
     */
    Taylor(value_t lo, value_t hi, Order order = default_max_order()) :
      Taylor(interval_t(lo, hi), order) {}

    /** @brief Constructs a Taylor polynomial from an interval.
     *
     *  For a non-empty interval, the resulting polynomial has a single
     *  non-constant coefficient on a freshly minted expansion variable
     *  (dropped if @p order is 0). If @p interval is empty, the resulting
     *  polynomial is also empty.
     *
     *  @param[in] interval The interval represented by the polynomial.
     *  @param[in] order    The truncation order, defaults to
     *                      default_max_order().
     *
     *  @throw std::bad_alloc If memory allocation for the coefficient fails.
     *                        Strong throw guarantee.
     */
    explicit Taylor(const interval_t& interval,
                    Order order = default_max_order()) :
      m_order_(order.value) {
        if(interval.empty()) { return; }
        m_constant_ = interval.median();
        if(interval.radius() > 0) {
            monomial_t mono(make_deviation());
            if(mono.degree() <= m_order_) {
                m_coeffs_[mono] = interval.radius();
            }
        }
    }

    /** @brief Constructs a Taylor polynomial from a full set of state.
     *
     *  This constructor allows for the creation of a polynomial by
     *  specifying the full state (the constant term, the non-constant
     *  coefficients, and the truncation order). Any entry of @p coeffs
     *  whose monomial has degree above @p order is dropped, so the
     *  invariant that no stored term exceeds the truncation order holds
     *  regardless of what is passed in. This ctor is meant primarily for
     *  use by other parts of Sigma, but can be used by users as well.
     *
     *  @param[in] constant The constant term of the polynomial.
     *  @param[in] coeffs   The non-constant coefficients of the polynomial.
     *  @param[in] order    The truncation order, defaults to
     *                      default_max_order().
     *
     *  @throw none No throw guarantee
     */
    Taylor(value_t constant, coeffs_t coeffs,
           Order order = default_max_order()) :
      m_constant_(constant), m_order_(order.value) {
        for(auto&& [mono, coeff] : coeffs) {
            if(mono.degree() <= m_order_) { m_coeffs_[mono] = coeff; }
        }
    }

    /// Makes a deep copy of @p other.
    Taylor(const Taylor& other) = default;

    /// Constructs a Taylor polynomial by moving @p other.
    Taylor(Taylor&& other) noexcept = default;

    /// Assigns the value of @p other to this polynomial.
    Taylor& operator=(const Taylor& other) = default;

    /// Moves the value of @p other to this polynomial.
    Taylor& operator=(Taylor&& other) noexcept = default;

    // -- State Accessors -----------------------------------------------------

    /** @brief Returns an interval enclosing the values of *this.
     *
     *  This is a convenience method for calling bound(). See the
     *  documentation for bound() for details.
     *
     *  @return An interval enclosing the values of *this over its domain.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     */
    interval_t range() const { return bound(); }

    /** @brief Returns the truncation order of *this.
     *
     *  @return The truncation order of *this.
     *
     *  @throw none No throw guarantee
     */
    size_type max_order() const noexcept { return m_order_; }

    /** @brief Returns the constant term of *this.
     *
     *  This is @f$c_f@f$: the coefficient of the @f$\beta = 0@f$ monomial.
     *
     *  @return The constant term of *this.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     */
    value_t constant() const {
        assert_not_empty_();
        return *m_constant_;
    }

    /** @brief Returns the non-constant coefficients of *this.
     *
     *  @return A map from monomial to coefficient. The constant term is not
     *          included; see constant().
     *
     *  @throw none No throw guarantee
     */
    const coeffs_t& coefficients() const noexcept { return m_coeffs_; }

    /** @brief Returns the number of terms in the polynomial.
     *
     *  This counts the constant term (if *this is not empty) plus the
     *  number of non-constant coefficients, regardless of whether any of
     *  those coefficients happen to be 0.
     *
     *  @return The number of terms in *this.
     *
     *  @throw none No throw guarantee
     */
    size_type n_terms() const noexcept {
        return m_coeffs_.size() + (empty() ? 0 : 1);
    }

    /** @brief Checks if this Taylor polynomial represents the empty set.
     *
     *  @return True if *this represents the empty set, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool empty() const noexcept { return !m_constant_; }

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
     *  This is a convenience method for calling contains(other.range()).
     *
     *  @param[in] other The Taylor polynomial to check.
     *
     *  @return True if range() contains @p other's range, false otherwise.
     *
     *  @throw std::domain_error If @p other is empty. Strong throw
     *                           guarantee.
     */
    bool contains(const Taylor& other) const { return contains(other.range()); }

    /** @brief Returns an interval enclosing the values of the polynomial.
     *
     *  Computes @f$B(P)@f$ of Eq. \f$\eqref{eq:tm-bound}\f$ using Berz and
     *  Makino's quadratic fast bounder (QFB), per docs/taylor.md's
     *  "Dominated and fast bounders" section. For each expansion variable
     *  @f$\delta_i@f$, its diagonal contribution
     *  @f$c_{e_i}\delta_i + c_{2e_i}\delta_i^2@f$ (the linear and
     *  pure-quadratic terms in @f$\delta_i@f$ alone) is bounded exactly, in
     *  closed form, by completing the square -- no dependency-problem
     *  overestimation, since each @f$\delta_i@f$ is independent of every
     *  other. Everything else (cross terms and degree-@f$\geq 3@f$ terms)
     *  is bounded by the naive per-term baseline instead. The two are summed
     *  with the constant term to produce the final enclosure.
     *
     *  @return An interval enclosing the values of *this over its domain.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     */
    interval_t bound() const;

    /** @brief Returns the partial derivative of *this with respect to
     *         @p var.
     *
     *  Ordinary term-by-term polynomial differentiation: a term
     *  @f$c\,\delta^{\beta}@f$ with @f$\beta_{\texttt{var}} = e > 0@f$
     *  differentiates to @f$ce\,\delta^{\beta - e_{\texttt{var}}}@f$, where
     *  @f$e_{\texttt{var}}@f$ is the multi-index with a 1 in the
     *  @p var slot and 0 elsewhere; terms not involving @p var vanish.
     *  Differentiation cannot raise degree, so it never has terms to drop.
     *  The truncation order of the result is the same as *this.
     *
     *  @param[in] var The expansion variable to differentiate with respect
     *                 to.
     *
     *  @return The partial derivative of *this with respect to @p var.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     */
    Taylor derivative(deviation_t var) const;

    /** @brief Returns *this with every term above @p new_order dropped.
     *
     *  If @p new_order is greater than or equal to max_order(), this simply
     *  relabels the truncation order; no coefficients are dropped, since
     *  none exceed it.
     *
     *  @param[in] new_order The truncation order of the result.
     *
     *  @return *this, truncated to @p new_order.
     *
     *  @throw std::domain_error If *this is empty. Strong throw guarantee.
     */
    Taylor truncate(size_type new_order) const;

    /** @brief Creates a string of the polynomial form of *this.
     *
     *  The polynomial is written as the constant term plus each
     *  coefficient times its monomial (see Monomial::print()). If *this
     *  contains multiple coefficients, the order in which they are printed
     *  follows the ordering of coeffs_t.
     *
     *  @return A string representation of the polynomial form of *this.
     *
     *  @throw std::bad_alloc If memory allocation for the string fails.
     *                        Strong throw guarantee.
     */
    std::string print_polynomial_form() const;

    /** @brief Creates a string of the interval form of *this.
     *
     *  This is a convenience method for calling
     *  range().print_interval_form().
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

    // -- Arithmetic Operators ------------------------------------------------

    /** @brief Returns the additive inverse of *this.
     *
     *  @return The additive inverse of *this.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor operator-() const;

    /** @brief Overwrites *this with the sum of *this and @p value.
     *
     *  If *this is empty, the resulting polynomial has a constant term of
     *  @p value and no coefficients. Otherwise this adds @p value to the
     *  constant term and leaves the coefficients unchanged.
     *
     *  @param[in] value The value to add to *this.
     *
     *  @return Reference to this polynomial after addition.
     *
     *  @throw none No throw guarantee
     */
    Taylor& operator+=(value_t value) {
        if(empty()) { return *this = Taylor(value, Order(m_order_)); }
        (*m_constant_) += value;
        return *this;
    }

    /** @brief Overwrites *this with the sum of *this and @p other.
     *
     *  Implements Eq. \f$\eqref{eq:tm-add}\f$'s polynomial part:
     *  coefficients are summed monomial-wise. If *this and @p other have
     *  different truncation orders, the result's order is the smaller of
     *  the two, and terms of the higher-order operand above that order are
     *  dropped before adding (see docs/taylor.md's "Mixed orders resolve
     *  downward").
     *
     *  @param[in] other The polynomial to add to *this.
     *
     *  @return Reference to this polynomial after addition.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor& operator+=(const Taylor& other);

    /** @brief Returns the sum of *this and @p value.
     *
     *  This is a convenience method for calling `Taylor(*this) += value`.
     *
     *  @param[in] value The value to add to *this.
     *
     *  @return The sum of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor operator+(value_t value) const { return Taylor(*this) += value; }

    /** @brief Returns the sum of *this and @p other.
     *
     *  This is a convenience method for calling `Taylor(*this) += other`.
     *
     *  @param[in] other The polynomial to add to *this.
     *
     *  @return The sum of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor operator+(const Taylor& other) const {
        return Taylor(*this) += other;
    }

    /** @brief Overwrites *this with the difference of *this and @p value.
     *
     *  @param[in] value The value to subtract from *this.
     *
     *  @return Reference to this polynomial after subtraction.
     *
     *  @throw none No throw guarantee
     */
    Taylor& operator-=(value_t value) {
        if(empty()) { return *this = Taylor(-value, Order(m_order_)); }
        (*m_constant_) -= value;
        return *this;
    }

    /** @brief Overwrites *this with the difference of *this and @p other.
     *
     *  This is implemented as `*this += -other`. See the documentation for
     *  operator+=(Taylor) and operator-() for details.
     *
     *  @param[in] other The polynomial to subtract from *this.
     *
     *  @return Reference to this polynomial after subtraction.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor& operator-=(const Taylor& other) { return *this += -other; }

    /** @brief Returns the difference of *this and @p value.
     *
     *  This is a convenience method for calling `Taylor(*this) -= value`.
     *
     *  @param[in] value The value to subtract from *this.
     *
     *  @return The difference of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor operator-(value_t value) const { return Taylor(*this) -= value; }

    /** @brief Returns the difference of *this and @p other.
     *
     *  This is a convenience method for calling `Taylor(*this) -= other`.
     *
     *  @param[in] other The polynomial to subtract from *this.
     *
     *  @return The difference of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor operator-(const Taylor& other) const {
        return Taylor(*this) -= other;
    }

    /** @brief Overwrites *this with the product of *this and @p value.
     *
     *  @param[in] value The value to multiply *this by.
     *
     *  @return Reference to this polynomial after multiplication.
     *
     *  @throw none No throw guarantee
     */
    Taylor& operator*=(value_t value) {
        if(empty()) { return *this; }
        (*m_constant_) *= value;
        for(auto&& [mono, coeff] : m_coeffs_) { coeff *= value; }
        return *this;
    }

    /** @brief Overwrites *this with the product of *this and @p other.
     *
     *  Implements the polynomial part of Eq. \f$\eqref{eq:tm-mul}\f$,
     *  @f$(P_1 P_2)_{\leq n}@f$: the exact product, truncated to the
     *  smaller of *this's and @p other's truncation orders. Unlike
     *  TaylorModel (not yet implemented), Taylor has no remainder to bound
     *  the discarded terms into, so they are simply dropped -- consistent
     *  with Taylor making no claim to enclose anything. As with
     *  operator+=(Taylor), mixed orders resolve to the smaller of the two.
     *
     *  @param[in] other The polynomial to multiply *this by.
     *
     *  @return Reference to this polynomial after multiplication.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor& operator*=(const Taylor& other);

    /** @brief Returns the product of *this and @p value.
     *
     *  This is a convenience method for calling `Taylor(*this) *= value`.
     *
     *  @param[in] value The value to multiply *this by.
     *
     *  @return The product of *this and @p value.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor operator*(value_t value) const { return Taylor(*this) *= value; }

    /** @brief Returns the product of *this and @p other.
     *
     *  This is a convenience method for calling `Taylor(*this) *= other`.
     *
     *  @param[in] other The polynomial to multiply *this by.
     *
     *  @return The product of *this and @p other.
     *
     *  @throw std::bad_alloc If memory allocation for the new polynomial
     *                        fails. Strong throw guarantee.
     */
    Taylor operator*(const Taylor& other) const {
        return Taylor(*this) *= other;
    }

    // -- Comparison Operators ------------------------------------------------

    /** @brief Checks if *this and @p other represent the same polynomial.
     *
     *  Two Taylor polynomials are equal if they are both empty, or if they
     *  have the same truncation order, the same constant term, and the same
     *  coefficients.
     *
     *  @param[in] other The polynomial to compare with *this.
     *
     *  @return True if *this and @p other represent the same polynomial,
     *          false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool operator==(const Taylor& other) const {
        if(empty() != other.empty()) { return false; }
        if(empty()) { return true; }
        if(m_order_ != other.m_order_) { return false; }
        if(m_constant_ != other.m_constant_) { return false; }
        return m_coeffs_ == other.m_coeffs_;
    }

    /** @brief Checks if *this and @p other represent different polynomials.
     *
     *  @param[in] other The polynomial to compare with *this.
     *
     *  @return True if *this and @p other represent different polynomials,
     *          false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool operator!=(const Taylor& other) const { return !(*this == other); }

    /// Returns a process-wide unique integer ID for a new expansion
    /// variable. Delegates to Monomial::make_variable() so Taylor's
    /// variables and Monomial's share a single numbering.
    static deviation_t make_deviation() { return Monomial::make_variable(); }

private:
    /// Asserts that *this is not empty and throws domain_error if it is.
    void assert_not_empty_() const {
        if(empty()) { throw std::domain_error("Taylor polynomial is empty"); }
    }

    /// Returns the exact range of quad * d^2 + lin * d over d in [-1, 1], by
    /// completing the square. Used by bound()'s quadratic fast bounder to
    /// evaluate each variable's diagonal contribution without incurring any
    /// dependency-problem overestimation.
    static interval_t diagonal_extrema_(value_t lin, value_t quad) {
        if(quad == value_t{0}) {
            auto mag = std::fabs(lin);
            return interval_t(-mag, mag);
        }
        value_t f_lo          = quad - lin; // f(-1)
        value_t f_hi          = quad + lin; // f(1)
        value_t vertex        = -lin / (value_t{2} * quad);
        value_t f_vertex      = -(lin * lin) / (value_t{4} * quad);
        bool vertex_in_domain = vertex >= value_t{-1} && vertex <= value_t{1};

        if(quad > value_t{0}) { // convex: vertex is the minimum
            auto lo = vertex_in_domain ? f_vertex : std::min(f_lo, f_hi);
            return interval_t(lo, std::max(f_lo, f_hi));
        }
        // concave: vertex is the maximum
        auto hi = vertex_in_domain ? f_vertex : std::max(f_lo, f_hi);
        return interval_t(std::min(f_lo, f_hi), hi);
    }

    /// This is the constant term of the polynomial. If empty, m_constant_
    /// = nullopt.
    std::optional<value_t> m_constant_;

    /// Map from monomial to coefficient, for non-constant terms only. If
    /// empty, m_coeffs_ = {}.
    coeffs_t m_coeffs_;

    /// The truncation order of *this.
    size_type m_order_;
};

// -- Non-member functions -------------------------------------------------

/** @brief Outputs the range of a Taylor polynomial to an output stream.
 *
 *  @related Taylor
 *  @tparam ValueType The type of the values in the polynomial.
 *
 *  For a non-empty polynomial, this method defers to the output operator
 *  for interval_t to print the value of range(). An empty polynomial is
 *  printed as "&#8709;", matching Interval's own operator<<, rather than
 *  throwing (range() throws for an empty polynomial). See
 *  Taylor::print_polynomial_form() for a string representation showing the
 *  individual terms instead.
 *
 *  @param[in] os The output stream to write to.
 *  @param[in] t The Taylor polynomial to output.
 *
 *  @return The output stream.
 *
 *  @throw none No throw guarantee
 */
template<typename ValueType>
std::ostream& operator<<(std::ostream& os, const Taylor<ValueType>& t) {
    if(t.empty()) {
        os << "∅";
        return os;
    }
    os << t.range();
    return os;
}

/** @brief Multiplies a scalar by a Taylor polynomial.
 *
 *  @related Taylor
 *  @tparam ValueType The type of the values in the polynomial.
 *
 *  This is a convenience method for calling `t * value`.
 *
 *  @param[in] value The scalar to multiply by.
 *  @param[in] t The Taylor polynomial to multiply.
 *
 *  @return The product of the scalar and the polynomial.
 *
 *  @throw std::bad_alloc If memory allocation for the new polynomial fails.
 *                        Strong throw guarantee.
 */
template<typename ValueType>
Taylor<ValueType> operator*(ValueType value, const Taylor<ValueType>& t) {
    return t * value;
}

// -- Out-of-line definitions ------------------------------------------------

template<typename ValueType>
auto Taylor<ValueType>::bound() const -> interval_t {
    assert_not_empty_();
    interval_t domain(value_t{-1}, value_t{1});

    // Each variable's linear and pure-quadratic coefficients (c_{e_i} and
    // c_{2e_i}), collected so their sum can be bounded exactly below.
    std::map<deviation_t, std::pair<value_t, value_t>> diagonal;

    // Naive per-term bound of everything QFB doesn't capture exactly: cross
    // terms (more than one variable) and degree->= 3 single-variable terms.
    interval_t remainder(value_t{0}, value_t{0});

    for(auto&& [mono, coeff] : m_coeffs_) {
        auto&& exps = mono.exponents();
        if(exps.size() == 1) {
            auto [var, exp] = *exps.begin();
            if(exp == 1) {
                diagonal[var].first += coeff;
                continue;
            }
            if(exp == 2) {
                diagonal[var].second += coeff;
                continue;
            }
        }
        interval_t term(value_t{1}, value_t{1});
        for(auto&& [var, exp] : exps) { term *= pow(domain, exp); }
        term *= coeff;
        remainder += term;
    }

    interval_t total(constant(), constant());
    total += remainder;
    for(auto&& [var, coeffs] : diagonal) {
        auto [lin, quad] = coeffs;
        total += diagonal_extrema_(lin, quad);
    }
    return total;
}

template<typename ValueType>
auto Taylor<ValueType>::derivative(deviation_t var) const -> Taylor {
    assert_not_empty_();
    value_t new_constant{0};
    coeffs_t new_coeffs;
    for(auto&& [mono, coeff] : m_coeffs_) {
        auto e = mono.exponent(var);
        if(e == 0) { continue; }
        auto exps         = mono.exponents();
        exps[var]         = e - 1;
        value_t new_coeff = coeff * static_cast<value_t>(e);
        monomial_t new_mono(exps);
        if(new_mono.empty()) {
            new_constant += new_coeff;
        } else {
            new_coeffs[new_mono] += new_coeff;
        }
    }
    return Taylor(new_constant, std::move(new_coeffs), Order(m_order_));
}

template<typename ValueType>
auto Taylor<ValueType>::truncate(size_type new_order) const -> Taylor {
    assert_not_empty_();
    coeffs_t new_coeffs;
    for(auto&& [mono, coeff] : m_coeffs_) {
        if(mono.degree() <= new_order) { new_coeffs[mono] = coeff; }
    }
    return Taylor(*m_constant_, std::move(new_coeffs), Order(new_order));
}

template<typename ValueType>
std::string Taylor<ValueType>::print_polynomial_form() const {
    if(empty()) { return "∅"; }
    std::stringstream ss;
    ss << constant();
    for(auto&& [mono, coeff] : m_coeffs_) {
        ss << " + " << coeff << "*" << mono;
    }
    return ss.str();
}

template<typename ValueType>
auto Taylor<ValueType>::operator-() const -> Taylor {
    if(empty()) { return *this; }
    coeffs_t new_coeffs;
    for(auto&& [mono, coeff] : m_coeffs_) { new_coeffs[mono] = -coeff; }
    return Taylor(-constant(), std::move(new_coeffs), Order(m_order_));
}

template<typename ValueType>
auto Taylor<ValueType>::operator+=(const Taylor& other) -> Taylor& {
    if(empty()) { return *this = other; }
    if(other.empty()) { return *this; }
    auto new_order       = std::min(m_order_, other.m_order_);
    auto lhs             = truncate(new_order);
    auto rhs             = other.truncate(new_order);
    value_t new_constant = *lhs.m_constant_ + *rhs.m_constant_;
    coeffs_t new_coeffs  = lhs.m_coeffs_;
    for(auto&& [mono, coeff] : rhs.m_coeffs_) { new_coeffs[mono] += coeff; }
    return *this =
             Taylor(new_constant, std::move(new_coeffs), Order(new_order));
}

template<typename ValueType>
auto Taylor<ValueType>::operator*=(const Taylor& other) -> Taylor& {
    if(empty() || other.empty()) { return *this = Taylor(); }
    auto new_order       = std::min(m_order_, other.m_order_);
    auto lhs             = truncate(new_order);
    auto rhs             = other.truncate(new_order);
    value_t new_constant = (*lhs.m_constant_) * (*rhs.m_constant_);
    coeffs_t new_coeffs;
    for(auto&& [mono, coeff] : rhs.m_coeffs_) {
        new_coeffs[mono] += (*lhs.m_constant_) * coeff;
    }
    for(auto&& [mono, coeff] : lhs.m_coeffs_) {
        new_coeffs[mono] += coeff * (*rhs.m_constant_);
    }
    for(auto&& [mono1, c1] : lhs.m_coeffs_) {
        for(auto&& [mono2, c2] : rhs.m_coeffs_) {
            auto mono = mono1 * mono2;
            if(mono.degree() > new_order) { continue; }
            new_coeffs[mono] += c1 * c2;
        }
    }
    return *this =
             Taylor(new_constant, std::move(new_coeffs), Order(new_order));
}

/// Typedef for a Taylor polynomial of floats
using TFloat = Taylor<float>;

/// Typedef for a Taylor polynomial of doubles
using TDouble = Taylor<double>;

} // namespace sigma
