#pragma once
#include <iterator>
#include <numeric>
#include <sigma/interval.hpp>
#include <sstream>

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
    using interval_t   = Interval<ValueType>;
    using size_type    = std::size_t;
    using error_term_t = std::shared_ptr<size_type>;
    using center_t     = std::vector<value_t>;
    using radius_t     = std::vector<value_t>;
    using radii_t      = std::unordered_map<error_term_t, radius_t>;

    PartitionedAffine() = default;

    PartitionedAffine(value_t lower, value_t upper,
                      size_type num_partitions = 10) :
      PartitionedAffine(interval_t(lower, upper), num_partitions) {}

    PartitionedAffine(const interval_t& interval,
                      size_type num_partitions = 10);

    PartitionedAffine(center_t centers, radii_t radii) :
      m_centers_(std::move(centers)), m_radii_(std::move(radii)) {}

    bool contains(value_t value) const { return range().contains(value); }

    bool contains(const interval_t& interval) const {
        return range().contains(interval);
    }

    // bool contains(const affine_t& affine) const {
    //     return contains(affine.range());
    // }

    // bool contains(const PartitionedAffine& other) const {
    //     return contains(other.range());
    // }

    std::string print_interval_form() const {
        return range().print_interval_form();
    }

    std::string print_affine_form() const;

    size_type num_partitions() const { return m_centers_.size(); }

    interval_t range(size_type i) const;

    interval_t range() const;

    value_t radius() const;

    value_t center() const;

    bool empty() const { return m_centers_.empty(); }

    // interval_t traditional_interval() const {
    //     if(m_intervals_.empty()) { return interval_t(); }
    //     interval_t rv = m_intervals_[0].traditional_interval();
    //     for(size_type i = 1; i < m_intervals_.size(); ++i) {
    //         rv = rv.set_union(m_intervals_[i].traditional_interval());
    //     }
    //     return rv;
    // }

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

    PartitionedAffine& operator-=(const PartitionedAffine& other) {
        auto temp = -other;
        return *this += temp;
    }

    // PartitionedAffine& operator*=(value_t value) {
    //     for(auto& interval : m_intervals_) { interval *= value; }
    //     return *this;
    // }

    PartitionedAffine& operator*=(const PartitionedAffine& other);

    // PartitionedAffine& operator/=(value_t value) {
    //     for(auto& interval : m_intervals_) { interval /= value; }
    //     return *this;
    // }

    // PartitionedAffine& operator/=(const PartitionedAffine& other);

private:
    std::string print_vector_(const center_t& vector) const {
        std::stringstream ss;
        ss << "(" << vector[0];
        for(size_type i = 0; i < vector.size(); ++i) {
            ss << ", " << vector[i];
        }
        ss << ")";
        return ss.str();
    }

    void assert_same_size(const PartitionedAffine& other) const {
        if(num_partitions() != other.num_partitions()) {
            throw std::domain_error("PartitionedAffine sizes do not match");
        }
    }

    error_term_t make_error_term_() {
        return std::make_shared<size_type>(m_radii_.size());
    }

    center_t m_centers_;
    radii_t m_radii_;
};

template<typename ValueType>
PartitionedAffine<ValueType>::PartitionedAffine(const interval_t& interval,
                                                size_type n) {
    auto partition_width = interval.width() / value_t(n);
    auto lower           = interval.lower();
    auto upper           = interval.upper();

    // Index of the last partition
    auto nm1 = n - 1;

    // dx_i = partition_width * i
    // interval_i = [lower + dx_i, lower + dx_{i+1}]
    // center_i   = (lower + dx_i + lower + dx_{i+1}) / 2
    // radius_i   = (lower + dx_{i+1} - lower - dx_i) / 2
    // n.b. to ensure a partitioning, partition n - 1 has uppeer bound upper,
    // "upper" instead of "lower + dx_{n-1}".
    radius_t radius;
    for(size_type i = 0; i < nm1; ++i) {
        auto lower_i  = lower + partition_width * i;
        auto upper_i  = lower + partition_width * (i + 1);
        auto center_i = (lower_i + upper_i) / value_t(2.0);
        auto radius_i = (upper_i - lower_i) / value_t(2.0);
        m_centers_.push_back(center_i);
        radius.push_back(radius_i);
    }
    auto lower_i = lower + partition_width * nm1;
    m_centers_.push_back((lower_i + upper) / value_t(2.0));
    radius.push_back((upper - lower_i) / value_t(2.0));
    m_radii_[make_error_term_()] = std::move(radius);
}

