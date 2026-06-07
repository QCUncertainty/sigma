#pragma once
#include <cmath>
#include <sigma/affine/affine.hpp>

namespace sigma {

template<typename T>
Affine<T> abs(const Affine<T>& a) {
    throw std::logic_error("Not implemented");
}

template<typename T>
Affine<T> fabs(const Affine<T>& a) {
    return abs(a);
}

} // namespace sigma
