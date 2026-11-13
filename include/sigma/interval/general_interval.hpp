#pragma once
#include "sigma/interval/interval.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

/** @file general_interval.hpp
 *  @brief Defines the GeneralInterval class
 */

namespace sigma {

/** @brief Models an interval whose bounds may be open.
 *
 *  Interval represents closed intervals only. GeneralInterval adds the
 *  openness of each bound on top of one, so the half-open \f$ \lbrack a, b) \f$
 *  and \f$ (a, b\rbrack \f$ and the open \f$ (a, b) \f$ are representable too.
 *  Openness decides which values are members, and nothing else: the width,
 *  median, and radius are those of the closure, the closed Interval with the
 *  same bounds. An infinite bound is never a member, so it is always open.
 *
 *  An Interval converts implicitly, and losslessly, to a GeneralInterval with
 *  both bounds closed (save an infinite one), so arithmetic that mixes the two
 *  types promotes to GeneralInterval. The reverse conversion discards
 *  information and is spelled out with closure().
 *
 *  @tparam ValueType The floating-point type of the bounds
 */
template<typename ValueType>
class GeneralInterval {
public:
    /// The numeric type of the bounds
    using value_t = ValueType;

    /// The type of the closure, the closed interval with the same bounds
    using closure_t = Interval<value_t>;

    /** @brief Default constructor
     *
     *  Constructs an empty interval (contains no values).
     *
     *  @throw none No throw guarantee
     */
    GeneralInterval() = default;

    /** @brief Construct a degenerate interval from a single value
     *
     *  @param value The value to set both bounds to. Both bounds are closed,
     *               unless @p value is infinite, in which case the interval
     *               is empty.
     *
     *  @throw none No throw guarantee
     */
    GeneralInterval(value_t value) : GeneralInterval(value, value) {}

    /** @brief Construct from a closed interval
     *
     *  Every closed interval is a general interval with two closed bounds, so
     *  this conversion is implicit and loses nothing. The exception is an
     *  infinite bound, which is opened, as for every other constructor.
     *
     *  @param closed The closed interval to convert
     *
     *  @throw none No throw guarantee
     */
    GeneralInterval(const closure_t& closed) {
        if(!closed.empty()) {
            *this = GeneralInterval(closed.lower(), closed.upper());
        }
    }

    /** @brief Construct an interval from two bounds and their openness
     *
     *  For convenience, the bounds are sorted so that lower() <= upper(). An
     *  infinite bound is always open, whatever is requested. A single value
     *  with an open bound contains nothing, so it constructs the empty
     *  interval; this includes a single infinite value.
     *
     *  @param lower The lower bound of the interval
     *  @param upper The upper bound of the interval
     *  @param left_open Is @p lower just outside the interval?
     *  @param right_open Is @p upper just outside the interval?
     *
     *  @throw none No throw guarantee
     */
    GeneralInterval(value_t lower, value_t upper, bool left_open = false,
                    bool right_open = false);

    /** @brief Returns the distance between the interval's bounds.
     *
     *  The width does NOT depend on the openness of the bounds.
     *
     *  @return The width of the interval
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t width() const { return m_closure_.width(); }

    /** @brief Returns the midpoint of the interval
     *
     *  @return The midpoint value
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t median() const { return m_closure_.median(); }

    /** @brief Returns the half-width of the interval
     *
     *  @return The half-width value
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t radius() const { return m_closure_.radius(); }

    /** @brief Is *this the empty interval?
     *
     *  @return True if *this is the empty interval, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool empty() const { return m_closure_.empty(); }

    /** @brief Is lower() NOT contained in the interval?
     *
     *  @return True if lower() is NOT contained in the interval. False for the
     *          empty interval.
     *
     *  @throw none No throw guarantee
     */
    bool left_open() const { return m_is_left_open_; }

    /** @brief Is lower() contained in the interval?
     *
     *  @return True if lower() is contained in the interval.
     *
     *  @throw none No throw guarantee.
     */
    bool left_closed() const { return !left_open(); }

    /** @brief Is upper() NOT contained in the interval?
     *
     *  @return True if upper() is NOT contained in the interval. False for the
     *          empty interval.
     *
     *  @throw none No throw guarantee
     */
    bool right_open() const { return m_is_right_open_; }

