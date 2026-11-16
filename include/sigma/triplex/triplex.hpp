#pragma once
#include "sigma/interval/general_interval.hpp"
#include "sigma/interval/interval.hpp"
#include "sigma/interval/traits.hpp"
#include "sigma/triplex/detail_/make_triplex.hpp"
#include <cmath>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

/** @file triplex.hpp
 *  @brief Defines the Triplex class
 */

namespace sigma {

/** @brief Models an interval together with a best estimate inside it.
 *
 *  A triplex number \f$ \langle a, m, b \rangle \f$ is an interval
 *  \f$ [a, b] \f$ plus a distinguished member \f$ m \f$ of it, the nominal
 *  value. The interval is a rigorous enclosure, computed exactly as
 *  IntervalType would compute it. The nominal value is computed in ordinary,
 *  round-to-nearest floating point from the operand nominal values, so the
 *  nominal value of any triplex computation is the result of the same
 *  computation performed with plain value_t on the input nominal values.
 *  Nothing ties the nominal value to the median of the interval; in general
 *  they differ.
 *
 *  If the scalar computation overflows, or otherwise produces a non-finite
 *  value, the nominal value is kept anyway. Such a nominal value is not a
 *  member of the interval, and can be detected with `std::isfinite(nominal())`.
 *
 *  See the Triplex Arithmetic page of the documentation for the full design.
 *
 *  @tparam IntervalType The type of the bounds: Interval<T> for closed bounds
 *                       or GeneralInterval<T> for bounds that may be open.
 */
template<typename IntervalType>
class Triplex {
public:
    /// The type of the bounds
    using interval_t = IntervalType;

    /// The numeric type of the bounds and the nominal value
    using value_t = typename interval_t::value_t;

    /** @brief Default constructor
     *
     *  Constructs the empty triplex, which contains no values and has no
     *  nominal value.
     *
     *  @throw none No throw guarantee
     */
    Triplex() = default;

    /** @brief Constructs the degenerate triplex for an exactly known value.
     *
     *  @param value The value of the lower bound, the nominal value, and the
     *               upper bound.
     *
     *  @throw std::domain_error if @p value is not finite. Strong throw
     *         guarantee.
     */
    Triplex(value_t value) : Triplex(interval_t(value), value) {}

    /** @brief Constructs a triplex from its bounds and its nominal value.
     *
     *  The arguments are in the order they are ordered by, and unlike for
     *  Interval nothing is reordered. Both bounds are closed.
     *
     *  @param lower The lower bound
     *  @param nominal The nominal value. Must satisfy
     *                 @p lower <= @p nominal <= @p upper.
     *  @param upper The upper bound
     *
     *  @throw std::domain_error if @p nominal is not a member of the interval
     *         [@p lower, @p upper], which includes the case
     *         @p lower > @p upper. Strong throw guarantee.
     */
    Triplex(value_t lower, value_t nominal, value_t upper) :
      Triplex(checked_bounds_(lower, nominal, upper), nominal) {}

    /** @brief Constructs a triplex whose bounds may be open.
     *
     *  Only available when interval_t is a GeneralInterval.
     *
     *  @tparam I Used to disable this overload for closed intervals. Do not
     *            supply it.
     *
     *  @param lower The lower bound
     *  @param nominal The nominal value. Must be a member of the interval, so
     *                 it may not equal an open bound.
     *  @param upper The upper bound
     *  @param left_open Is @p lower just outside the interval?
     *  @param right_open Is @p upper just outside the interval?
     *
     *  @throw std::domain_error if @p nominal is not a member of the interval,
     *         which includes the case @p lower > @p upper. Strong throw
     *         guarantee.
     */
    template<typename I                                      = interval_t,
             std::enable_if_t<is_general_interval_v<I>, int> = 0>
    Triplex(value_t lower, value_t nominal, value_t upper, bool left_open,
            bool right_open = false) :
      Triplex(checked_bounds_(lower, nominal, upper, left_open, right_open),
              nominal) {}

