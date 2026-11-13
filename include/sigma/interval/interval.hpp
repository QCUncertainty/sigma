#pragma once
#include "sigma/interval/detail_/policies.hpp"
#include <algorithm>
#include <boost/numeric/interval.hpp>
#include <cmath>
#include <iostream>
#include <optional>
#include <tuple>
#include <type_traits>

/** @file interval.hpp
 *  @brief Defines the Interval class
 */

namespace sigma {

/** @brief Models a closed numeric interval.
 *
 *  An interval is defined by a lower and upper bound, both of which are
 *  members of the set. This class wraps boost::numeric::interval and provides
 *  a median (midpoint) and radius (half-width) interface consistent with the
 *  rest of the sigma library. An infinite bound means the interval is
 *  unbounded in that direction. Intervals whose bounds may be open are
 *  represented by GeneralInterval.
 *
 *  @tparam ValueType The floating-point type of the bounds
 */
template<typename ValueType>
class Interval {
public:
    /// The numeric type of the bounds
    using value_t = ValueType;

    /** @brief Default constructor
     *
     *  Constructs an empty interval (contains no values).
     *
     *  @throw none No throw guarantee
     */
    Interval() {}

    /** @brief Construct an interval from a single value
     *
     *  Constructs an interval with both bounds set to the given value.
     *
     *  @param value The value to set both bounds to
     *
     *  @throw none No throw guarantee
     */
    Interval(value_t value) : Interval(value, value) {}

    /** @brief Construct an interval from two bounds
     *
     *  For convenience, the bounds are sorted so that lower() <= upper().
     *
     *  @param lower The lower bound of the interval
     *  @param upper The upper bound of the interval
     *  @throw none No throw guarantee
     */
    Interval(value_t lower, value_t upper) :
      m_interval_(interval_t(std::min(lower, upper), std::max(lower, upper))) {}

    /** @brief Returns the distance between the interval's bounds.
     *
     *  The width of an interval is defined by |upper() - lower()|. The width
     *  of an empty interval is undefined (and calling this member on an empty
     *  interval will cause a domain_error to be thrown)
     *
     *  @return The width of the interval
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t width() const {
        assert_not_empty_();
        return boost::numeric::width(*m_interval_);
    }

    /** @brief Returns the midpoint of the interval
     *
     *  @return The midpoint value
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t median() const {
        assert_not_empty_();
        return boost::numeric::median(*m_interval_);
    }

    /** @brief Returns the half-width of the interval
     *
     *  This is the distance from the median to either endpoint. This is a
     *  convenience method for width() / value_t{2}
     *
     *  @return The half-width value
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t radius() const { return width() / value_t{2}; }

    /** @brief Is *this the empty interval?
     *
     *  The empty interval contains no values.
     *
     *  @return True if *this is the empty interval, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool empty() const { return !m_interval_; }

    /** @brief Returns the lower bound of the interval
     *
     *  @return The lower bound value
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t lower() const {
        assert_not_empty_();
        return m_interval_->lower();
    }

    /** @brief Returns the upper bound of the interval
     *
     *  @return The upper bound value
     *
     *  @throw std::domain_error if the interval is empty
     */
    value_t upper() const {
        assert_not_empty_();
        return m_interval_->upper();
    }

    /** @brief Whether a scalar lies in this interval
     *
     *  An infinite bound marks an unbounded direction, not a member, so no
     *  interval contains an infinite (or NaN) @p value.
     *
     *  @param value The scalar to test
     *
     *  @return True if @p value is finite and lower() <= @p value <= upper(),
     *          and false otherwise or if *this is empty.
     *
     *  @throw none No throw guarantee
     */
    bool contains(value_t value) const {
        return !empty() && std::isfinite(value) && lower() <= value &&
               value <= upper();
    }

    /** @brief Is @p other fully contained in this interval?
     *
     *  @param other The interval to test. The empty interval is contained in
     *               every interval.
     *
     *  @return True if every value in @p other is in *this.
     *
     *  @throw none No throw guarantee
     */
    bool contains(const Interval& other) const {
        if(other.empty()) { return true; }
        // Compare bounds directly, since contains(value) rejects an infinite
        // bound and an unbounded interval must still contain itself.
        return !empty() && lower() <= other.lower() && other.upper() <= upper();
    }