    /** @brief Is upper() contained in the interval?
     *
     *  @return True if upper() is contained in the interval.
     *
     *  @throw none No throw guarantee
     */
    bool right_closed() const { return !right_open(); }

    /** @brief Returns the lower bound of the interval
     *
     *  @return The lower bound value
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t lower() const { return m_closure_.lower(); }

    /** @brief Returns the upper bound of the interval
     *
     *  @return The upper bound value
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t upper() const { return m_closure_.upper(); }

    /** @brief Returns the closure of the interval
     *
     *  The closure is the closed interval with the same bounds, which is the
     *  smallest Interval containing *this.
     *
     *  @return The closure of *this. The closure of the empty interval is the
     *          empty Interval.
     *
     *  @throw none No throw guarantee
     */
    const closure_t& closure() const { return m_closure_; }

    /** @brief Whether a scalar lies in this interval
     *
     *  Returns true if lower() <= @p value <= upper(). Equality to lower()/
     *  upper() is only allowed if left_closed()/right_closed() is true,
     *  respectively.
     *
     *  @param value The scalar to test
     *
     *  @return True if @p value lies in the interval represented by *this and
     *          false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool contains(value_t value) const;

    /** @brief Is @p other fully contained in this interval?
     *
     *  @param other The interval to test. The empty interval is contained in
     *               every interval.
     *
     *  @return True if every value in @p other is in *this.
     *
     *  @throw none No throw guarantee
     */
    bool contains(const GeneralInterval& other) const;

    /** @brief Returns the union of this interval and another interval
     *
     *  Because the result must be an interval, union is only defined when
     *  there is no gap between the intervals: they must overlap, or touch at a
     *  point that at least one of them contains, as in
     *  \f$ \lbrack 0, 1) \cup [1, 2] = [0, 2] \f$.
     *
     *  @param other The interval to union with
     *  @return The union of this interval and @p other
     *
     *  @throw std::domain_error if there is a gap between the intervals.
     *         Strong throw guarantee.
     */
    GeneralInterval set_union(const GeneralInterval& other) const;

    /** @brief Returns the intersection of this interval and another interval
     *
     *  @param other The interval to intersect with
     *  @return The intersection of this interval and @p other
     *
     *  @throw none No throw guarantee.
     */
    GeneralInterval set_intersection(const GeneralInterval& other) const;

    // -- Arithmetic in-place operators ----------------------------------------

    /** @brief Negation of an interval
     *
     *  Given the interval [a, b], the negation is [-b, -a]. The openness of the
     *  bounds is reversed, e.g., [a, b) becomes (-b, -a].
     *
     *  @return A new interval with negated bounds
     *
     *  @throw none No throw guarantee
     */
    GeneralInterval operator-() const {
        if(empty()) { return GeneralInterval(); }
        return GeneralInterval(-upper(), -lower(), right_open(), left_open());
    }

    /** @brief In-place addition of another interval
     *
     *  A bound of the sum is open if either bound that produced it is.
     *
     *  @param rhs The interval to add
     *  @return Reference to this interval after addition
     *
     *  @throw none No throw guarantee
     */
    GeneralInterval& operator+=(const GeneralInterval& rhs);

    /** @brief In-place subtraction of another interval
     *
     *  @param rhs The interval to subtract
     *  @return Reference to this interval after subtraction
     *
     *  @throw none No throw guarantee
     */
    GeneralInterval& operator-=(const GeneralInterval& rhs) {
        return *this += -rhs;
    }

    /** @brief In-place multiplication by another interval
     *
     *  A bound of the product is closed if any of the products of operand
     *  bounds that equals it combines two closed bounds, or if it is zero and
     *  either operand contains zero.
     *
     *  @param rhs The interval to multiply by
     *  @return Reference to this interval after multiplication
     *
     *  @throw none No throw guarantee
     */
    GeneralInterval& operator*=(const GeneralInterval& rhs);

    /** @brief In-place division by another interval
     *
     *  A divisor merely bounded by an open zero, such as \f$ (0, 1\rbrack \f$,
     *  does not contain zero and is divided by normally; the quotient is then
     *  unbounded on that side. A bound of the quotient is closed under the same
     *  rule as for multiplication, except that a zero bound is closed when the
     *  dividend contains zero.
     *
     *  @param rhs The interval to divide by
     *  @return Reference to this interval after division
     *
     *  @throw std::domain_error if @p rhs contains zero. Strong throw
     *         guarantee.
     */
    GeneralInterval& operator/=(const GeneralInterval& rhs);

