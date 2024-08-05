#pragma once
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>

namespace sigma {

// Foward Declaration
namespace detail_ {
template<typename T>
class Setter; // Class to alter values inside Uncertain
} // namespace detail_

/** @brief Models an unceratin variable.
 *
 *  In this context, an uncertain variable is one that is dependent on one or
 *  more independent variables. It has a mean value and standard deviation. This
 *  class also keeps track of the variables that each instance is dependent
 *  upon and the contribution of that variable to the uncertainty of this
 *  instance.
 *
 *  @tparam ValueType The type of the value and standard deviation
 *
 */
template<typename ValueType>
class Uncertain {
public:
    /// Type of the instance
    using my_t = Uncertain<ValueType>;

    /// The numeric type of the variable
    using value_t = ValueType;

    /// The type of a standard deviation this depends on
    using dep_sd_t = value_t;

    /// A pointer to a dependency of this variable
    using dep_sd_ptr = std::shared_ptr<dep_sd_t>;

    /// A map of dependencies and their contributions to the uncertainty
    using deps_map_t = std::map<dep_sd_ptr, value_t>;

    /// @brief Default ctor
    Uncertain() noexcept = default;

    /** @brief Construct an certain value from mean
     *
     *  The result of this is not an uncertain value. This is mostly a courtesy
     *  function for simpler operation.
     *
     *  @param mean The value of the variable
     *
     *  @throw none No throw guarantee
     */
    Uncertain(value_t mean) : m_mean_(mean), m_sd_(0.0) {}

    /** @brief Construct an uncertain value from mean and standard deviation
     *
     *  Effectively, this creates a value that is a function of a single
     *  independent variable.
     *
     *  @param mean The average value of the variable
     *  @param sd The standard deviation of the variable
     *
     *  @throw none No throw guarantee
     */
    Uncertain(value_t mean, value_t sd);

    /** @brief Get the mean value of the variable
     *
     *  @return The value of the mean
     *
     *  @throw none No throw guarantee
     */
    value_t mean() const { return m_mean_; }

    /** @brief Get the standard deviation of the variable
     *
     *  @return The value of the standard deviation
     *
     *  @throw none No throw guarantee
     */
    value_t sd() const { return m_sd_; }

    /** @brief Get the dependencies of the variable
     *
     *  @return The dependencies map
     *
     *  @throw none No throw guarantee
     */
    const deps_map_t& deps() const { return m_deps_; }

private:
    /// Mean value of the variable
    value_t m_mean_;

    /// Standard deviation of the variable
    value_t m_sd_;

    /** Map of the standard deviations this value is dependent on to their
     *  partial derivatives with respect to this value
     */
    deps_map_t m_deps_ = {};

    /** A friendly class used by functions to manipulate the private members
     *  of a variable that is being updated
     */
    template<typename T>
    friend class detail_::Setter;

}; // class Uncertain

// -- Out-of-line Definitions --------------------------------------------------

template<typename ValueType>
Uncertain<ValueType>::Uncertain(value_t mean, value_t sd) :
  m_mean_(mean), m_sd_(sd) {
    m_deps_.emplace(
      std::make_pair(std::make_shared<dep_sd_t>(sd), value_t{1.0}));
}

// -- Utility functions --------------------------------------------------------

/** @relates Uncertain
 *  @brief Overload stream insertion to print uncertain variable
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param os The ostream to write to
 *  @param u The uncertain variable to write
 *
 *  @return The modified ostream instance
 *
 *  @throws std::ios_base::failure if anything goes wrong while writing.
 *          Weak throw guarantee.
 */
template<typename ValueType>
std::ostream& operator<<(std::ostream& os, const Uncertain<ValueType>& u) {
    os << u.mean() << "+/-" << u.sd();
    return os;
}

/** @relates Uncertain
 *  @brief Compare two variables for equality
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param lhs The first variable
 *  @param rhs The second variable
 *
 *  @return Whether the instances are equivalent
 *
 */
template<typename ValueType1, typename ValueType2>
bool operator==(const Uncertain<ValueType1>& lhs,
                const Uncertain<ValueType2>& rhs) {
    if constexpr(!std::is_same_v<ValueType1, ValueType2>) {
        return false;
    } else {
        if(lhs.mean() != rhs.mean()) return false;
        if(lhs.sd() != rhs.sd()) return false;
        if(lhs.deps() != rhs.deps()) return false;
        return true;
    }
}

/** @relates Uncertain
 *  @brief Compare two variables for inequality
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param lhs The first variable
 *  @param rhs The second variable
 *
 *  @return Whether the instances are not equivalent
 *
 */
template<typename ValueType1, typename ValueType2>
bool operator!=(const Uncertain<ValueType1>& lhs,
                const Uncertain<ValueType2>& rhs) {
    return !(lhs == rhs);
}

/** @relates Uncertain
 *  @brief Whether one variable is less than another
 *
 *  Compares the mean values of the two variables
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param lhs The first variable
 *  @param rhs The second variable
 *
 *  @return Whether @p lhs is less than @p rhs
 *
 */
template<typename ValueType1, typename ValueType2>
bool operator<(const Uncertain<ValueType1>& lhs,
               const Uncertain<ValueType2>& rhs) {
    return lhs.mean() < rhs.mean();
}

/** @relates Uncertain
 *  @brief Whether one variable is greater than another
 *
 *  Compares the mean values of the two variables
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param lhs The first variable
 *  @param rhs The second variable
 *
 *  @return Whether @p lhs is greater than @p rhs
 *
 */
template<typename ValueType1, typename ValueType2>
bool operator>(const Uncertain<ValueType1>& lhs,
               const Uncertain<ValueType2>& rhs) {
    return rhs < lhs;
}

/** @relates Uncertain
 *  @brief Whether one variable is less than or equal to another
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param lhs The first variable
 *  @param rhs The second variable
 *
 *  @return Whether @p lhs is less than or equal to @p rhs
 *
 */
template<typename ValueType1, typename ValueType2>
bool operator<=(const Uncertain<ValueType1>& lhs,
                const Uncertain<ValueType2>& rhs) {
    return (lhs == rhs) || (lhs < rhs);
}

/** @relates Uncertain
 *  @brief Whether one variable is greater than or equal to another
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param lhs The first variable
 *  @param rhs The second variable
 *
 *  @return Whether @p lhs is greater than or equal to @p rhs
 *
 */
template<typename ValueType1, typename ValueType2>
bool operator>=(const Uncertain<ValueType1>& lhs,
                const Uncertain<ValueType2>& rhs) {
    return (lhs == rhs) || (lhs > rhs);
}

/// Typedef for an uncertain float
using UFloat = Uncertain<float>;

/// Typedef for an uncertain double
using UDouble = Uncertain<double>;

} // namespace sigma
