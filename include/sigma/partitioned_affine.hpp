#pragma once
#include <algorithm>
#include <map>
#include <sigma/affine.hpp>
#include <sigma/error_partition.hpp>
#include <sigma/interval.hpp>
#include <unordered_set>
#include <utility>
#include <vector>
/** @file partitioned_affine.hpp
 *  @brief Defines the PartitionedAffine class
 */

namespace sigma {

/** @brief Implements a partitioned affine interval.
 *
 *  A partitioned affine interval is an interval that is partitioned into a
 *  number of subintervals.
 */
template<typename ValueType>
class PartitionedAffine {
public:
    using value_t      = ValueType;
    using interval_t   = Interval<value_t>;
    using affine_t     = Affine<value_t>;
    using size_type    = typename affine_t::size_type;
    using error_term_t = typename affine_t::error_term_t;
    using partition_t  = ErrorPartition<value_t>;
    using partitions_t = std::vector<partition_t>;
    using affines_t    = std::unordered_map<partition_t, affine_t>;

    PartitionedAffine() = default;

    PartitionedAffine(value_t lower, value_t upper,
                      size_type num_partitions = 10) :
      PartitionedAffine(interval_t(lower, upper), num_partitions) {}

    PartitionedAffine(interval_t interval, size_type num_partitions = 10);

    PartitionedAffine(partitions_t partitions, affines_t affines) :
      m_partitions_(std::move(partitions)), m_affines_(std::move(affines)) {}

    bool contains(value_t value) const { return range().contains(value); }
    bool contains(const interval_t& interval) const {
        return range().contains(interval);
    }
    bool contains(const PartitionedAffine& other) const {
        return contains(other.range());
    }

    std::string print_interval_form() const {
        return range().print_interval_form();
    }

    std::string print_affine_form() const;

    size_type num_partitions() const { return m_partitions_.size(); }

    interval_t range(size_type i) const;

    interval_t range() const;

    value_t radius() const;

    value_t center() const;

    bool empty() const { return num_partitions() == 0; }
    void repartition(size_type n);

    PartitionedAffine operator-() const;

    // PartitionedAffine& operator+=(value_t value) {
    //     for(auto& interval : m_intervals_) { interval += value; }
    //     return *this;
    // }

    PartitionedAffine& operator+=(const PartitionedAffine& other);

    // PartitionedAffine operator-=(value_t value) {
    //     for(auto& interval : m_intervals_) { interval -= value; }
    //     return *this;
    // }

    /// Dependent @p a -= @p a: rebuild the strip from the Minkowski
    /// combination of the two @p m_certificate_ intervals, then reapply
    /// slack (same as a diagonal / template merge) so `partition_hull_`
    /// encloses the classical 1D outer.
    PartitionedAffine& operator-=(const PartitionedAffine& other) {
        return *this += -other;
    }

    // PartitionedAffine& operator*=(value_t value) {
    //     for(auto& interval : m_intervals_) { interval *= value; }
    //     return *this;
    // }

    PartitionedAffine& operator*=(const PartitionedAffine& other);

    // PartitionedAffine& operator/=(value_t value);

    PartitionedAffine& operator/=(const PartitionedAffine& other);

    PartitionedAffine apply_affine_transform(value_t alpha, value_t zeta,
                                             value_t delta) const;

    PartitionedAffine multiplicative_inverse() const;

private:
    void assert_partition_bound_(size_type a) const {
        if(a >= num_partitions()) {
            throw std::out_of_range("Partition index out of range");
        }
    }

    void assert_same_size_(const PartitionedAffine& other) const {
        if(num_partitions() != other.num_partitions()) {
            throw std::domain_error("PartitionedAffine sizes do not match");
        }
    }

    error_term_t make_error_term_() const {
        return std::make_shared<size_type>(0);
    }

    const auto& add_partition_(error_term_t error_term,
                               const interval_t& interval) {
        partition_t p;
        p.add_partition(error_term, interval);
        m_partitions_.push_back(p);
        return m_partitions_.back();
    }

    auto get_errors_(size_type i) const {
        std::unordered_set<error_term_t> errors;
        const auto& part_i        = m_partitions_[i];
        const auto& error_terms_i = m_affines_.at(part_i).error_terms();
        for(const auto& [error_symbol, error_term] : error_terms_i) {
            assert(part_i.count(error_symbol) > 0);
            errors.insert(error_symbol);
        }
        return errors;
    }

