#pragma once
#include <functional>
#include <sigma/interval.hpp>
#include <unordered_map>
#include <unordered_set>

namespace sigma {

template<typename ValueType>
class ErrorPartition {
public:
    using size_type            = std::size_t;
    using error_term_t         = std::shared_ptr<size_type>;
    using interval_t           = Interval<ValueType>;
    using error_to_partition_t = std::unordered_map<error_term_t, interval_t>;
    using error_set_t          = std::unordered_set<error_term_t>;

    ErrorPartition() = default;

    ErrorPartition(error_to_partition_t partitions) :
      m_partitions_(std::move(partitions)) {}

    void add_partition(error_term_t error_term, interval_t interval) {
        m_partitions_[error_term] = interval;
    }

    bool count(error_term_t error_term) const {
        return m_partitions_.count(error_term) > 0;
    }

    const auto& at(error_term_t error_term) const {
        assert_error_term_exists_(error_term);
        return m_partitions_.at(error_term);
    }

    error_set_t error_set() const {
        error_set_t errors;
        for(const auto& [error_term, interval] : m_partitions_) {
            errors.insert(error_term);
        }
        return errors;
    }

    ErrorPartition align_with(const ErrorPartition& other) const {
        ErrorPartition rv;
        for(const auto& [error_symbol, interval_i] : m_partitions_) {
            if(other.count(error_symbol)) {
                const auto& interval_j = other.at(error_symbol);
                auto aligned = interval_i.set_intersection(interval_j);
                // If empty then this partition is NOT compatible
                // with other
                if(aligned.empty()) { return ErrorPartition(); }

                rv.add_partition(error_symbol, aligned);
            } else {
                rv.add_partition(error_symbol, interval_i);
            }
        }
        for(const auto& [error_symbol, interval_j] : other.m_partitions_) {
            if(count(error_symbol)) { continue; }
            rv.add_partition(error_symbol, interval_j);
        }
        return rv;
    }

    auto hash() const {
        std::size_t seed = 0;
        for(const auto& [error_term, interval] : m_partitions_) {
            auto hash_error    = std::hash<error_term_t>()(error_term);
            auto hash_interval = std::hash<interval_t>()(interval);
            seed ^= hash_error + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash_interval + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    auto begin() const { return m_partitions_.begin(); }
    auto end() const { return m_partitions_.end(); }
    auto size() const { return m_partitions_.size(); }
    auto empty() const { return m_partitions_.empty(); }

    bool operator==(const ErrorPartition& other) const {
        return m_partitions_ == other.m_partitions_;
    }

    bool operator!=(const ErrorPartition& other) const {
        return !(*this == other);
    }

private:
    void assert_error_term_exists_(error_term_t error_term) const {
        if(!count(error_term)) {
            throw std::out_of_range("Error term does not exist");
        }
    }

    error_to_partition_t m_partitions_;
};

} // namespace sigma

namespace std {
template<typename ValueType>
struct hash<sigma::ErrorPartition<ValueType>> {
    size_t operator()(const sigma::ErrorPartition<ValueType>& i) const {
        return i.hash();
    }
};
} // namespace std