    /** @brief Print the interval in interval form
     *
     *  The interval is printed in the form where closed bounds are represented
     *  by square brackets and open bounds are represented by parentheses.
     *
     *  @return A string representation of the interval.
     *
     *  @throw none No throw guarantee
     */
    std::string print_interval_form() const;

    // -- Arithmetic free functions --------------------------------------------
    //
    // These are hidden friends rather than free templates so that an operand
    // that is a closed Interval or a scalar converts implicitly: template
    // argument deduction never considers conversions.

    /** @brief Addition of two intervals
     *
     *  Either operand may also be an Interval or a scalar, which is converted.
     *
     *  @param lhs The left-hand interval
     *  @param rhs The right-hand interval
     *
     *  @return A new interval that is the sum of @p lhs and @p rhs
     *
     *  @throw none No throw guarantee
     */
    friend GeneralInterval operator+(GeneralInterval lhs,
                                     const GeneralInterval& rhs) {
        return lhs += rhs;
    }

    /** @brief Subtraction of two intervals
     *
     *  Either operand may also be an Interval or a scalar, which is converted.
     *
     *  @param lhs The left-hand interval
     *  @param rhs The right-hand interval
     *
     *  @return A new interval that is the difference of @p lhs and @p rhs
     *
     *  @throw none No throw guarantee
     */
    friend GeneralInterval operator-(GeneralInterval lhs,
                                     const GeneralInterval& rhs) {
        return lhs -= rhs;
    }

    /** @brief Multiplication of two intervals
     *
     *  Either operand may also be an Interval or a scalar, which is converted.
     *
     *  @param lhs The left-hand interval
     *  @param rhs The right-hand interval
     *
     *  @return A new interval that is the product of @p lhs and @p rhs
     *
     *  @throw none No throw guarantee
     */
    friend GeneralInterval operator*(GeneralInterval lhs,
                                     const GeneralInterval& rhs) {
        return lhs *= rhs;
    }

    /** @brief Division of two intervals
     *
     *  Either operand may also be an Interval or a scalar, which is converted.
     *
     *  @param lhs The left-hand interval
     *  @param rhs The right-hand interval
     *
     *  @return A new interval that is the quotient of @p lhs and @p rhs
     *
     *  @throw std::domain_error if @p rhs contains zero. Strong throw
     *         guarantee.
     */
    friend GeneralInterval operator/(GeneralInterval lhs,
                                     const GeneralInterval& rhs) {
        return lhs /= rhs;
    }

private:
    /** @brief Whether one bound of a product or a quotient is open.
     *
     *  Multiplying or dividing [a, b] by [c, d] combines the bounds four ways,
     *  the "corners", and each bound of the result is the smallest or largest
     *  of them. A corner is a member of the result when both bounds it came
     *  from are members of their intervals. Several corners can tie for the
     *  same extreme, and the bound is a member if any of them is.
     *
     *  Away from zero, an extreme of \f$ xy \f$ or \f$ x/y \f$ is only reached
     *  at a corner, since moving an interior factor would move the value past
     *  it. Zero is the exception: a zero factor makes the product zero whatever
     *  the other factor is, as in \f$ [0, 0] \cdot (0, 1) = [0, 0] \f$. That
     *  case is handled by from_corners_, not here.
     *
     *  @param[in] value The bound of the result.
     *  @param[in] ll The corner combining both lower bounds.
     *  @param[in] lh The corner combining @p lhs's lower and @p rhs's upper.
     *  @param[in] hl The corner combining @p lhs's upper and @p rhs's lower.
     *  @param[in] hh The corner combining both upper bounds.
     *  @param[in] lhs The left-hand operand.
     *  @param[in] rhs The right-hand operand.
     *
     *  @return Whether @p value is NOT a member of the result.
     *
     *  @throw none No throw guarantee.
     */
    static bool corner_bound_open_(value_t value, value_t ll, value_t lh,
                                   value_t hl, value_t hh,
                                   const GeneralInterval& lhs,
                                   const GeneralInterval& rhs);

