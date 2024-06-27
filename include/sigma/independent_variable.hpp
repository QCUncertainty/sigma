#pragma once
#include <iostream>

namespace sigma {

template<typename ValueType>
class IndependentVariable {
public:
    /// Type of the instance
    using my_t = IndependentVariable<ValueType>;

    /// The numeric type of the variable
    using value_t = ValueType;

    /// @brief Default ctor
    IndependentVariable() noexcept = default;

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

extern template class IndependentVariable<double>;

} // namespace sigma