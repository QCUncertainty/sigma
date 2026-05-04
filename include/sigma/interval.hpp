#pragma once
#include <algorithm>
#include <boost/numeric/interval.hpp>
#include <iostream>
#include <optional>
#include <type_traits>
/** @file interval.hpp
 *  @brief Defines the Interval class
 */

namespace sigma {

/** @brief Models a numeric interval.
 *
 *  An interval is defined by a lower and upper bound. This class wraps
 *  boost::numeric::interval and provides a median (midpoint) and radius
 *  (half-width) interface consistent with the rest of the sigma library.
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
    Interval(value_t value) : Interval(value, value, false, false) {}

    /** @brief Construct an interval from two bounds
     *
     *  For convenience, the bounds are sorted so that lower() <= upper().
     *
     *
     *  @param lower The lower bound of the interval
     *  @param upper The upper bound of the interval
     *  @param left_open Is @p lower just outside the interval?
     *  @param right_open Is @p upper just outside the interval?
     *  @throw none No throw guarantee
     */
    Interval(value_t lower, value_t upper, bool left_open = false,
             bool right_open = false) :
      m_is_left_open_(left_open),
      m_is_right_open_(right_open),
      m_interval_(interval_t(std::min(lower, upper), std::max(lower, upper))) {
        // A single value in an open interval is actually empty
        if((left_open || right_open) && lower == upper) *this = Interval();
    }

    /** @brief Returns the distance between the interval's bounds.
     *
     *  The width of an interval is defined by |upper() - lower()|. The width
     *  does NOT depend on the openness of the interval. The width of an empty
     *  interval is undefined (and calling this member on an empty interval
     * will cause a domain_error to be thrown)
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

    /** @brief  Is lower() NOT contained in the interval?
     *
     *  @return True if lower() is NOT contained in the interval
     *
     *  @throw none No throw guarantee
     */
    bool left_open() const { return m_is_left_open_; }

    /** @brief  Is lower() contained in the interval?
     *
     * @return True if lower() is contained in the interval.
     *
     * @throw none No throw guarantee.
     */
    bool left_closed() const { return !left_open(); }

    /** @brief Is upper() NOT contained in the interval?
     *
     *  @return True if upper() is NOT contained in the interval
     *
     *  @throw none No throw guarantee
     */
    bool right_open() const { return m_is_right_open_; }