    /** @brief Builds a product or a quotient from its bounds and corners.
     *
     *  Code factorization for operator*= and operator/=. The corners are
     *  recomputed in the ambient rounding mode only to identify which of them
     *  each bound came from; a corner that ties with another to within a
     *  rounding is interchangeable with it for that purpose.
     *
     *  @param[in] result The outwardly rounded closure of the result.
     *  @param[in] ll The corner combining both lower bounds.
     *  @param[in] lh The corner combining @p lhs's lower and @p rhs's upper.
     *  @param[in] hl The corner combining @p lhs's upper and @p rhs's lower.
     *  @param[in] hh The corner combining both upper bounds.
     *  @param[in] lhs The left-hand operand.
     *  @param[in] rhs The right-hand operand.
     *  @param[in] zero_attained Is zero a value of the result? If so, a zero
     *                           bound of @p result is closed.
     *
     *  @return The result with the openness of each bound set.
     *
     *  @throw none No throw guarantee.
     */
    static GeneralInterval from_corners_(const closure_t& result, value_t ll,
                                         value_t lh, value_t hl, value_t hh,
                                         const GeneralInterval& lhs,
                                         const GeneralInterval& rhs,
                                         bool zero_attained);

    /** @brief The reciprocal of an interval that does not contain zero.
     *
     *  Used by operator/= for the one case closed division cannot express: a
     *  divisor with an open bound at zero. Such an interval does not contain
     *  zero, so its reciprocal is a single unbounded interval, whereas the
     *  closure does contain zero and cannot be divided by.
     *
     *  @param[in] a The interval to invert. Must not be empty and must not
     *               contain zero, though a bound of it may be an open zero.
     *
     *  @return An interval enclosing the reciprocal of every value in @p a.
     *
     *  @throw std::domain_error if @p a is empty. Strong throw guarantee.
     */
    static GeneralInterval reciprocal_(const GeneralInterval& a);

    /// The closed interval with the same bounds. Empty if *this is.
    closure_t m_closure_;

    /// Whether the lower bound is open
    bool m_is_left_open_ = false;