    auto get_common_errors_(size_type i, size_type j) const {
        const auto errors_i = get_errors_(i);
        const auto errors_j = get_errors_(j);
        std::unordered_set<error_term_t> common_errors;
        for(const auto& error_i : errors_i) {
            if(errors_j.count(error_i) > 0) { common_errors.insert(error_i); }
        }
        return common_errors;
    }

    auto get_all_errors_(size_type i, size_type j) const {
        auto all_errors      = get_errors_(i);
        const auto& errors_j = get_errors_(j);
        for(const auto& error_j : errors_j) { all_errors.insert(error_j); }
        return all_errors;
    }

    affine_t restrict_affine_(const affine_t& affine,
                              const partition_t& parts) const {
        auto new_center = affine.center();
        affine_t rv;
        for(const auto& [error_symbol, error_term] : affine.error_terms()) {
            if(parts.count(error_symbol) == 0) continue;
            auto range = parts.at(error_symbol);
            new_center += error_term * range.median();
            rv.add_error_term(error_symbol, error_term * range.radius());
        }
        rv.set_center(new_center);
        return rv;
    }

    partitions_t m_partitions_;
    affines_t m_affines_;
}; // namespace sigma

template<typename ValueType>
PartitionedAffine<ValueType>::PartitionedAffine(interval_t interval,
                                                size_type n) {
    if(n == 0) { return; }
    // Full affine: x = c + re
    // Partition e into n sub-intervals. a-th given by [c_a - r_a, c_a +
    // r_a] Partition width dx= 2 / n c_a - r_a = (-1 + a * dx) c_a + r_a =
    // (-1 + (a + 1) * dx) or 1 if a == n -1 e_a = c_a + r_a eta_a eta_a =
    // [-1, 1] (but different symbol from e) x_a = c + r(c_a + r_a eta_a)
    // c^x_a = c + rc_a
    // r^x_a = r * r_a
    value_t center       = interval.median();
    value_t radius       = interval.radius();
    value_t lower        = -1;
    value_t upper        = 1;
    auto partition_width = (upper - lower) / value_t(n);

    auto error_term = make_error_term_();

    affine_t full_affine;
    full_affine.set_center(center);
    full_affine.add_error_term(error_term, radius);
    for(size_type a = 0; a < n; ++a) {
        auto lower_a = lower + partition_width * a;
        auto upper_a = lower + partition_width * (a + 1);
        // Make the upper bound just slightly less than the lower bound of the
        // next partition to avoid overlap.
        upper_a = std::nextafter(upper_a, lower_a);
        if(a == n - 1) { upper_a = upper; }
        interval_t range_a(lower_a, upper_a);
        auto partition        = add_partition_(error_term, range_a);
        auto new_affine       = restrict_affine_(full_affine, partition);
        m_affines_[partition] = new_affine;
    }
}

template<typename ValueType>
std::string PartitionedAffine<ValueType>::print_affine_form() const {
    std::stringstream ss;
    if(empty()) { return ""; }
    ss << m_affines_.at(m_partitions_[0]).print_affine_form();
    for(size_type i = 1; i < num_partitions(); ++i) {
        ss << " + " << m_affines_.at(m_partitions_[i]).print_affine_form();
    }
    return ss.str();
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::radius() const -> value_t {
    return range().radius();
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::center() const -> value_t {
    return range().median();
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::range(size_type i) const -> interval_t {
    // Affine will bound check i
    return m_affines_.at(m_partitions_[i]).range();
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::range() const -> interval_t {
    if(empty()) { return interval_t(); }
    interval_t rv = range(0);
    value_t lower = rv.lower();
    value_t upper = rv.upper();
    // Skip taking unions incase noise made the ranges slightly disjoint
    for(size_type i = 1; i < num_partitions(); ++i) {
        auto range_i = range(i);
        lower        = std::min(lower, range_i.lower());
        upper        = std::max(upper, range_i.upper());
    }
    return interval_t(lower, upper);
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::apply_affine_transform(value_t alpha,
                                                          value_t zeta,
                                                          value_t delta) const
  -> PartitionedAffine {
    if(empty()) { return PartitionedAffine(); }
    partitions_t new_partitions;
    affines_t new_affines;
    for(const auto& [partition, affine] : m_affines_) {
        auto new_affine = affine.apply_affine_transform(alpha, zeta, delta);
        partition_t new_partition;
        for(const auto& [error_symbol, partition_i] : partition) {
            affine_t temp(partition_i);
            temp = temp.apply_affine_transform(alpha, zeta, delta);
            new_partition.add_partition(error_symbol, temp.range());
        }
        new_affines[new_partition] = new_affine;
        new_partitions.push_back(new_partition);
        assert(new_partitions.back() == new_partition);
    }
    return PartitionedAffine(std::move(new_partitions), std::move(new_affines));
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::operator-() const -> PartitionedAffine {
    if(empty()) { return PartitionedAffine(); }
    affines_t new_affines;
    for(const auto& [partition, affine] : m_affines_) {
        auto new_affine        = -affine;
        new_affines[partition] = new_affine;
    }
    return PartitionedAffine(m_partitions_, std::move(new_affines));
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::operator+=(const PartitionedAffine& other)
  -> PartitionedAffine& {
    partitions_t new_partitions;
    affines_t new_affines;

    using error_terms_t = std::unordered_map<error_term_t, value_t>;
    for(size_type i = 0; i < num_partitions(); ++i) {
        const auto& partition_i = m_partitions_[i];
        const auto& affine_i    = m_affines_.at(partition_i);

        for(size_type j = 0; j < other.num_partitions(); ++j) {
            const auto& partition_j = other.m_partitions_[j];
            const auto& affine_j    = other.m_affines_.at(partition_j);

            auto combined_partition = partition_i.align_with(partition_j);
            if(combined_partition.empty()) { continue; }
            auto i_restricted = restrict_affine_(affine_i, combined_partition);
            auto j_restricted = restrict_affine_(affine_j, combined_partition);

            auto new_center = i_restricted.center() + j_restricted.center();

            error_terms_t new_error_terms;
            for(const auto& error : combined_partition.error_set()) {
                value_t i_error_term = 0.0;
                if(i_restricted.error_terms().count(error) > 0) {
                    i_error_term = i_restricted.error_terms().at(error);
                }
                value_t j_error_term = 0.0;
                if(j_restricted.error_terms().count(error) > 0) {
                    j_error_term = j_restricted.error_terms().at(error);
                }
                new_error_terms[error] = i_error_term + j_error_term;
            }
            Affine new_affine(new_center, std::move(new_error_terms));
            new_partitions.push_back(combined_partition);
            new_affines[new_partitions.back()] = new_affine;
        }
    }

    return *this = PartitionedAffine(std::move(new_partitions),
                                     std::move(new_affines));
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::operator*=(const PartitionedAffine& other)
  -> PartitionedAffine& {
    partitions_t new_partitions;
    affines_t new_affines;

    using error_terms_t = std::unordered_map<error_term_t, value_t>;
    for(size_type i = 0; i < num_partitions(); ++i) {
        const auto& partition_i = m_partitions_[i];
        const auto& affine_i    = m_affines_.at(partition_i);

        for(size_type j = 0; j < other.num_partitions(); ++j) {
            const auto& partition_j = other.m_partitions_[j];
            const auto& affine_j    = other.m_affines_.at(partition_j);

            auto combined_partition = partition_i.align_with(partition_j);
            if(combined_partition.empty()) { continue; }
            auto i_restricted = restrict_affine_(affine_i, combined_partition);
            auto j_restricted = restrict_affine_(affine_j, combined_partition);

            auto i_center   = i_restricted.center();
            auto j_center   = j_restricted.center();
            auto new_center = i_center * j_center;

            error_terms_t new_error_terms;

            for(const auto& error : combined_partition.error_set()) {
                value_t i_error_term = 0.0;
                if(i_restricted.error_terms().count(error) > 0) {
                    i_error_term =
                      i_restricted.error_terms().at(error) * j_center;
                }
                value_t j_error_term = 0.0;
                if(j_restricted.error_terms().count(error) > 0) {
                    j_error_term =
                      j_restricted.error_terms().at(error) * i_center;
                }
                new_error_terms[error] = i_error_term + j_error_term;
            }

            value_t new_error = 0.0;
            for(const auto& i_error : i_restricted.error_terms()) {
                for(const auto& j_error : j_restricted.error_terms()) {
                    new_error += std::fabs(i_error.second * j_error.second);
                }
            }
            new_error_terms[make_error_term_()] = new_error;
            Affine new_affine(new_center, std::move(new_error_terms));
            new_partitions.push_back(combined_partition);
            new_affines[new_partitions.back()] = new_affine;
        }
    }

    return *this = PartitionedAffine(std::move(new_partitions),
                                     std::move(new_affines));
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::operator/=(const PartitionedAffine& other)
  -> PartitionedAffine& {
    partitions_t new_partitions;
    affines_t new_affines;

    using error_terms_t = std::unordered_map<error_term_t, value_t>;
    for(size_type i = 0; i < num_partitions(); ++i) {
        const auto& partition_i = m_partitions_[i];
        const auto& affine_i    = m_affines_.at(partition_i);

        for(size_type j = 0; j < other.num_partitions(); ++j) {
            const auto& partition_j = other.m_partitions_[j];
            const auto& affine_j    = other.m_affines_.at(partition_j);

            auto combined_partition = partition_i.align_with(partition_j);
            if(combined_partition.empty()) { continue; }
            auto i_restricted = restrict_affine_(affine_i, combined_partition);
            auto j_restricted = restrict_affine_(affine_j, combined_partition);
            auto one_over_j   = j_restricted.multiplicative_inverse();
            auto i_center     = i_restricted.center();
            auto j_center     = one_over_j.center();
            auto new_center   = i_center * j_center;

            error_terms_t new_error_terms;

            for(const auto& error : combined_partition.error_set()) {
                value_t i_error_term = 0.0;
                if(i_restricted.error_terms().count(error) > 0) {
                    i_error_term =
                      i_restricted.error_terms().at(error) * j_center;
                }
                value_t j_error_term = 0.0;
                if(one_over_j.error_terms().count(error) > 0) {
                    j_error_term =
                      one_over_j.error_terms().at(error) * i_center;
                }
                new_error_terms[error] = i_error_term + j_error_term;
            }

            value_t new_error = 0.0;
            for(const auto& i_error : i_restricted.error_terms()) {
                for(const auto& j_error : one_over_j.error_terms()) {
                    new_error += std::fabs(i_error.second * j_error.second);
                }
            }
            new_error_terms[make_error_term_()] = new_error;
            Affine new_affine(new_center, std::move(new_error_terms));
            new_partitions.push_back(combined_partition);
            new_affines[new_partitions.back()] = new_affine;
        }
    }

    return *this = PartitionedAffine(std::move(new_partitions),
                                     std::move(new_affines));
}

// template<typename ValueType>
// void PartitionedAffine<ValueType>::repartition(size_type n_new) {
//     auto n_old = num_partitions();
//     if(n_new == n_old) { return; }
//     center_t new_centers(n_new);
//     std::vector<value_t> widths(n_new);
//     {
//         auto range = partition_range_();
//         auto width = range.width() / ValueType(n_new);
//         auto lower = range.lower();
//         auto upper = range.upper();

//         for(size_type i = 0; i < n_new; ++i) {
//             auto low_edge  = lower + width * i;
//             auto high_edge = lower + width * (i + 1);
//             if(i == n_new - 1) { high_edge = upper; }
//             widths[i]      = (high_edge - low_edge) / ValueType(2.0);
//             new_centers[i] = (low_edge + high_edge) / ValueType(2.0);
//         }
//     }
//     radii_t new_radii;

//     // Map from error term to overlaps.
//     // Each overlap is the amount of the old range that overlaps with
//     the j-th
//     // partition of the new range
//     std::unordered_map<error_term_t, radius_t> overlaps;
//     for(auto&& [error_term, radius_i] : m_radii_) {
//         if(radius_i.size() == 0) { continue; }

//         std::vector<value_t> overlap(n_new, 0.0);
//         for(size_type i = 0; i < radius_i.size(); ++i) {
//             // Make the i-th old_range
//             auto old_lo = m_centers_[i] - radius_i[i];
//             auto old_hi = m_centers_[i] + radius_i[i];

//             interval_t old_range(old_lo, old_hi);
//             for(size_type j = 0; j < n_new; ++j) {
//                 // Make the j-th new_range
//                 auto new_lo = new_centers[j] - widths[j];
//                 auto new_hi = new_centers[j] + widths[j];
//                 interval_t new_range(new_lo, new_hi);

//                 auto intersection =
//                 old_range.set_intersection(new_range);
//                 if(intersection.empty()) { continue; }
//                 overlap[j] += intersection.width();
//             }
//         }
//         overlaps[error_term] = overlap;
//     }

//     // Calculate the total overlap, across all error terms, for each
//     new
//     // partition
//     radius_t overlap_sum(n_new, 0.0);
//     for(auto&& [error_term, overlap] : overlaps) {
//         for(size_type i = 0; i < n_new; ++i) { overlap_sum[i] +=
//         overlap[i];
//         }
//     }

//     // New radius
//     for(auto&& [error_term, radius_i] : m_radii_) {
//         radius_t new_radius(n_new, 0.0);
//         auto& overlap_i = overlaps[error_term];
//         for(size_type i = 0; i < n_new; ++i) {
//             new_radius[i] = widths[i] * overlap_i[i] /
//             overlap_sum[i];
//         }
//         new_radii[error_term] = new_radius;
//     }

//     m_centers_ = new_centers;
//     m_radii_   = new_radii;
// }

} // namespace sigma
