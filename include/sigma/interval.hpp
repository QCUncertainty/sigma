#pragma once
#include <boost/numeric/interval.hpp>
#include <iostream>
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
     *  Constructs an interval with both bounds set to 0.0.
     *
     *  @throw none No throw guarantee
     */
    Interval() : m_interval_(0.0, 0.0) {}

    /** @brief Construct an interval from a single value
     *
     *  Constructs an interval with both bounds set to the given value.
     *
     *  @param value The value to set both bounds to
     *
     *  @throw none No throw guarantee
     */
    Interval(value_t value) : m_interval_(value, value) {}

    /** @brief Construct an interval from lower and upper bounds
     *
     *  @param lower The lower bound of the interval
     *  @param upper The upper bound of the interval
     *
     *  @throw none No throw guarantee
     */
    Interval(value_t lower, value_t upper) : m_interval_(lower, upper) {}

    /** @brief Returns the lower bound of the interval
     *
     *  @return The lower bound value
     *
     *  @throw none No throw guarantee
     */
    value_t lower() const { return m_interval_.lower(); }

    /** @brief Returns the upper bound of the interval
     *
     *  @return The upper bound value
     *
     *  @throw none No throw guarantee
     */
    value_t upper() const { return m_interval_.upper(); }

    /** @brief Returns the midpoint of the interval
     *
     *  @return The midpoint value
     *
     *  @throw none No throw guarantee
     */
    value_t median() const { return boost::numeric::median(m_interval_); }

    /** @brief Returns the half-width of the interval
     *
     *  This is the distance from the median to either endpoint.
     *
     *  @return The half-width value
     *
     *  @throw none No throw guarantee
     */
    value_t radius() const {
        return boost::numeric::width(m_interval_) / value_t{2};
    }

    // -- Arithmetic in-place operators ----------------------------------------

    /** @brief In-place addition of another interval
     *
     *  @param rhs The interval to add
     *  @return Reference to this interval after addition
     *
     *  @throw none No throw guarantee
     */
    Interval& operator+=(const Interval& rhs) {
        m_interval_ += rhs.m_interval_;
        return *this;
    }

    /** @brief In-place addition of a scalar
     *
     *  @param rhs The scalar to add
     *  @return Reference to this interval after addition
     *
     *  @throw none No throw guarantee
     */
    Interval& operator+=(value_t rhs) {
        m_interval_ += rhs;
        return *this;
    }

    /** @brief In-place subtraction of another interval
     *
     *  @param rhs The interval to subtract
     *  @return Reference to this interval after subtraction
     *
     *  @throw none No throw guarantee
     */
    Interval& operator-=(const Interval& rhs) {
        m_interval_ -= rhs.m_interval_;
        return *this;
    }

    /** @brief In-place subtraction of a scalar
     *
     *  @param rhs The scalar to subtract
     *  @return Reference to this interval after subtraction
     *
     *  @throw none No throw guarantee
     */
    Interval& operator-=(value_t rhs) {
        m_interval_ -= rhs;
        return *this;
    }

    /** @brief In-place multiplication by another interval
     *
     *  @param rhs The interval to multiply by
     *  @return Reference to this interval after multiplication
     *
     *  @throw none No throw guarantee
     */
    Interval& operator*=(const Interval& rhs) {
        m_interval_ *= rhs.m_interval_;
        return *this;
    }

    /** @brief In-place multiplication by a scalar
     *
     *  @param rhs The scalar to multiply by
     *  @return Reference to this interval after multiplication
     *
     *  @throw none No throw guarantee
     */
    Interval& operator*=(value_t rhs) {
        m_interval_ *= rhs;
        return *this;
    }

    /** @brief In-place division by another interval
     *
     *  @param rhs The interval to divide by
     *  @return Reference to this interval after division
     *
     *  @throw none No throw guarantee
     */
    Interval& operator/=(const Interval& rhs) {
        m_interval_ /= rhs.m_interval_;
        return *this;
    }

    /** @brief In-place division by a scalar
     *
     *  @param rhs The scalar to divide by
     *  @return Reference to this interval after division
     *
     *  @throw none No throw guarantee
     */
    Interval& operator/=(value_t rhs) {
        m_interval_ /= rhs;
        return *this;
    }

private:
    /// The underlying boost interval
    boost::numeric::interval<value_t> m_interval_;

}; // class Interval

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

// -- Arithmetic free functions ------------------------------------------------

/** @relates Interval
 *  @brief Negation of an interval
 *
 *  @tparam T The numerical type of the interval
 *  @param a The interval to negate
 *
 *  @return A new interval with negated bounds
 *
 *  @throw none No throw guarantee
 */
template<typename T>
Interval<T> operator-(const Interval<T>& a) {
    return Interval<T>(-a.upper(), -a.lower());
}

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