    /// Whether the upper bound is open
    bool m_is_right_open_ = false;

}; // class GeneralInterval

// -- Out-of-line Definitions --------------------------------------------------

template<typename ValueType>
GeneralInterval<ValueType>::GeneralInterval(value_t lower, value_t upper,
                                            bool left_open, bool right_open) :
  m_closure_(lower, upper),
  // Infinity is not a real number, so it is never a member
  m_is_left_open_(left_open || std::isinf(m_closure_.lower())),
  m_is_right_open_(right_open || std::isinf(m_closure_.upper())) {
    // A single value in an open interval is actually empty
    if((m_is_left_open_ || m_is_right_open_) && lower == upper) {
        *this = GeneralInterval();
    }
}

template<typename ValueType>
bool GeneralInterval<ValueType>::contains(value_t value) const {
    if(!m_closure_.contains(value)) { return false; }

    // Not allowed to be a bound if the interval is open on that side
    if(left_open() && value == lower()) { return false; }
    if(right_open() && value == upper()) { return false; }

    return true;
}

template<typename ValueType>
bool GeneralInterval<ValueType>::contains(const GeneralInterval& other) const {
    if(other.empty()) { return true; }
    if(!m_closure_.contains(other.m_closure_)) { return false; }

    // *this and other may share a bound. For each shared bound the choices
    // are:
    // *this open and other closed -> not okay
    // *this open and other open -> okay
    // *this closed and other closed -> okay
    // *this closed and other open -> okay
    if(other.lower() == lower()) {
        if(left_open() && other.left_closed()) { return false; }
    }
    if(other.upper() == upper()) {
        if(right_open() && other.right_closed()) { return false; }
    }
    return true;
}

template<typename ValueType>
GeneralInterval<ValueType> GeneralInterval<ValueType>::set_union(
  const GeneralInterval& other) const {
    if(empty()) { return other; }
    if(other.empty()) { return *this; }
    // The intervals need not share a point: [0, 1) and [1, 2] only touch, but
    // their union is [0, 2]. What breaks the union is a gap, i.e., bounds that
    // are separated or that coincide at a point neither interval contains.
    const auto separated = [](const GeneralInterval& left,
                              const GeneralInterval& right) {
        if(left.upper() < right.lower()) { return true; }
        return left.upper() == right.lower() && left.right_open() &&
               right.left_open();
    };
    if(separated(*this, other) || separated(other, *this)) {
        throw std::domain_error("Intervals do not overlap");
    }
    value_t low        = lower();
    value_t hi         = upper();
    bool lower_is_open = false;
    bool upper_is_open = false;
    if(lower() < other.lower()) { // *this has the lower bound
        lower_is_open = left_open();
    } else if(lower() == other.lower()) {
        // If both are open, then result is open; otherwise closed
        lower_is_open = left_open() && other.left_open();
    } else {
        low           = other.lower();
        lower_is_open = other.left_open();
    }

    if(upper() > other.upper()) {
        upper_is_open = right_open();
    } else if(upper() == other.upper()) {
        upper_is_open = right_open() && other.right_open();
    } else {
        hi            = other.upper();
        upper_is_open = other.right_open();
    }

    return GeneralInterval(low, hi, lower_is_open, upper_is_open);
}

template<typename ValueType>
GeneralInterval<ValueType> GeneralInterval<ValueType>::set_intersection(
  const GeneralInterval& other) const {
    if(empty() || other.empty()) { return GeneralInterval(); }
    if(other.upper() < lower() || other.lower() > upper()) {
        return GeneralInterval();
    }
    value_t low        = lower();
    value_t hi         = upper();
    bool lower_is_open = left_open();
    bool upper_is_open = right_open();

    if(lower() < other.lower()) {
        low           = other.lower();
        lower_is_open = other.left_open();
    } else if(lower() == other.lower()) {
        lower_is_open = left_open() || other.left_open();
    }

    if(upper() > other.upper()) {
        hi            = other.upper();
        upper_is_open = other.right_open();
    } else if(upper() == other.upper()) {
        upper_is_open = right_open() || other.right_open();
    }
    return GeneralInterval(low, hi, lower_is_open, upper_is_open);
}

template<typename ValueType>
GeneralInterval<ValueType>& GeneralInterval<ValueType>::operator+=(
  const GeneralInterval& rhs) {
    if(empty()) { return *this = rhs; }
    if(rhs.empty()) { return *this; }
    m_closure_ += rhs.m_closure_;
    m_is_left_open_  = left_open() || rhs.left_open();
    m_is_right_open_ = right_open() || rhs.right_open();
    // The sum may have overflowed to an infinite bound
    return *this = GeneralInterval(lower(), upper(), left_open(), right_open());
}

template<typename ValueType>
bool GeneralInterval<ValueType>::corner_bound_open_(
  value_t value, value_t ll, value_t lh, value_t hl, value_t hh,
  const GeneralInterval& lhs, const GeneralInterval& rhs) {
    bool is_open = true;
    if(value == ll) { is_open &= lhs.left_open() || rhs.left_open(); }
    if(value == lh) { is_open &= lhs.left_open() || rhs.right_open(); }
    if(value == hl) { is_open &= lhs.right_open() || rhs.left_open(); }
    if(value == hh) { is_open &= lhs.right_open() || rhs.right_open(); }
    return is_open;
}

template<typename ValueType>
GeneralInterval<ValueType> GeneralInterval<ValueType>::from_corners_(
  const closure_t& result, value_t ll, value_t lh, value_t hl, value_t hh,
  const GeneralInterval& lhs, const GeneralInterval& rhs, bool zero_attained) {
    // fmin and fmax skip the NaN of a zero times an infinite bound, which is
    // not a corner the result can come from.
    auto low  = std::fmin(std::fmin(ll, lh), std::fmin(hl, hh));
    auto high = std::fmax(std::fmax(ll, lh), std::fmax(hl, hh));

    // An attained zero is checked against the result rather than the corners,
    // which are all NaN for, e.g., [0, 0] * (-inf, inf).
    bool lower_is_open = !(zero_attained && result.lower() == value_t(0)) &&
                         corner_bound_open_(low, ll, lh, hl, hh, lhs, rhs);
    bool upper_is_open = !(zero_attained && result.upper() == value_t(0)) &&
                         corner_bound_open_(high, ll, lh, hl, hh, lhs, rhs);

    return GeneralInterval(result.lower(), result.upper(), lower_is_open,
                           upper_is_open);
}

template<typename ValueType>
GeneralInterval<ValueType>& GeneralInterval<ValueType>::operator*=(
  const GeneralInterval& rhs) {
    if(empty() || rhs.empty()) { return *this = GeneralInterval(); }

    auto product = m_closure_ * rhs.m_closure_;

    auto ll = lower() * rhs.lower();
    auto lh = lower() * rhs.upper();
    auto hl = upper() * rhs.lower();
    auto hh = upper() * rhs.upper();

    const bool zero_attained = contains(value_t(0)) || rhs.contains(value_t(0));
    return *this =
             from_corners_(product, ll, lh, hl, hh, *this, rhs, zero_attained);
}

template<typename ValueType>
GeneralInterval<ValueType> GeneralInterval<ValueType>::reciprocal_(
  const GeneralInterval& a) {
    using interval_t   = detail_::boost_interval_t<value_t>;
    constexpr auto inf = std::numeric_limits<value_t>::infinity();
    const interval_t one(value_t(1));

    // A zero bound is necessarily an open one here, and the reciprocal runs
    // off to infinity in that direction. Everything else is a directed
    // division by a point.
    value_t low =
      a.upper() == value_t(0) ? -inf : (one / interval_t(a.upper())).lower();
    value_t high =
      a.lower() == value_t(0) ? inf : (one / interval_t(a.lower())).upper();

    // The reciprocal is decreasing, so the bounds trade places
    return GeneralInterval(low, high, a.right_open(), a.left_open());
}

template<typename ValueType>
GeneralInterval<ValueType>& GeneralInterval<ValueType>::operator/=(
  const GeneralInterval& rhs) {
    if(empty() || rhs.empty()) { return *this = GeneralInterval(); }
    if(rhs.contains(value_t(0))) {
        throw std::domain_error("Division by zero");
    }

    // The closure of a divisor bounded by an open zero contains zero, so
    // closed division would reject it. Multiplying by the reciprocal keeps the
    // information at the cost of one extra rounding.
    if(rhs.lower() == value_t(0) || rhs.upper() == value_t(0)) {
        return *this *= reciprocal_(rhs);
    }

    auto quotient = m_closure_ / rhs.m_closure_;

    auto ll = lower() / rhs.lower();
    auto lh = lower() / rhs.upper();
    auto hl = upper() / rhs.lower();
    auto hh = upper() / rhs.upper();

    // A quotient is zero exactly when its dividend is
    const bool zero_attained = contains(value_t(0));
    return *this =
             from_corners_(quotient, ll, lh, hl, hh, *this, rhs, zero_attained);
}

template<typename ValueType>
std::string GeneralInterval<ValueType>::print_interval_form() const {
    if(empty()) { return "[∅]"; }
    std::string left_open_str  = left_open() ? "(" : "[";
    std::string right_open_str = right_open() ? ")" : "]";

    return left_open_str + std::to_string(lower()) + ", " +
           std::to_string(upper()) + right_open_str;
}

// -- Utility functions --------------------------------------------------------

/** @relates GeneralInterval
 *  @brief Overload stream insertion to print an interval
 *
 *  @tparam ValueType The numerical type of the bounds
 *  @param os The ostream to write to
 *  @param i The interval to write
 *
 *  @return The modified ostream instance
 *
 *  @throws std::ios_base::failure if anything goes wrong while writing.
 *          Weak throw guarantee.
 */
template<typename ValueType>
std::ostream& operator<<(std::ostream& os,
                         const GeneralInterval<ValueType>& i) {
    return os << i.closure();
}

/** @relates GeneralInterval
 *  @brief Compare two intervals for equality
 *
 *  Two intervals are equal if they have the same bounds and the same openness.
 *
 *  @tparam T1 The numerical type of the left-hand interval
 *  @tparam T2 The numerical type of the right-hand interval
 *  @param lhs The first interval
 *  @param rhs The second interval
 *
 *  @return Whether the instances are equivalent
 */
template<typename T1, typename T2>
bool operator==(const GeneralInterval<T1>& lhs,
                const GeneralInterval<T2>& rhs) {
    if constexpr(!std::is_same_v<T1, T2>) return false;
    if(lhs.left_open() != rhs.left_open()) { return false; }
    if(lhs.right_open() != rhs.right_open()) { return false; }
    return lhs.closure() == rhs.closure();
}

/** @relates GeneralInterval
 *  @brief Compare two intervals for inequality
 *
 *  @tparam T1 The numerical type of the left-hand interval
 *  @tparam T2 The numerical type of the right-hand interval
 *  @param lhs The first interval
 *  @param rhs The second interval
 *
 *  @return Whether the instances are not equivalent
 */
template<typename T1, typename T2>
bool operator!=(const GeneralInterval<T1>& lhs,
                const GeneralInterval<T2>& rhs) {
    return !(lhs == rhs);
}

/** @relates GeneralInterval
 *  @brief Orders two intervals, primarily by their median.
 *
 *  The same deliberately weak, total ordering as for Interval, meant only for
 *  consumers that need *some* consistent ordering to sort by and not a rigorous
 *  enclosure relation: the closures are compared first. Intervals with the
 *  same closure are then ordered by the openness of the lower bound and then
 *  of the upper bound, with a closed bound before an open one, so that, as for
 *  Interval, neither of two intervals precedes the other exactly when they are
 *  equal. The empty interval sorts before every non-empty one.
 *
 *  @tparam T The numerical type of the bounds
 *  @param lhs The first interval
 *  @param rhs The second interval
 *
 *  @return Whether @p lhs precedes @p rhs.
 *
 *  @throw none No throw guarantee
 */
template<typename T>
bool operator<(const GeneralInterval<T>& lhs, const GeneralInterval<T>& rhs) {
    if(lhs.closure() < rhs.closure()) { return true; }
    if(rhs.closure() < lhs.closure()) { return false; }
    return std::make_tuple(lhs.left_open(), lhs.right_open()) <
           std::make_tuple(rhs.left_open(), rhs.right_open());
}

/// @relates GeneralInterval
/// @brief Same ordering as operator<, with the operands reversed.
template<typename T>
bool operator>(const GeneralInterval<T>& lhs, const GeneralInterval<T>& rhs) {
    return rhs < lhs;
}

/// @relates GeneralInterval
/// @brief Same ordering as operator<, negated.
template<typename T>
bool operator<=(const GeneralInterval<T>& lhs, const GeneralInterval<T>& rhs) {
    return !(rhs < lhs);
}

/// @relates GeneralInterval
/// @brief Same ordering as operator<, negated and reversed.
template<typename T>
bool operator>=(const GeneralInterval<T>& lhs, const GeneralInterval<T>& rhs) {
    return !(lhs < rhs);
}

/** @relates GeneralInterval
 *  @brief Widens an interval just enough to contain a point.
 *
 *  As for widen(const Interval<T>&, T), except that @p point may also lie on
 *  an open bound of @p a. Either way, the bound that @p point moves or lands on
 *  is closed.
 *
 *  @tparam T The numerical type of the bounds
 *  @param a The interval to widen
 *  @param point The value the result must contain. An infinite or NaN value is
 *               never a member of an interval, so for one of those the result
 *               cannot contain @p point.
 *
 *  @return @p a if it already contains @p point. Otherwise @p a with the bound
 *          on the side of @p point moved to @p point and closed; the
 *          degenerate interval [@p point, @p point] if @p a is empty.
 *
 *  @throw none No throw guarantee
 */
template<typename T>
GeneralInterval<T> widen(const GeneralInterval<T>& a,
                         typename GeneralInterval<T>::value_t point) {
    if(a.empty()) { return GeneralInterval<T>(point); }
    if(a.contains(point)) { return a; }
    if(point <= a.lower()) {
        return GeneralInterval<T>(point, a.upper(), false, a.right_open());
    }
    return GeneralInterval<T>(a.lower(), point, a.left_open(), false);
}

/// Typedef for a general interval of floats
using GIFloat = GeneralInterval<float>;

/// Typedef for a general interval of doubles
using GIDouble = GeneralInterval<double>;

} // namespace sigma

namespace std {

/// Specializes std::hash for GeneralIntervals
template<typename ValueType>
struct hash<sigma::GeneralInterval<ValueType>> {
    /// Hashes the closure and then folds in the openness of each bound.
    size_t operator()(const sigma::GeneralInterval<ValueType>& i) const {
        std::size_t seed = std::hash<sigma::Interval<ValueType>>()(i.closure());
        std::size_t openness =
          (i.left_open() ? 1 : 0) + (i.right_open() ? 2 : 0);
        seed ^= openness + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

} // namespace std

#include "sigma/interval/general_operations/operations.hpp"
