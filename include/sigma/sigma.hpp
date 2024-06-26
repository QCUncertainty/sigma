#pragma once
#include <iostream>

namespace sigma {

template<typename ValueType>
class Uncertain {
public:
    /// Type of the instance
    using my_t = Uncertain<ValueType>;

    /// Type of the mean value
    using mean_t = ValueType;

    /// Type of the standard deviation
    using std_t = ValueType;

    /// @brief Default ctor
    Uncertain() noexcept = default;

    /** @brief Construct an independent variable from mean and standard
     *         deviation
     *
     *  @param mean The average value of the variable
     *  @param std The standard deviation of the variable
     *
     *  @throw none No throw guarantee
     */
    Uncertain(mean_t mean, std_t std) : m_mean_(mean), m_std_(std){};

    /** @brief Get the mean value of the variable
     *
     *  @return The value of the mean
     *
     *  @throw none No throw guarantee
     */
    mean_t mean() const { return m_mean_; }

    /** @brief Get the standard deviation of the variable
     *
     *  @return The value of the standard deviation
     *
     *  @throw none No throw guarantee
     */
    std_t std() const { return m_std_; }

private:
    /// Mean value of the variable
    mean_t m_mean_;

    /// Standard deviation of the variable
    std_t m_std_;
};

/** @relates Uncertain
 *  @brief Overload stream insertion to print Uncertain variables
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
    os << u.mean() << "+/-" << u.std();
    return os;
}

extern template class Uncertain<double>;

/// Typedef of an uncertain double
using UDouble = Uncertain<double>;

} // namespace sigma