    /** @brief Constructs a triplex from an interval and a nominal value.
     *
     *  There is deliberately no constructor from an interval alone, since it
     *  would have to make up a nominal value.
     *
     *  @param interval The bounds of the triplex
     *  @param nominal The nominal value. Must be a member of @p interval.
     *
     *  @throw std::domain_error if @p nominal is not a member of @p interval,
     *         which includes every @p nominal when @p interval is empty. Strong
     *         throw guarantee.
     */
    explicit Triplex(interval_t interval, value_t nominal) :
      m_interval_(std::move(interval)), m_nominal_(nominal) {
        if(!m_interval_.contains(m_nominal_)) {
            throw std::domain_error("The nominal value of a triplex must be a "
                                    "member of its interval");
        }
    }

    // -- Accessors ------------------------------------------------------------

    /** @brief Is *this the empty triplex?
     *
     *  @return True if *this contains no values, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool empty() const { return m_interval_.empty(); }

    /** @brief Returns the lower bound
     *
     *  @return The lower bound value
     *
     *  @throw std::domain_error if *this is empty. Strong throw guarantee.
     */
    value_t lower() const {
        assert_not_empty_();
        return m_interval_.lower();
    }

    /** @brief Returns the nominal value
     *
     *  The nominal value is the best estimate of the quantity. It is NOT, in
     *  general, the midpoint of the bounds (see median()), nor a statistical
     *  expectation.
     *
     *  @return The nominal value. May be non-finite if a computation
     *          overflowed.
     *
     *  @throw std::domain_error if *this is empty. Strong throw guarantee.
     */
    value_t nominal() const {
        assert_not_empty_();
        return m_nominal_;
    }

    /** @brief Returns the upper bound
     *
     *  @return The upper bound value
     *
     *  @throw std::domain_error if *this is empty. Strong throw guarantee.
     */
    value_t upper() const {
        assert_not_empty_();
        return m_interval_.upper();
    }

    /** @brief Returns the distance between the bounds
     *
     *  @return upper() - lower(), as computed by interval_t.
     *
     *  @throw std::domain_error if *this is empty. Strong throw guarantee.
     */
    value_t width() const {
        assert_not_empty_();
        return m_interval_.width();
    }

    /** @brief Returns the midpoint of the bounds
     *
     *  The median is a property of the set and is unrelated to nominal().
     *
     *  @return The midpoint of lower() and upper().
     *
     *  @throw std::domain_error if *this is empty. Strong throw guarantee.
     */
    value_t median() const {
        assert_not_empty_();
        return m_interval_.median();
    }

    /** @brief Returns the distance from the lower bound to the nominal value
     *
     *  @return nominal() - lower()
     *
     *  @throw std::domain_error if *this is empty. Strong throw guarantee.
     */
    value_t lower_radius() const { return nominal() - lower(); }

    /** @brief Returns the distance from the nominal value to the upper bound
     *
     *  @return upper() - nominal()
     *
     *  @throw std::domain_error if *this is empty. Strong throw guarantee.
     */
    value_t upper_radius() const { return upper() - nominal(); }

    /** @brief Returns the bounds, discarding the nominal value
     *
     *  @return The interval of *this. Empty if *this is.
     *
     *  @throw none No throw guarantee
     */
    const interval_t& interval() const { return m_interval_; }

    /** @brief Is lower() NOT a member of *this?
     *
     *  Only available when interval_t is a GeneralInterval.
     *
     *  @tparam I Used to disable this overload for closed intervals. Do not
     *            supply it.
     *
     *  @return True if the lower bound is open. False for the empty triplex.
     *
     *  @throw none No throw guarantee
     */
    template<typename I                                      = interval_t,
             std::enable_if_t<is_general_interval_v<I>, int> = 0>
    bool left_open() const {
        return m_interval_.left_open();
    }

