#pragma once
#include <cmath>
#include <sigma/affine/affine.hpp>

namespace sigma {

template<typename T>
Affine<T> abs(const Affine<T>& a) {
    if(a.empty()) { return a; }
    auto a_range = a.range();
    if(a_range.lower() >= T(0.0)) {
        return a;
    } else if(a_range.upper() <= T(0.0)) {
        return -a;
    }
    auto new_center      = std::abs(a.center()) / 2.0;
    auto new_error_terms = a.error_terms();
    for(auto&& [error_symbol, error_term_i] : new_error_terms) {
        error_term_i = error_term_i / 2.0;
    }
    return Affine<T>(new_center, new_error_terms);
}

template<typename T>
Affine<T> fabs(const Affine<T>& a) {
    return abs(a);
}

} // namespace sigma
