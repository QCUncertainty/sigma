#include "testing.hpp"
#include <sigma/error_partition.hpp>

TEMPLATE_TEST_CASE("ErrorPartition", "", float, double) {
    using partition_t          = sigma::ErrorPartition<TestType>;
    using interval_t           = typename partition_t::interval_t;
    using error_term_t         = typename partition_t::error_term_t;
    using error_to_partition_t = typename partition_t::error_to_partition_t;
    using error_set_t          = typename partition_t::error_set_t;

    interval_t i0(0.0, 1.0);
    interval_t i1(-1.0, 1.0);
    error_term_t e0 = std::make_shared<std::size_t>(1.0);
    error_term_t e1 = std::make_shared<std::size_t>(2.0);

    error_to_partition_t map;
    map[e0] = i0;

    SECTION("Constructor") {
        SECTION("Default") {
            partition_t defaulted;
            REQUIRE(defaulted.size() == 0);
        }

        SECTION("From map") {
            partition_t value(map);
            REQUIRE(value.count(e0));
            REQUIRE(value.at(e0) == i0);
            REQUIRE(value.size() == 1);
        }

        SECTION("Copy Constructor") {
            partition_t value(map);
            partition_t value2(value);
            REQUIRE(value2 == value);
            REQUIRE(value2.count(e0));
            REQUIRE(value2.at(e0) == i0);
            REQUIRE(value2.size() == 1);
        }
    }

    SECTION("add_partition") {
        partition_t value;
        REQUIRE(value.size() == 0);
        value.add_partition(e0, i0);
        REQUIRE(value.size() == 1);
        REQUIRE(value.count(e0));
        REQUIRE(value.at(e0) == i0);

        value.add_partition(e1, i1);
        REQUIRE(value.size() == 2);
        REQUIRE(value.count(e1));
        REQUIRE(value.at(e1) == i1);
    }

    SECTION("count") {
        partition_t value;
        value.add_partition(e0, i0);
        REQUIRE(value.count(e0));
        REQUIRE_FALSE(value.count(e1));
    }

    SECTION("at") {
        partition_t value;
        value.add_partition(e0, i0);
        REQUIRE(value.at(e0) == i0);
        REQUIRE_THROWS_AS(value.at(e1), std::out_of_range);
    }

    SECTION("error_set") {
        partition_t value;
        REQUIRE(value.error_set() == error_set_t{});

        value.add_partition(e0, i0);
        REQUIRE(value.error_set() == error_set_t{e0});

        value.add_partition(e1, i1);
        REQUIRE(value.error_set() == error_set_t{e0, e1});
    }

    SECTION("align_with") {
        partition_t value;
        partition_t value2;
        REQUIRE(value.align_with(value2) == partition_t());
        REQUIRE(value2.align_with(value) == partition_t());

        value.add_partition(e0, i0);
        REQUIRE(value.align_with(value) == partition_t(map));
        REQUIRE(value.align_with(value2) == partition_t(map));

        value2.add_partition(e0, i1);
        REQUIRE(value.align_with(value2) == value);

        partition_t value3;
        value3.add_partition(e1, i1);

        partition_t corr;
        corr.add_partition(e0, i0);
        corr.add_partition(e1, i1);
        REQUIRE(value.align_with(value3) == corr);
    }

    SECTION("size") {
        partition_t value;
        REQUIRE(value.size() == 0);
        value.add_partition(e0, i0);
        REQUIRE(value.size() == 1);
        value.add_partition(e1, i1);
        REQUIRE(value.size() == 2);
    }

    SECTION("empty") {
        partition_t value;
        REQUIRE(value.empty());
        value.add_partition(e0, i0);
        REQUIRE_FALSE(value.empty());
    }

    SECTION("operator==") {
        partition_t value;
        REQUIRE(value == partition_t());

        partition_t value2(map);
        REQUIRE(value2 == partition_t(map));
        REQUIRE_FALSE(value2 == value);
    }

    SECTION("operator!=") {
        partition_t value;
        REQUIRE_FALSE(value != partition_t());

        partition_t value2(map);
        REQUIRE_FALSE(value2 != partition_t(map));
        REQUIRE(value2 != value);
    }
}