    /** @brief Is lower() a member of *this?
     *
     *  Only available when interval_t is a GeneralInterval.
     *
     *  @tparam I Used to disable this overload for closed intervals. Do not
     *            supply it.
     *
     *  @return True if the lower bound is closed.
     *
     *  @throw none No throw guarantee
     */
    template<typename I                                      = interval_t,
             std::enable_if_t<is_general_interval_v<I>, int> = 0>
    bool left_closed() const {
        return m_interval_.left_closed();
    }

    /** @brief Is upper() NOT a member of *this?
     *
     *  Only available when interval_t is a GeneralInterval.
     *
     *  @tparam I Used to disable this overload for closed intervals. Do not
     *            supply it.
     *
     *  @return True if the upper bound is open. False for the empty triplex.
     *
     *  @throw none No throw guarantee
     */
    template<typename I                                      = interval_t,
             std::enable_if_t<is_general_interval_v<I>, int> = 0>
    bool right_open() const {
        return m_interval_.right_open();
    }

    /** @brief Is upper() a member of *this?
     *
     *  Only available when interval_t is a GeneralInterval.
     *
     *  @tparam I Used to disable this overload for closed intervals. Do not
     *            supply it.
     *
     *  @return True if the upper bound is closed.
     *
     *  @throw none No throw guarantee
     */
    template<typename I                                      = interval_t,
             std::enable_if_t<is_general_interval_v<I>, int> = 0>
    bool right_closed() const {
        return m_interval_.right_closed();
    }

    // -- Set queries ----------------------------------------------------------

    /** @brief Whether a scalar lies within the bounds
     *
     *  @param value The scalar to test
     *
     *  @return True if interval() contains @p value, false otherwise.
     *
     *  @throw none No throw guarantee
     */
    bool contains(value_t value) const { return m_interval_.contains(value); }

    /** @brief Whether the bounds of @p other lie within the bounds of *this
     *
     *  The nominal values play no part.
     *
     *  @param other The triplex to test. The empty triplex is contained in
     *               every triplex.
     *
     *  @return True if interval() contains @p other.interval().
     *
     *  @throw none No throw guarantee
     */
    bool contains(const Triplex& other) const {
        return m_interval_.contains(other.m_interval_);
    }

    // -- Arithmetic in-place operators ----------------------------------------
    //
    // A value_t operand converts implicitly to the degenerate triplex, so
    // these also cover arithmetic between a triplex and a scalar.

    /** @brief Negation of a triplex
     *
     *  Negation is exact, so \f$ -\langle a, m, b \rangle =
     *  \langle -b, -m, -a \rangle \f$, with the openness of the bounds swapped.
     *
     *  @return A new triplex with negated bounds and nominal value
     *
     *  @throw none No throw guarantee
     */
    Triplex operator-() const {
        if(empty()) { return Triplex(); }
        return detail_::make_triplex_(-m_interval_, -m_nominal_);
    }

    /** @brief In-place addition of another triplex
     *
     *  As for interval_t, the empty triplex is the identity of addition.
     *
     *  @param rhs The triplex to add
     *  @return Reference to this triplex after addition
     *
     *  @throw none No throw guarantee
     */
    Triplex& operator+=(const Triplex& rhs);

    /** @brief In-place subtraction of another triplex
     *
     *  As for interval_t, subtracting from the empty triplex negates @p rhs.
     *
     *  @param rhs The triplex to subtract
     *  @return Reference to this triplex after subtraction
     *
     *  @throw none No throw guarantee
     */
    Triplex& operator-=(const Triplex& rhs);

    /** @brief In-place multiplication by another triplex
     *
     *  @param rhs The triplex to multiply by
     *  @return Reference to this triplex after multiplication. Empty if either
     *          operand is.
     *
     *  @throw none No throw guarantee
     */
    Triplex& operator*=(const Triplex& rhs);

    /** @brief In-place division by another triplex
     *
     *  As for interval_t, division by an interval containing zero is an
     *  error, even when the nominal value of @p rhs is not zero.
     *
     *  @param rhs The triplex to divide by
     *  @return Reference to this triplex after division. Empty if either
     *          operand is.
     *
     *  @throw std::domain_error if @p rhs contains zero. Strong throw
     *         guarantee.
     */
    Triplex& operator/=(const Triplex& rhs);

