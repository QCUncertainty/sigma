#pragma once
#include <cmath>
#include <sigma/affine/affine.hpp>

namespace sigma {

template<typename T>
Affine<T> sqrt(const Affine<T>& a) {
    if(a.empty()) { return a; }
    auto a_range = a.range();
    auto lo      = a_range.lower();
    auto hi      = a_range.upper();
    if(lo < 0) { throw std::domain_error("Affine form has negative values."); }

    auto sqrt_lower = std::sqrt(lo);
    auto sqrt_upper = std::sqrt(hi);

    auto sqrt_sum  = sqrt_upper + sqrt_lower;
    auto sqrt_diff = sqrt_upper - sqrt_lower;
    auto sqrt_prod = sqrt_upper * sqrt_lower;

    auto alpha = T(1.0) / (sqrt_sum);
    auto zeta  = sqrt_sum / T(8.0) + T(0.5) * sqrt_prod / sqrt_sum;
    auto delta = sqrt_diff * sqrt_diff / (T(8.0) * sqrt_sum);

    return a.apply_affine_transform(alpha, zeta, delta);
}

template<typename T>
Affine<T> exp(const Affine<T>& a) {
    auto a_range = a.range();
    auto hi      = a_range.upper();
    auto lo      = a_range.lower();
    if(hi - lo == 0) { return Affine<T>(std::exp(lo)); }

    T one(1.0);
    T two(2.0);
    auto exp_lower = std::exp(lo);
    auto exp_upper = std::exp(hi);
    auto alpha     = (exp_upper - exp_lower) / (hi - lo);
    auto log_alpha = std::log(alpha);
    auto c_chord   = exp_lower - alpha * lo;
    auto c_tangent = alpha * (one - log_alpha);
    auto zeta      = (c_chord + c_tangent) / two;
    auto delta     = (c_chord - c_tangent) / two;
    return a.apply_affine_transform(alpha, zeta, delta);
}

template<typename T>
Affine<T> log(const Affine<T>& a) {
    auto a_range = a.range();
    auto hi      = a_range.upper();
    auto lo      = a_range.lower();
    if(lo <= 0) {
        throw std::domain_error("Affine form has non-positive values.");
    }
    if(hi - lo == 0) { return Affine<T>(std::log(lo)); }

    auto log_lo = std::log(lo);
    auto alpha  = (std::log(hi) - log_lo) / (hi - lo);
    auto xs     = T(1.0) / alpha;
    auto ys     = alpha * (xs - lo) + log_lo;
    auto log_xs = std::log(xs);
    auto zeta   = (log_xs + ys) / 2 - alpha * xs;
    auto delta  = (log_xs - ys) / 2;

    return a.apply_affine_transform(alpha, zeta, delta);
}

template<typename T, typename U>
Affine<T> pow(const Affine<T>& a, const U& exp) {
    U zero(0);
    if(a.empty()) { return a; }
    if(exp == zero) { return Affine<T>(T(1.0)); }

    // Handle cases where the affine form contains 0
    if(a.contains(zero) && exp < zero) {
        throw std::domain_error(
          "Can not raise an affine form containing 0 to a negative power.");
    } else if(a.contains(zero) && exp > zero) {
        return Affine<T>(zero);
    }

    // Handle cases where affine form is strictly negative
    if(a.range().upper() < zero) {
        using clean_u_t = std::decay_t<U>;
        if constexpr(std::is_floating_point_v<clean_u_t>) {
            clean_u_t exp_int;
            if(std::modf(exp, &exp_int) != zero) {
                throw std::domain_error(
                  "Can not raise an affine form with negative values to a "
                  "non-integer power.");
            }
        }

        auto abs_log           = sigma::log(-a);
        auto pow_abs           = sigma::exp(T(exp) * abs_log);
        const bool exp_is_even = static_cast<long long>(exp) % 2 == 0;
        return exp_is_even ? pow_abs : -pow_abs;
    }

    // Handle cases where affine form is strictly positive
    auto loga = sigma::log(a);
    return sigma::exp(loga * exp);
}

} // namespace sigma
