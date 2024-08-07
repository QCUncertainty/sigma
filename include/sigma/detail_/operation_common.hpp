#pragma once

#include "sigma/detail_/setter.hpp"
#include "sigma/uncertain.hpp"

namespace sigma::detail_ {

template<typename T>
Uncertain<T> unary_result(const Uncertain<T>& a, T mean, T dcda) {
    Uncertain<T> c(a);
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(mean);
    c_setter.update_derivatives(dcda);
    return c;
}

} // namespace sigma::detail_