    /** @brief Returns the union of this interval and another interval
     *
     *  The union of two intervals is the smallest interval that contains both
     *  intervals. Because the result must be an interval, union is only defined
     *  when the intervals overlap at least at one point.
     *
     *  @param other The interval to union with
     *  @return The union of this interval and @p other
     *
     *  @throw std::domain_error if the intervals do not overlap. Strong throw
     *          guarantee.
     */
    Interval set_union(const Interval& other) const;

    /** @brief Returns the intersection of this interval and another interval
     *
     *  The intersection of two intervals is the largest interval that is
     *  contained in both intervals.
     *
     *  @param other The interval to intersect with
     *  @return The intersection of this interval and @p other

     *  @throw None No throw guarantee.
     */
    Interval set_intersection(const Interval& other) const;

    // -- Arithmetic in-place operators ----------------------------------------

    /** @brief Negation of an interval
     *
     *  Given the interval [a, b], the negation is [-b, -a].
     *
     *  @return A new interval with negated bounds
     *
     *  @throw none No throw guarantee
     */
    Interval operator-() const {
        if(empty()) { return Interval(); }
        return Interval(-upper(), -lower());
    }

    /** @brief In-place addition of another interval
     *
     *  @param rhs The interval to add
     *  @return Reference to this interval after addition
     *
     *  @throw none No throw guarantee
     */
    Interval& operator+=(const Interval& rhs);

    /** @brief In-place addition of a scalar
     *
     *  @param rhs The scalar to add
     *  @return Reference to this interval after addition
     *
     *  @throw none No throw guarantee
     */
    Interval& operator+=(value_t rhs) { return *this += Interval(rhs, rhs); }

    /** @brief In-place subtraction of another interval
     *
     *  @param rhs The interval to subtract
     *  @return Reference to this interval after subtraction
     *
     *  @throw none No throw guarantee
     */
    Interval& operator-=(const Interval& rhs) { return *this += -rhs; }

    /** @brief In-place subtraction of a scalar
     *
     *  @param rhs The scalar to subtract
     *  @return Reference to this interval after subtraction
     *
     *  @throw none No throw guarantee
     */
    Interval& operator-=(value_t rhs) { return *this -= Interval(rhs, rhs); }

    /** @brief In-place multiplication by another interval
     *
     *  @param rhs The interval to multiply by
     *  @return Reference to this interval after multiplication
     *
     *  @throw none No throw guarantee
     */
    Interval& operator*=(const Interval& rhs);

    /** @brief In-place multiplication by a scalar
     *
     *  @param rhs The scalar to multiply by
     *  @return Reference to this interval after multiplication
     *
     *  @throw none No throw guarantee
     */
    Interval& operator*=(value_t rhs) { return *this *= Interval(rhs); }

    /** @brief In-place division by another interval
     *
     *  @param rhs The interval to divide by
     *  @return Reference to this interval after division
     *
     *  @throw std::domain_error if @p rhs contains zero, including as a bound.
     *         Strong throw guarantee.
     */
    Interval& operator/=(const Interval& rhs);

    /** @brief In-place division by a scalar
     *
     *  @param rhs The scalar to divide by
     *  @return Reference to this interval after division
     *
     *  @throw none No throw guarantee
     */
    Interval& operator/=(value_t rhs) { return *this /= Interval(rhs, rhs); }

    /** @brief Print the interval in interval form
     *
     *  The interval is printed as "[lower, upper]".
     *
     *  @return A string representation of the interval.
     *
     *  @throw none No throw guarantee
     */
    std::string print_interval_form() const;

private:
    /// Code factorization for throwing when a method doesn't make sense with an
    /// empty interval.
    void assert_not_empty_() const {
        if(empty()) { throw std::domain_error("Interval is empty"); }
    }

    /// The underlying interval type. See detail_/policies.hpp for why the
    /// default boost policies are not used.
    using interval_t = detail_::boost_interval_t<value_t>;