    /** @brief Is upeer contained in the interval?
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
     *  Returns true if lower() <= @p value <= upper(). Equality to lower()/
     *  upper() is only allowed if lower_closed()/upper_closed() is true,
     *  respectively.
     *
     *  @param value The scalar to test
     *
     *  @return True if @p value lies in the interval represented by *this and
     *          false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool contains(value_t value) const {
        if(empty()) { return false; }

        // Check if the value is definitely outside the interval
        if(value < lower() || value > upper()) { return false; }

        // Now we know it's squarely in the interval (or one of the bounds)

        // Not allowed to be a bound if the interval is open on that side
        if(left_open() && value == lower()) { return false; }
        if(right_open() && value == upper()) { return false; }

        return true;
    }

    /** @brief Is @p other fully contained in this interval?
     *
     */
    bool contains(const Interval& other) const {
        if(other.empty()) { return true; }
        if(empty()) { return false; }

        // Check if definitely outside the interval
        if(other.lower() < lower() || other.upper() > upper()) { return false; }

        // Check if the interval is squarely inside the interval
        if(other.lower() > lower() && other.upper() < upper()) { return true; }

        // *this and other must share at least one bound
        // For each bound the choices are:
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

    /** @brief Returns the union of this interval and another interval
     *
     *  The union of two intervals is the smallest interval that contains both
     *  intervals. Because the result must be an interval, union is only defined
     *  when then the intervals overlap at least at one point.
     *
     *  @param other The interval to union with
     *  @return The union of this interval and @p other
     *
     *  @throw std::domain_error if the intervals do not overlap. Strong throw
     *          guarantee.
     */
    Interval set_union(const Interval& other) const {
        if(empty()) { return other; }
        if(other.empty()) { return *this; }
        if(set_intersection(other).empty()) {
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

        return Interval(low, hi, lower_is_open, upper_is_open);
    }

    /** @brief Returns the intersection of this interval and another interval
     *
     *  The intersection of two intervals is the largest interval that is
     *  contained in both intervals.
     *
     *  @param other The interval to intersect with
     *  @return The intersection of this interval and @p other

     *  @throw None No throw guarantee.
     */
    Interval set_intersection(const Interval& other) const {
        if(empty() || other.empty()) { return Interval(); }
        if(other.upper() < lower() || other.lower() > upper()) {
            return Interval();
        }
        value_t low        = lower();
        value_t hi         = upper();
        bool lower_is_open = left_open();
        bool upper_is_open = right_open();

        if(lower() < other.lower()) {
            low           = other.lower();
            lower_is_open = other.left_open();
        } else if(lower() > other.lower()) {
            low           = lower();
            lower_is_open = left_open();
        } else {
            lower_is_open = left_open() || other.left_open();
        }

        if(upper() > other.upper()) {
            hi            = other.upper();
            upper_is_open = other.right_open();
        } else if(upper() < other.upper()) {
            hi            = upper();
            upper_is_open = right_open();
        } else {
            upper_is_open = right_open() || other.right_open();
        }
        return Interval(low, hi, lower_is_open, upper_is_open);
    }

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
    Interval operator-() const {
        if(empty()) { return Interval(); }
        return Interval(-upper(), -lower(), right_open(), left_open());
    }

    /** @brief In-place addition of another interval
     *
     *  @param rhs The interval to add
     *  @return Reference to this interval after addition
     *
     *  @throw none No throw guarantee
     */
    Interval& operator+=(const Interval& rhs) {
        if(empty()) {
            return *this = rhs;
        } else if(rhs.empty()) {
            return *this;
        } else {
            m_interval_      = *m_interval_ + *rhs.m_interval_;
            m_is_left_open_  = left_open() || rhs.left_open();
            m_is_right_open_ = right_open() || rhs.right_open();
        }
        return *this;
    }

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
    Interval& operator*=(const Interval& rhs) {
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

        if(empty() || rhs.empty()) {
            return *this = Interval();
        } else if(width() == 0 && rhs.width() == 0) { // Both points
            value_t prod = lower() * rhs.lower();
            return *this = Interval(prod);
        } else if(width() == 0) { // *this is a point, rhs is an interval
            auto l             = lower();
            auto ll            = l * rhs.lower();
            auto lh            = l * rhs.upper();
            value_t low        = std::min(ll, lh);
            value_t high       = std::max(ll, lh);
            bool lower_is_open = rhs.left_open();
            bool upper_is_open = rhs.right_open();
            if(low != ll) {
                lower_is_open = rhs.right_open();
                upper_is_open = rhs.right_open();
            }
            return *this = Interval(low, high, lower_is_open, upper_is_open);
        } else if(rhs.width() == 0) { // rhs is point, *this is an interval
            // Dispatch to *this is point, rhs is interval
            return *this = (rhs * (*this));
        }
        auto ll            = lower() * rhs.lower();
        auto lh            = lower() * rhs.upper();
        auto hl            = upper() * rhs.lower();
        auto hh            = upper() * rhs.upper();
        auto low           = std::min(std::min(ll, lh), std::min(hl, hh));
        auto high          = std::max(std::max(ll, lh), std::max(hl, hh));
        bool lower_is_open = left_open() || rhs.left_open();
        bool upper_is_open = right_open() || rhs.right_open();
        // if(low == ll) // default is correct for this case
        if(low == lh) {
            lower_is_open = left_open() || rhs.right_open();
        } else if(low == hl) {
            lower_is_open = right_open() || rhs.left_open();
        } else if(low == hh) {
            lower_is_open = right_open() || rhs.right_open();
        }

        // if(high == hh) // default is correct for this case
        if(high == lh) {
            upper_is_open = left_open() || rhs.right_open();
        } else if(high == hl) {
            upper_is_open = right_open() || rhs.left_open();
        } else if(high == ll) {
            upper_is_open = left_open() || rhs.left_open();
        }

        return *this = Interval(low, high, lower_is_open, upper_is_open);
    }

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
     *  @throw none No throw guarantee
     */
    Interval& operator/=(const Interval& rhs) {
        if(empty() || rhs.empty()) {
            *this = Interval();
            return *this;
        } else {
            m_interval_ = *m_interval_ / *rhs.m_interval_;
        }
        return *this;
    }

    /** @brief In-place division by a scalar
     *
     *  @param rhs The scalar to divide by
     *  @return Reference to this interval after division
     *
     *  @throw none No throw guarantee
     */
    Interval& operator/=(value_t rhs) { return *this /= Interval(rhs, rhs); }

    std::string print_interval_form() const {
        if(empty()) { return "[]"; }
        std::string left_open_str  = left_open() ? "(" : "[";
        std::string right_open_str = right_open() ? ")" : "]";

        return left_open_str + std::to_string(lower()) + ", " +
               std::to_string(upper()) + right_open_str;
    }

private:
    void assert_not_empty_() const {
        if(empty()) { throw std::domain_error("Interval is empty"); }
    }
    using interval_t = boost::numeric::interval<value_t>;

    bool m_is_left_open_  = false;
    bool m_is_right_open_ = false;
    std::optional<interval_t> m_interval_;

}; // class Interval

// -- Utility functions
// --------------------------------------------------------

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
        os << " ∅";
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
    if(lhs.left_open() != rhs.left_open()) { return false; }
    if(lhs.right_open() != rhs.right_open()) { return false; }
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
 *  @brief Whether one interval is certainly less than another
 *
 *  Returns true only when @p lhs lies entirely below @p rhs, i.e.
 *  lhs.upper() < rhs.lower().
 *
 *  @tparam T1 The numerical type of the left-hand interval
 *  @tparam T2 The numerical type of the right-hand interval
 *  @param lhs The first interval
 *  @param rhs The second interval
 *
 *  @return Whether @p lhs is certainly less than @p rhs
 */
template<typename T1, typename T2>
bool operator<(const Interval<T1>& lhs, const Interval<T2>& rhs) {
    return lhs.upper() < rhs.lower();
}

/** @relates Interval
 *  @brief Whether one interval is greater than another
 *
 *  @tparam T1 The numerical type of the left-hand interval
 *  @tparam T2 The numerical type of the right-hand interval
 *  @param lhs The first interval
 *  @param rhs The second interval
 *
 *  @return Whether @p lhs is certainly greater than @p rhs
 */
template<typename T1, typename T2>
bool operator>(const Interval<T1>& lhs, const Interval<T2>& rhs) {
    return rhs < lhs;
}

/** @relates Interval
 *  @brief Whether one interval is less than or equal to another
 *
 *  @tparam T1 The numerical type of the left-hand interval
 *  @tparam T2 The numerical type of the right-hand interval
 *  @param lhs The first interval
 *  @param rhs The second interval
 *
 *  @return Whether @p lhs is less than or equal to @p rhs
 */
template<typename T1, typename T2>
bool operator<=(const Interval<T1>& lhs, const Interval<T2>& rhs) {
    return (lhs == rhs) || (lhs < rhs);
}

/** @relates Interval
 *  @brief Whether one interval is greater than or equal to another
 *
 *  @tparam T1 The numerical type of the left-hand interval
 *  @tparam T2 The numerical type of the right-hand interval
 *  @param lhs The first interval
 *  @param rhs The second interval
 *
 *  @return Whether @p lhs is greater than or equal to @p rhs
 */
template<typename T1, typename T2>
bool operator>=(const Interval<T1>& lhs, const Interval<T2>& rhs) {
    return (lhs == rhs) || (lhs > rhs);
}

// -- Arithmetic free functions
// ------------------------------------------------

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
    return lhs /= rhs;
}

/** @overload */
template<typename T>
Interval<T> operator/(T lhs, const Interval<T>& rhs) {
    return Interval<T>(lhs, lhs) /= rhs;
}

/// Typedef for an interval of floats
using IFloat = Interval<float>;

/// Typedef for an interval of doubles
using IDouble = Interval<double>;

} // namespace sigma

namespace std {
template<typename ValueType>
struct hash<sigma::Interval<ValueType>> {
    size_t operator()(const sigma::Interval<ValueType>& i) const {
        std::size_t hash_low  = std::hash<ValueType>()(i.lower());
        std::size_t hash_high = std::hash<ValueType>()(i.upper());
        std::size_t seed      = hash_low;
        seed ^= hash_high + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
} // namespace std