    // -- Arithmetic free functions --------------------------------------------
    //
    // These are hidden friends so that a value_t operand converts implicitly:
    // template argument deduction never considers conversions.

    /** @brief Addition of two triplex numbers
     *
     *  Either operand may also be a value_t, which is converted.
     *
     *  @param lhs The left-hand triplex
     *  @param rhs The right-hand triplex
     *
     *  @return The sum of @p lhs and @p rhs
     *
     *  @throw none No throw guarantee
     */
    friend Triplex operator+(Triplex lhs, const Triplex& rhs) {
        return lhs += rhs;
    }

    /** @brief Subtraction of two triplex numbers
     *
     *  Either operand may also be a value_t, which is converted.
     *
     *  @param lhs The left-hand triplex
     *  @param rhs The right-hand triplex
     *
     *  @return The difference of @p lhs and @p rhs
     *
     *  @throw none No throw guarantee
     */
    friend Triplex operator-(Triplex lhs, const Triplex& rhs) {
        return lhs -= rhs;
    }

    /** @brief Multiplication of two triplex numbers
     *
     *  Either operand may also be a value_t, which is converted.
     *
     *  @param lhs The left-hand triplex
     *  @param rhs The right-hand triplex
     *
     *  @return The product of @p lhs and @p rhs
     *
     *  @throw none No throw guarantee
     */
    friend Triplex operator*(Triplex lhs, const Triplex& rhs) {
        return lhs *= rhs;
    }

    /** @brief Division of two triplex numbers
     *
     *  Either operand may also be a value_t, which is converted.
     *
     *  @param lhs The left-hand triplex
     *  @param rhs The right-hand triplex
     *
     *  @return The quotient of @p lhs and @p rhs
     *
     *  @throw std::domain_error if @p rhs contains zero. Strong throw
     *         guarantee.
     */
    friend Triplex operator/(Triplex lhs, const Triplex& rhs) {
        return lhs /= rhs;
    }

private:
    /// Lets make_triplex_ use the unchecked constructor
    friend Triplex detail_::make_triplex_<>(interval_t bounds, value_t nominal);

    /// Tag selecting the constructor that does not check the invariant
    struct unchecked_tag_ {};

    /** @brief Constructs a triplex without checking the class invariant.
     *
     *  @param interval The bounds of the triplex
     *  @param nominal The nominal value
     *
     *  @throw none No throw guarantee
     */
    Triplex(interval_t interval, value_t nominal, unchecked_tag_) :
      m_interval_(std::move(interval)), m_nominal_(nominal) {}

    /** @brief Builds the interval for the constructors taking three values.
     *
     *  interval_t sorts its bounds, which would hide a reversed pair, so the
     *  ordering of the three values is checked here first.
     *
     *  @param lower The lower bound
     *  @param nominal The nominal value
     *  @param upper The upper bound
     *  @param args The openness flags, for a GeneralInterval
     *
     *  @return The interval [@p lower, @p upper].
     *
     *  @throw std::domain_error if @p lower <= @p nominal <= @p upper does not
     *         hold. Strong throw guarantee.
     */
    template<typename... Args>
    static interval_t checked_bounds_(value_t lower, value_t nominal,
                                      value_t upper, Args... args) {
        if(!(lower <= nominal && nominal <= upper)) {
            throw std::domain_error(
              "A triplex requires lower <= nominal <= upper");
        }
        return interval_t(lower, upper, args...);
    }

    /// Code factorization for throwing when a method needs a value
    void assert_not_empty_() const {
        if(empty()) { throw std::domain_error("Triplex is empty"); }
    }

    /// The bounds. Empty if *this is.
    interval_t m_interval_;

