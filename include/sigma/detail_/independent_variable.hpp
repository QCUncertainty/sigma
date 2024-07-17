#pragma once
#include <iostream>

namespace sigma {

/** @brief Models an independent variable.
 *
 *  An independent variable is one that is not correlated to any other variable.
 *  It has a mean value and a standard deviation, though this class only keeps
 *  track of the standard deviation. 
 *
 *  @tparam ValueType The type of the standard deviation
 *
 */
template<typename ValueType>
class IndependentVariable {
public:
    /// Type of this instance
    using my_t = IndependentVariable<ValueType>;

    /// The numeric type of the variable
    using value_t = ValueType;

    IndependentVariable() = delete;

    /** @brief Construct an independent variable from mean and standard
     *         deviation
     *
     *  @param sd The standard deviation of the variable
     *
     *  @throw none No throw guarantee
     */
    IndependentVariable(value_t sd) : m_sd_(sd) {};

    /** @brief Get the standard deviation of the variable
     *
     *  @return The value of the standard deviation
     *
     *  @throw none No throw guarantee
     */
    value_t sd() const { return m_sd_; }

private:

    /// Standard deviation of the variable
    value_t m_sd_;

}; // class IndependentVariable

} // namespace sigma