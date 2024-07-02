#pragma once
#include <iostream>

namespace sigma {

/** @brief Models an independent variable.
 *
 *  An independent variable is one that is not correlated to any other variable.
 *  It has a mean value and a standard deviation.
 *
 *  @tparam ValueType The type of the value and standard deviation
 *
 */
template<typename ValueType>
class IndependentVariable {
public:
    /// Type of this instance
    using my_t = IndependentVariable<ValueType>;

    /// The numeric type of the variable
    using value_t = ValueType;

    /** @brief Construct an independent variable from mean and standard
     *         deviation
     *
     *  @param mean The average value of the variable
     *  @param std The standard deviation of the variable
     *
     *  @throw none No throw guarantee
     */
    IndependentVariable(value_t mean, value_t std) :
      m_mean_(mean), m_std_(std){};

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
    value_t std() const { return m_std_; }

private:
    /// Mean value of the variable
    value_t m_mean_;

    /// Standard deviation of the variable
    value_t m_std_;

}; // class IndependentVariable

/** @relates IndependentVariable
 *  @brief Overload stream insertion to print independent variable
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param os The ostream to write to
 *  @param u The independent variable to write
 *
 *  @return The modified ostream instance
 *
 *  @throws std::ios_base::failure if anything goes wrong while writing.
 *          Weak throw guarantee.
 */
template<typename ValueType>
std::ostream& operator<<(std::ostream& os,
                         const IndependentVariable<ValueType>& u) {
    os << u.mean() << "+/-" << u.std();
    return os;
}

/** @relates IndependentVariable
 *  @brief Compare two independent Variables
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param lhs The first variable
 *  @param rhs The second variable
 *
 *  @return Whether the instances are equivalent
 *
 */
template<typename ValueType>
bool operator==(const IndependentVariable<ValueType>& lhs,
                const IndependentVariable<ValueType>& rhs) {
    return (lhs.mean() == rhs.mean()) && (lhs.std() == rhs.std());
}

/** @relates IndependentVariable
 *  @brief Compare two independent Variables
 *
 *  @tparam ValueType The numerical type of the variable
 *  @param lhs
 *  @param rhs
 *
 *  @return Whether the instances are not equivalent
 *
 */
template<typename ValueType>
bool operator!=(const IndependentVariable<ValueType>& lhs,
                const IndependentVariable<ValueType>& rhs) {
    return !(lhs == rhs);
}

extern template class IndependentVariable<double>;

} // namespace sigma