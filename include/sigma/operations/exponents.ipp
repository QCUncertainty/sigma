#pragma once

#include "sigma/affine.hpp"
#include "sigma/detail_/operation_common.hpp"
#include "sigma/interval.hpp"
#include "sigma/partitioned_affine.hpp"
#include <cmath>

namespace sigma {

template<typename T, typename U>
Uncertain<T> pow(const Uncertain<T>& a, const U& exp) {
    T mean = std::pow(a.mean(), exp);
    T dcda = exp * std::pow(a.mean(), exp - 1);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> pow(const Uncertain<T>& a, const Uncertain<T>& exp) {
    T mean = std::pow(a.mean(), exp.mean());
    T dcda = exp.mean() * std::pow(a.mean(), exp.mean() - 1);
    T dcdb = std::log(a.mean()) * std::pow(a.mean(), exp.mean());
    return detail_::binary_result(a, exp, mean, dcda, dcdb);
}

template<typename T>
Uncertain<T> sqrt(const Uncertain<T>& a) {
    T mean = std::sqrt(a.mean());
    T dcda = 1.0 / (2.0 * std::sqrt(a.mean()));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> cbrt(const Uncertain<T>& a) {
    T mean = std::cbrt(a.mean());
    T dcda = 1.0 / (3.0 * std::cbrt(std::pow(a.mean(), 2.0)));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> exp(const Uncertain<T>& a) {
    T mean = std::exp(a.mean());
    T dcda = std::exp(a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> exp2(const Uncertain<T>& a) {
    T mean = std::exp2(a.mean());
    T dcda = mean * std::log(2.0);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> expm1(const Uncertain<T>& a) {
    T mean = std::expm1(a.mean());
    T dcda = std::exp(a.mean());
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> log(const Uncertain<T>& a) {
    T mean = std::log(a.mean());
    T dcda = 1.0 / a.mean();
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> log10(const Uncertain<T>& a) {
    T mean = std::log10(a.mean());
    T dcda = 1.0 / (a.mean() * std::log(10.0));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> log2(const Uncertain<T>& a) {
    T mean = std::log2(a.mean());
    T dcda = 1.0 / (a.mean() * std::log(2.0));
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> log1p(const Uncertain<T>& a) {
    T mean = std::log1p(a.mean());
    T dcda = 1.0 / (a.mean() + 1.0);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T>
Uncertain<T> hypot(const Uncertain<T>& a, const Uncertain<T>& b) {
    T mean = std::hypot(a.mean(), b.mean());
    T dcda = a.mean() / std::hypot(a.mean(), b.mean());
    T dcdb = b.mean() / std::hypot(a.mean(), b.mean());
    return detail_::binary_result(a, b, mean, dcda, dcdb);
}

template<typename T, typename U>
Uncertain<T> hypot(const Uncertain<T>& a, const U& b) {
    T mean = std::hypot(a.mean(), b);
    T dcda = a.mean() / std::hypot(a.mean(), b);
    return detail_::unary_result(a, mean, dcda);
}

template<typename T, typename U>
Uncertain<T> hypot(const U& a, const Uncertain<T>& b) {
    return hypot(b, a);
}

template<typename T>
Interval<T> sqrt(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    auto low  = std::sqrt(a.lower());
    auto high = std::sqrt(a.upper());
    return Interval<T>(low, high, a.left_open(), a.right_open());
}

template<typename T>
Interval<T> exp(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    auto low  = std::exp(a.lower());
    auto high = std::exp(a.upper());
    return Interval<T>(low, high, a.left_open(), a.right_open());
}

template<typename T>
Interval<T> log(const Interval<T>& a) {
    if(a.empty()) { return Interval<T>(); }
    auto low  = std::log(a.lower());
    auto high = std::log(a.upper());
    return Interval<T>(low, high, a.left_open(), a.right_open());
}

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

template<typename T>
PartitionedAffine<T> sqrt(const PartitionedAffine<T>& a) {
    using partitions_t = typename PartitionedAffine<T>::partitions_t;
    using affines_t    = typename PartitionedAffine<T>::affines_t;

    partitions_t new_partitions;
    affines_t new_affines;
    for(const auto& [partition, affine] : a.partitioned_affines()) {
        auto new_affine        = sqrt(affine);
        new_affines[partition] = new_affine;
        new_partitions.push_back(partition);
    }
    return PartitionedAffine<T>(new_partitions, new_affines);
}

} // namespace sigma