    /// The nominal value. Meaningless if *this is empty.
    value_t m_nominal_ = value_t(0);

}; // class Triplex

// -- Out-of-line Definitions --------------------------------------------------

// The bounds are computed first and the nominal value second. interval_t's
// operations restore the rounding mode before they return, so the nominal value
// is computed in round-to-nearest, as plain floating point would compute it.

template<typename IntervalType>
Triplex<IntervalType>& Triplex<IntervalType>::operator+=(const Triplex& rhs) {
    if(empty()) { return *this = rhs; }
    if(rhs.empty()) { return *this; }
    auto bounds     = m_interval_ + rhs.m_interval_;
    value_t nominal = m_nominal_ + rhs.m_nominal_;
    return *this    = detail_::make_triplex_(std::move(bounds), nominal);
}

template<typename IntervalType>
Triplex<IntervalType>& Triplex<IntervalType>::operator-=(const Triplex& rhs) {
    if(empty()) { return *this = -rhs; }
    if(rhs.empty()) { return *this; }
    auto bounds     = m_interval_ - rhs.m_interval_;
    value_t nominal = m_nominal_ - rhs.m_nominal_;
    return *this    = detail_::make_triplex_(std::move(bounds), nominal);
}

template<typename IntervalType>
Triplex<IntervalType>& Triplex<IntervalType>::operator*=(const Triplex& rhs) {
    if(empty() || rhs.empty()) { return *this = Triplex(); }
    auto bounds     = m_interval_ * rhs.m_interval_;
    value_t nominal = m_nominal_ * rhs.m_nominal_;
    return *this    = detail_::make_triplex_(std::move(bounds), nominal);
}

template<typename IntervalType>
Triplex<IntervalType>& Triplex<IntervalType>::operator/=(const Triplex& rhs) {
    if(empty() || rhs.empty()) { return *this = Triplex(); }
    auto bounds     = m_interval_ / rhs.m_interval_;
    value_t nominal = m_nominal_ / rhs.m_nominal_;
    return *this    = detail_::make_triplex_(std::move(bounds), nominal);
}

// -- Utility functions --------------------------------------------------------

/** @relates Triplex
 *  @brief Overload stream insertion to print a triplex
 *
 *  The triplex is printed as "[lower, nominal, upper]", with a parenthesis in
 *  place of the bracket for an open bound. The empty triplex prints as "∅".
 *
 *  @tparam IntervalType The interval type of the triplex
 *  @param os The ostream to write to
 *  @param t The triplex to write
 *
 *  @return The modified ostream instance
 *
 *  @throws std::ios_base::failure if anything goes wrong while writing.
 *          Weak throw guarantee.
 */
template<typename IntervalType>
std::ostream& operator<<(std::ostream& os, const Triplex<IntervalType>& t) {
    if(t.empty()) { return os << "∅"; }
    bool is_left_open  = false;
    bool is_right_open = false;
    if constexpr(is_general_interval_v<IntervalType>) {
        is_left_open  = t.left_open();
        is_right_open = t.right_open();
    }
    return os << (is_left_open ? "(" : "[") << t.lower() << ", " << t.nominal()
              << ", " << t.upper() << (is_right_open ? ")" : "]");
}

/** @relates Triplex
 *  @brief Compare two triplex numbers for equality
 *
 *  Two triplex numbers are equal if their intervals, including the openness
 *  of any bound, and their nominal values are equal.
 *
 *  @tparam I1 The interval type of the left-hand triplex
 *  @tparam I2 The interval type of the right-hand triplex
 *  @param lhs The first triplex
 *  @param rhs The second triplex
 *
 *  @return Whether the instances are equivalent
 *
 *  @throw none No throw guarantee
 */
template<typename I1, typename I2>
bool operator==(const Triplex<I1>& lhs, const Triplex<I2>& rhs) {
    if constexpr(!std::is_same_v<I1, I2>) {
        return false;
    } else {
        if(lhs.empty() || rhs.empty()) { return lhs.empty() && rhs.empty(); }
        return lhs.interval() == rhs.interval() &&
               lhs.nominal() == rhs.nominal();
    }
}

/** @relates Triplex
 *  @brief Compare two triplex numbers for inequality
 *
 *  @tparam I1 The interval type of the left-hand triplex
 *  @tparam I2 The interval type of the right-hand triplex
 *  @param lhs The first triplex
 *  @param rhs The second triplex
 *
 *  @return Whether the instances are not equivalent
 *
 *  @throw none No throw guarantee
 */
template<typename I1, typename I2>
bool operator!=(const Triplex<I1>& lhs, const Triplex<I2>& rhs) {
    return !(lhs == rhs);
}

/** @relates Triplex
 *  @brief Orders two triplex numbers, primarily by their nominal values.
 *
 *  As for Interval's ordering by median, this is a total order meant for
 *  sorting and not an enclosure relation. Triplex numbers are compared
 *  lexicographically by nominal value, then lower bound, then upper bound,
 *  and then, for a general triplex, by the openness of the lower and upper
 *  bounds, with a closed bound before an open one. The tie-breaks make the
 *  order consistent with operator==: neither of two triplex numbers precedes
 *  the other exactly when they are equal, so they can be keys of ordered
 *  containers. The empty triplex sorts before every non-empty one.
 *
 *  As for plain floating point, a NaN nominal value has no place in the order.
 *
 *  @tparam IntervalType The interval type of the triplex numbers
 *  @param lhs The first triplex
 *  @param rhs The second triplex
 *
 *  @return Whether @p lhs precedes @p rhs.
 *
 *  @throw none No throw guarantee
 */
template<typename IntervalType>
bool operator<(const Triplex<IntervalType>& lhs,
               const Triplex<IntervalType>& rhs) {
    if(rhs.empty()) { return false; }
    if(lhs.empty()) { return true; }
    const auto key = [](const Triplex<IntervalType>& t) {
        bool is_left_open  = false;
        bool is_right_open = false;
        if constexpr(is_general_interval_v<IntervalType>) {
            is_left_open  = t.left_open();
            is_right_open = t.right_open();
        }
        return std::make_tuple(t.nominal(), t.lower(), t.upper(), is_left_open,
                               is_right_open);
    };
    return key(lhs) < key(rhs);
}

/// @relates Triplex
/// @brief Same ordering as operator<, with the operands reversed.
template<typename IntervalType>
bool operator>(const Triplex<IntervalType>& lhs,
               const Triplex<IntervalType>& rhs) {
    return rhs < lhs;
}

/// @relates Triplex
/// @brief Same ordering as operator<, negated.
template<typename IntervalType>
bool operator<=(const Triplex<IntervalType>& lhs,
                const Triplex<IntervalType>& rhs) {
    return !(rhs < lhs);
}

/// @relates Triplex
/// @brief Same ordering as operator<, negated and reversed.
template<typename IntervalType>
bool operator>=(const Triplex<IntervalType>& lhs,
                const Triplex<IntervalType>& rhs) {
    return !(lhs < rhs);
}

/// Typedef for a triplex with closed float bounds
using TPFloat = Triplex<Interval<float>>;

/// Typedef for a triplex with closed double bounds
using TPDouble = Triplex<Interval<double>>;

} // namespace sigma

namespace std {

/// Specializes std::hash for Triplex
template<typename IntervalType>
struct hash<sigma::Triplex<IntervalType>> {
    /// Hashes the interval and then folds in the nominal value. The empty
    /// triplex hashes to zero.
    size_t operator()(const sigma::Triplex<IntervalType>& t) const {
        using value_t = typename IntervalType::value_t;
        if(t.empty()) { return 0; }
        std::size_t seed         = std::hash<IntervalType>()(t.interval());
        std::size_t hash_nominal = std::hash<value_t>()(t.nominal());
        seed ^= hash_nominal + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

} // namespace std

#include "sigma/triplex/eigen_compat.hpp"
#include "sigma/triplex/operations/operations.hpp"
