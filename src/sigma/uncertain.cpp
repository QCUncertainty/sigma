#include "independent_variable.hpp"
#include "sigma/uncertain.hpp"
#include <utility>

namespace sigma {

template<typename ValueType>
Uncertain<ValueType>::Uncertain(value_t mean, value_t std) :
  m_mean_(mean), m_std_(std) {
    m_deps_.emplace(
      std::make_pair(std::make_shared<ind_var_t>(mean, std), 1.0));
}

// Explicit Instantiation
template class Uncertain<float>;
template class Uncertain<double>;

} // namespace sigma