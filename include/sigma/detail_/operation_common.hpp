#pragma once

#include "sigma/detail_/setter.hpp"
#include "sigma/uncertain.hpp"

namespace sigma::detail_ {

template<typename T>
void inplace_unary(Uncertain<T>& c, T mean, T dcda) {
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(mean);
    c_setter.update_derivatives(dcda);
}

template<typename T>
Uncertain<T> unary_result(const Uncertain<T>& a, T mean, T dcda) {
    Uncertain<T> c(a);
    detail_::inplace_unary(c, mean, dcda);
    return c;
}

template<typename T>
void binary_inplace(Uncertain<T>& c, const Uncertain<T>& b, T mean, T dcda,
                    T dcdb) {
    detail_::Setter<Uncertain<T>> c_setter(c);
    c_setter.update_mean(mean);
    c_setter.update_derivatives(dcda, false);
    c_setter.update_derivatives(b.deps(), dcdb);
}

template<typename T>
Uncertain<T> binary_result(const Uncertain<T>& a, const Uncertain<T>& b, T mean,
                           T dcda, T dcdb) {
    Uncertain<T> c(a);
    detail_::binary_inplace(c, b, mean, dcda, dcdb);
    return c;
}

} // namespace sigma::detail_