    /// The underlying interval. If optional is empty, the interval is empty.
    std::optional<interval_t> m_interval_;

}; // class Interval

// -- Out-of-line Definitions --------------------------------------------------

template<typename ValueType>
Interval<ValueType> Interval<ValueType>::set_union(
  const Interval& other) const {
    if(empty()) { return other; }
    if(other.empty()) { return *this; }
    if(set_intersection(other).empty()) {
        throw std::domain_error("Intervals do not overlap");
    }
    return Interval(std::min(lower(), other.lower()),
                    std::max(upper(), other.upper()));
}

template<typename ValueType>
Interval<ValueType> Interval<ValueType>::set_intersection(
  const Interval& other) const {
    if(empty() || other.empty()) { return Interval(); }
    if(other.upper() < lower() || other.lower() > upper()) {
        return Interval();
    }
    return Interval(std::max(lower(), other.lower()),
                    std::min(upper(), other.upper()));
}

template<typename ValueType>
Interval<ValueType>& Interval<ValueType>::operator+=(const Interval& rhs) {
    if(empty()) {
        return *this = rhs;
    } else if(rhs.empty()) {
        return *this;
    } else {
        m_interval_ = *m_interval_ + *rhs.m_interval_;
    }
    return *this;
}

template<typename ValueType>
Interval<ValueType>& Interval<ValueType>::operator*=(const Interval& rhs) {
    /* Putting this here in case it's helpful for optimizing.
     * Given two intervals X = [a, b] and Y = [c, d], the product X * Y is:
     * [min(ac, ad, bc, bd), max(ac, ad, bc, bd)]. Thus the entire product
     * is determined by four sub-products: ac, ad, bc, and bd. Simple
     * combinatorics suggests that there are 16 possible products for X*Y,
     * (i.e., chose one sub-product for the new lower bound and one for the
     * new upper bound).
     *
     * If we know that a < b and c < d, i.e., our intervals are not points,
     * we can immediately rule out eight products:
     *
     * - [ac, ac], [ad, ad], [bc, bc], and [bd,bd].
     * - [ac, ad], [ac, bc], [bd, bc], and [bd,ad].
     *
     * Proofs that these products are impossible are given below.
     *
     * To prove the first four products are impossible consider [ac, ac],
     * which implies:
     *
     * - ac <= ad <= ac,
     * - ac <= bc <= ac, and
     * -ac <= bd <= ac
     *
     * The first inequality means c == d, the second means a == b. Which
     * means X = [a, a] and Y = [c, c], i.e. X and Y must be points. This
     * contridicts our assumption that X and Y are not points and thus these
     * four products will never appear. Similar logic holds for the other
     * three products of the form [xy, xy].
     *
     * Now consider the product [ac, ad]. This implies:
     *
     * - ac <= bc <= ad, and
     * - ac <= bd <= ad
     *
     * If a is negative, then d <= bc/a <= c, which is not possible with the
     * assumption c < d. If a is zero, the product [ac, ad] is [0, 0], which
     * means bc and bd must be zero. b can't be zero because then
     * X = [0, 0]. This would then mean that both c and d must be zero, but
     * c and d both can't be zero because then Y = [0, 0]. Thus a must be
     * positive. If a is positive, then so is b. If c >= 0 then d is
     * positive and then we have that bd > ad, which is not consistent with
     * bd <= ad. c also can't be negative because then bc < ac. We thus
     * conclude that [ac, ad] is not possible.
     *
     * The remaining three proofs follow similarly. Let "target interval"
     * refer to the interval we are proving is not possible. Then the proof
     * proceeds by showing:
     *
     * - We get two inequalities by forcing the remaining two sub-products
     *   to be bounded by the target interval. Above this showed that bc and
     *   bd must be in the interval [ac, ad]. We term these the "implicit
     *   sub-products" because they do not show up explicitly in the target
     *   interval.
     * - One of X or Y's bounds will appear twice in the target interval. We
     *   term this the "common bound". Above this bound was "a".
     * - If the common bound is the lower bound of X(Y) try assuming the
     *   commonbound is negative and dividing the inequalities by it. If the
     *   common bound is the upper bound, instead divide the inequalities by
     *   the positive of it. This will cause one of the two inequalities to
     *   be a contradiction eliminating that possibility. Above this ruled
     *   out "a" being negative.
     * - The common bound can't be zero otherwise X and Y collapse to
     *   point intervals.
     * - This means the common bound must be positive (negative) for a lower
     *   (upper) bound. In turn, the "other bound" must be positive
     *   (negative) too. Above this "other bound" was "b".
     * - The implicit sub-products will share other bound. If other bound is
     *   the upper (lower) bound, then assume that the lower (upper) bound
     *   of the other interval is positive (negative) or zero. This
     *   will force the upper (lower) bound of the other interval to be
     *   positive (negative) too. All bounds will now have the same sign
     *   and violate one of the inequalities.
     * - The last possibility is then that the lower (upper) bound of the
     *   other interval has the opposite sign as the common bound. This too
     *   will violate one of the inequalities and we conclude that the
     *   target interval is not possible.
     */

    if(empty() || rhs.empty()) { return *this = Interval(); }

    // Boost rounds each of the four sub-products outward before taking the
    // extremes.
    m_interval_ = *m_interval_ * *rhs.m_interval_;
    return *this;
}

template<typename ValueType>
Interval<ValueType>& Interval<ValueType>::operator/=(const Interval& rhs) {
    if(empty() || rhs.empty()) { return *this = Interval(); }
    if(rhs.contains(value_t(0))) {
        throw std::domain_error("Division by zero");
    }
    m_interval_ = *m_interval_ / *rhs.m_interval_;
    return *this;
}

template<typename ValueType>
std::string Interval<ValueType>::print_interval_form() const {
    if(empty()) { return "[∅]"; }
    return "[" + std::to_string(lower()) + ", " + std::to_string(upper()) + "]";
}

// -- Utility functions --------------------------------------------------------

/** @relates Interval
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
std::ostream& operator<<(std::ostream& os, const Interval<ValueType>& i) {
    if(i.empty()) {
        os << "∅";
        return os;
    }
    os << i.median() << "+/-" << i.radius();
    return os;
}

/** @relates Interval
 *  @brief Compare two intervals for equality
 *
 *  @tparam T1 The numerical type of the left-hand interval
 *  @tparam T2 The numerical type of the right-hand interval
 *  @param lhs The first interval
 *  @param rhs The second interval
 *
 *  @return Whether the instances are equivalent
 */
template<typename T1, typename T2>
bool operator==(const Interval<T1>& lhs, const Interval<T2>& rhs) {
    if constexpr(!std::is_same_v<T1, T2>) return false;
    if(lhs.empty() && rhs.empty()) { return true; }
    if(lhs.empty() || rhs.empty()) { return false; }
    return lhs.lower() == rhs.lower() && lhs.upper() == rhs.upper();
}

/** @relates Interval
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
bool operator!=(const Interval<T1>& lhs, const Interval<T2>& rhs) {
    return !(lhs == rhs);
}

/** @relates Interval
 *  @brief Orders two intervals, primarily by their median.
 *
 *  Interval containment does not give a total order (overlapping intervals
 *  are neither less than, greater than, nor equal to one another under it),
 *  so this comparison is a deliberately weaker, total one, meant only for
 *  consumers that need *some* consistent ordering to sort by and not a rigorous
 *  enclosure relation. Intervals are compared lexicographically by median,
 *  then lower bound, then upper bound. The tie-breaks make the order
 *  consistent with operator==: neither of two intervals precedes the other
 *  exactly when they are equal, so, e.g., [0, 2] and [-1, 3] are ordered even
 *  though they share a median, and intervals can be keys of ordered
 *  containers. The empty interval sorts before every non-empty one.
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
bool operator<(const Interval<T>& lhs, const Interval<T>& rhs) {
    if(rhs.empty()) { return false; }
    if(lhs.empty()) { return true; }
    return std::make_tuple(lhs.median(), lhs.lower(), lhs.upper()) <
           std::make_tuple(rhs.median(), rhs.lower(), rhs.upper());
}

/// @relates Interval
/// @brief Same ordering as operator<, with the operands reversed.
template<typename T>
bool operator>(const Interval<T>& lhs, const Interval<T>& rhs) {
    return rhs < lhs;
}

/// @relates Interval
/// @brief Same ordering as operator<, negated.
template<typename T>
bool operator<=(const Interval<T>& lhs, const Interval<T>& rhs) {
    return !(rhs < lhs);
}

/// @relates Interval
/// @brief Same ordering as operator<, negated and reversed.
template<typename T>
bool operator>=(const Interval<T>& lhs, const Interval<T>& rhs) {
    return !(lhs < rhs);
}

/** @relates Interval
 *  @brief Widens an interval just enough to contain a point.
 *
 *  @tparam T The numerical type of the bounds
 *  @param a The interval to widen
 *  @param point The value the result must contain. An infinite or NaN value is
 *               never a member of an interval, so for one of those the result
 *               cannot contain @p point.
 *
 *  @return @p a if it already contains @p point. Otherwise @p a with the bound
 *          on the side of @p point moved to @p point; the degenerate interval
 *          [@p point, @p point] if @p a is empty.
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> widen(const Interval<T>& a, typename Interval<T>::value_t point) {
    if(a.empty()) { return Interval<T>(point); }
    if(point < a.lower()) { return Interval<T>(point, a.upper()); }
    if(point > a.upper()) { return Interval<T>(a.lower(), point); }
    return a;
}

// -- Arithmetic free functions ------------------------------------------------

/** @relates Interval
 *  @brief Addition of two intervals
 *
 *  @tparam T The numerical type of the intervals
 *  @param lhs The left-hand interval
 *  @param rhs The right-hand interval
 *
 *  @return A new interval that is the sum of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> operator+(Interval<T> lhs, const Interval<T>& rhs) {
    return lhs += rhs;
}

/** @overload */
template<typename T>
Interval<T> operator+(Interval<T> lhs, T rhs) {
    return lhs += rhs;
}

/** @overload */
template<typename T>
Interval<T> operator+(T lhs, const Interval<T>& rhs) {
    return rhs + lhs;
}

/** @relates Interval
 *  @brief Subtraction of two intervals
 *
 *  @tparam T The numerical type of the intervals
 *  @param lhs The left-hand interval
 *  @param rhs The right-hand interval
 *
 *  @return A new interval that is the difference of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> operator-(Interval<T> lhs, const Interval<T>& rhs) {
    return lhs -= rhs;
}

/** @overload */
template<typename T>
Interval<T> operator-(Interval<T> lhs, T rhs) {
    return lhs -= rhs;
}

/** @overload */
template<typename T>
Interval<T> operator-(T lhs, const Interval<T>& rhs) {
    return Interval<T>(lhs, lhs) -= rhs;
}

/** @relates Interval
 *  @brief Multiplication of two intervals
 *
 *  @tparam T The numerical type of the intervals
 *  @param lhs The left-hand interval
 *  @param rhs The right-hand interval
 *
 *  @return A new interval that is the product of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> operator*(Interval<T> lhs, const Interval<T>& rhs) {
    return lhs *= rhs;
}

/** @overload */
template<typename T>
Interval<T> operator*(Interval<T> lhs, T rhs) {
    return lhs *= rhs;
}

/** @overload */
template<typename T>
Interval<T> operator*(T lhs, const Interval<T>& rhs) {
    return rhs * lhs;
}

/** @relates Interval
 *  @brief Division of two intervals
 *
 *  If the interval contains zero the resulting interval will loose most of
 * its information.
 *
 *  @tparam T The numerical type of the intervals
 *  @param lhs The left-hand interval
 *  @param rhs The right-hand interval
 *
 *  @return A new interval that is the quotient of @p lhs and @p rhs
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> operator/(Interval<T> lhs, const Interval<T>& rhs) {
    return lhs /= rhs;
}

/** @overload */
template<typename T>
Interval<T> operator/(Interval<T> lhs, T rhs) {
    if(rhs == 0) { throw std::domain_error("Division by zero"); }
    return lhs /= Interval<T>(rhs);
}

/** @overload */
template<typename T>
Interval<T> operator/(T lhs, const Interval<T>& rhs) {
    if(rhs.empty()) { return rhs; }
    if(rhs.contains(0)) { throw std::domain_error("Division by zero"); }
    return Interval<T>(lhs) / rhs;
}

/// Typedef for an interval of floats
using IFloat = Interval<float>;

/// Typedef for an interval of doubles
using IDouble = Interval<double>;

} // namespace sigma

namespace std {

/// Specializes std::hash for Intervals
template<typename ValueType>
struct hash<sigma::Interval<ValueType>> {
    /// Hashes the lower and upper bounds of an interval and then combines them
    /// using a simple XOR operation. The empty interval, which has no bounds,
    /// hashes to zero.
    size_t operator()(const sigma::Interval<ValueType>& i) const {
        if(i.empty()) { return 0; }
        std::size_t hash_low  = std::hash<ValueType>()(i.lower());
        std::size_t hash_high = std::hash<ValueType>()(i.upper());
        std::size_t seed      = hash_low;
        seed ^= hash_high + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

} // namespace std

#include "sigma/interval/eigen_compat.hpp"
#include "sigma/interval/operations/operations.hpp"
