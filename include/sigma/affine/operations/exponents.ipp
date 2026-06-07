#pragma once
#include <cmath>
#include <sigma/affine/affine.hpp>

namespace sigma {

template<typename T>
Affine<T> sqrt(const Affine<T>& a) {
    auto a_range    = a.range();
    auto sqrt_lower = std::sqrt(a_range.lower());
    auto sqrt_upper = std::sqrt(a_range.upper());
    auto sqrt_sum   = sqrt_upper + sqrt_lower;
    auto sqrt_diff  = sqrt_upper - sqrt_lower;
    auto alpha      = T(1.0) / (sqrt_sum);
    auto zeta =
      sqrt_sum / T(8.0) + T(0.5) * (sqrt_upper * sqrt_lower) / sqrt_sum;
    auto delta = sqrt_diff * sqrt_diff / (T(8.0) * sqrt_sum);
    return a.apply_affine_transform(alpha, zeta, delta);
}

} // namespace sigma