template<typename ValueType>
std::string PartitionedAffine<ValueType>::print_affine_form() const {
    std::stringstream ss;
    ss << print_vector_(m_centers_);
    std::size_t i = 0;
    for(auto&& [error_term, radius_i] : m_radii_) {
        ss << " + " << print_vector_(radius_i) << "ε" << i++;
    }
    return ss.str();
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::radius() const -> value_t {
    value_t rv = 0.0;
    for(auto&& [error_term, radius_i] : m_radii_) {
        for(size_type i = 0; i < radius_i.size(); ++i) {
            rv += std::fabs(radius_i[i]);
        }
    }
    return rv;
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::center() const -> value_t {
    return range().median();
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::range(size_type i) const -> interval_t {
    if(i >= num_partitions()) {
        throw std::out_of_range("Partition index out of range");
    }
    value_t center_i = m_centers_[i];
    value_t radius_i = 0.0;
    for(auto&& [error_term, radius_j] : m_radii_) {
        radius_i += std::fabs(radius_j[i]);
    }
    return interval_t(center_i - radius_i, center_i + radius_i);
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::range() const -> interval_t {
    if(empty()) { return interval_t(); }
    interval_t rv(range(0));
    for(size_type i = 1; i < num_partitions(); ++i) {
        auto range_i = range(i);
        auto lo      = std::min(rv.lower(), range_i.lower());
        auto hi      = std::max(rv.upper(), range_i.upper());
        rv           = interval_t(lo, hi);
    }
    return rv;
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::operator-() const -> PartitionedAffine {
    if(empty()) { return PartitionedAffine(); }
    center_t new_centers(m_centers_);
    for(auto& center : new_centers) { center = -center; }

    radii_t new_radii(m_radii_);
    for(auto&& [error_term, radius_i] : new_radii) {
        for(size_type i = 0; i < radius_i.size(); ++i) {
            radius_i[i] = -radius_i[i];
        }
    }

    return PartitionedAffine(new_centers, new_radii);
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::operator+=(const PartitionedAffine& other)
  -> PartitionedAffine& {
    assert_same_size(other);
    center_t new_centers(m_centers_);
    radii_t new_radii(m_radii_);

    for(size_type i = 0; i < new_centers.size(); ++i) {
        new_centers[i] += other.m_centers_[i];
    }

    for(auto&& [error_term, radius_i] : other.m_radii_) {
        if(new_radii.count(error_term) == 0) {
            new_radii[error_term] = radius_i;
        } else {
            for(size_type i = 0; i < radius_i.size(); ++i) {
                new_radii[error_term][i] += radius_i[i];
            }
        }
    }
    return *this = PartitionedAffine(new_centers, new_radii);
}

template<typename ValueType>
auto PartitionedAffine<ValueType>::operator*=(const PartitionedAffine& other)
  -> PartitionedAffine& {
    assert_same_size(other);
    auto n_partitions = num_partitions();

    // Update centers and traditional intervals
    center_t new_centers(m_centers_);
    for(size_type i = 0; i < n_partitions; ++i) {
        new_centers[i] *= other.m_centers_[i];
    }

    radii_t new_radii(m_radii_);
    radius_t this_radii(n_partitions, 0.0);
    radius_t other_radii(n_partitions, 0.0);

    // z_i = x_i * other.center_i
    for(auto&& [error_term, radius_i] : m_radii_) {
        for(size_type i = 0; i < n_partitions; ++i) {
            new_radii[error_term][i] *= other.m_centers_[i];
            this_radii[i] += std::fabs(radius_i[i]);
        }
    }

    // z_i = center_i * other.x_i
    for(auto&& [error_term, radius_i] : other.m_radii_) {
        if(new_radii.count(error_term) == 0) {
            new_radii[error_term] = radius_t(radius_i.size(), 0.0);
        }
        for(size_type i = 0; i < n_partitions; ++i) {
            new_radii[error_term][i] += radius_i[i] * m_centers_[i];
            other_radii[i] += std::fabs(radius_i[i]);
        }
    }

    // New error term
    radius_t new_radius(n_partitions, 0.0);
    for(size_type i = 0; i < n_partitions; ++i) {
        for(size_type j = 0; j < n_partitions; ++j) {
            new_radius[i] += this_radii[i] * other_radii[j];
        }
    }
    new_radii[make_error_term_()] = new_radius;
    return *this                  = PartitionedAffine(new_centers, new_radii);
}

// template<typename ValueType>
// auto PartitionedAffine<ValueType>::operator/=(const
// PartitionedAffine& other)
//   -> PartitionedAffine& {
//     std::vector<affine_t> new_intervals;
//     for(size_type i = 0; i < m_intervals_.size(); ++i) {
//         for(size_type j = 0; j < other.m_intervals_.size(); ++j) {
//             new_intervals.push_back(m_intervals_[i] /
//             other.m_intervals_[j]);
//         }
//     }
//     interval_t new_interval = new_intervals[0].range();

//     for(size_type i = 1; i < new_intervals.size(); ++i) {
//         new_interval =
//         new_interval.set_union(new_intervals[i].range());
//     }
//     PartitionNumber n{m_intervals_.size()};
//     return *this = PartitionedAffine(new_interval, n);
// }

} // namespace sigma
