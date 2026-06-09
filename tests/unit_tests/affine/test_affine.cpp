#include "catch2/catch_test_macros.hpp"
#include "testing.hpp"
#include <sstream>

using testing::test_affine;

TEMPLATE_TEST_CASE("Affine", "", float, double) {
    using value_t      = TestType;
    using affine_t     = sigma::Affine<TestType>;
    using interval_t   = typename affine_t::interval_t;
    using error_term_t = typename affine_t::error_term_t;

    value_t zero  = 0.0;
    value_t one   = 1.0;
    value_t two   = 2.0;
    value_t three = 3.0;
    value_t four  = 4.0;

    SECTION("Constructors") {
        SECTION("Default") {
            affine_t empty;
            REQUIRE(empty.empty());
        }

        SECTION("From Center") {
            affine_t value(one);
            test_affine(value, one, one);

            affine_t value2(zero);
            test_affine(value2, zero, zero);
        }

        SECTION("From Lower and Upper") {
            affine_t value(one, two);
            test_affine(value, one, two);

            affine_t actually_point(one, one);
            test_affine(actually_point, one, one);
        }

        SECTION("From Interval") {
            affine_t empty(interval_t{});
            REQUIRE(empty.empty());

            affine_t value(interval_t(one, two));
            test_affine(value, one, two);

            affine_t value2(interval_t(-two, three));
            test_affine(value2, -two, three);
        }

        SECTION("From Center and Error Terms") {
            affine_t value(one, {{error_term_t{}, one}});
            test_affine(value, zero, two);
        }

        SECTION("Copy Constructor") {
            affine_t empty;
            affine_t copy_empty(empty);
            REQUIRE(copy_empty.empty());

            affine_t point(one);
            affine_t copy_point(point);
            test_affine(copy_point, one, one);

            affine_t interval(one, two);
            affine_t copy_interval(interval);
            test_affine(copy_interval, one, two);
        }

        SECTION("Move Constructor") {
            affine_t empty;
            affine_t move_empty(std::move(empty));
            REQUIRE(move_empty.empty());

            affine_t point(one);
            affine_t move_point(std::move(point));
            test_affine(move_point, one, one);

            affine_t interval(one, two);
            affine_t move_interval(std::move(interval));
            test_affine(move_interval, one, two);
        }

        SECTION("Copy Assignment") {
            affine_t empty;
            affine_t copy_empty;
            auto pcopy_empty = &(copy_empty = empty);
            REQUIRE(pcopy_empty == &copy_empty);
            REQUIRE(copy_empty.empty());

            affine_t point(one);
            affine_t copy_point;
            auto pcopy_point = &(copy_point = point);
            REQUIRE(pcopy_point == &copy_point);
            test_affine(copy_point, one, one);

            affine_t interval(one, two);
            affine_t copy_interval;
            auto pcopy_interval = &(copy_interval = interval);
            REQUIRE(pcopy_interval == &copy_interval);
            test_affine(copy_interval, one, two);
        }

        SECTION("Move Assignment") {
            affine_t empty;
            affine_t move_empty;
            auto pmove_empty = &(move_empty = std::move(empty));
            REQUIRE(pmove_empty == &move_empty);
            REQUIRE(move_empty.empty());

            affine_t point(one);
            affine_t move_point;
            auto pmove_point = &(move_point = std::move(point));
            REQUIRE(pmove_point == &move_point);
            test_affine(move_point, one, one);

            affine_t interval(one, two);
            affine_t move_interval;
            auto pmove_interval = &(move_interval = std::move(interval));
            REQUIRE(pmove_interval == &move_interval);
            test_affine(move_interval, one, two);
        }
    }

    SECTION("range") {
        affine_t empty;
        REQUIRE(empty.range().empty());

        affine_t point(one);
        REQUIRE(point.range() == interval_t(one, one));

        affine_t interval(one, two);
        REQUIRE(interval.range() == interval_t(one, two));
    }

    SECTION("center") {
        affine_t empty;
        REQUIRE_THROWS_AS(empty.center(), std::domain_error);

        affine_t point(one);
        REQUIRE(point.center() == one);

        affine_t interval(one, two);
        REQUIRE(interval.center() == value_t(1.5));
    }

    SECTION("error_terms") {
        affine_t empty;
        REQUIRE(empty.error_terms().empty());

        affine_t point(one);
        REQUIRE(point.error_terms().empty());

        affine_t interval(one, two);
        REQUIRE(interval.error_terms().size() == 1);
    }

    SECTION("radius") {
        affine_t empty;
        REQUIRE_THROWS_AS(empty.radius(), std::domain_error);

        affine_t point(one);
        REQUIRE(point.radius() == zero);

        affine_t interval(one, two);
        REQUIRE(interval.radius() == value_t(0.5));

        interval.add_error_term(error_term_t{}, value_t(-0.1));
        REQUIRE(interval.radius() == value_t(0.6));
    }

    SECTION("set_center") {
        affine_t empty;
        empty.set_center(one);
        test_affine(empty, one, one);

        affine_t point(one);
        point.set_center(two);
        test_affine(point, two, two);

        affine_t interval(one, two);
        interval.set_center(value_t(3.5));
        test_affine(interval, three, four);
    }

    SECTION("add_error_term") {
        affine_t empty;
        empty.add_error_term(error_term_t{}, one);
        test_affine(empty, -one, one);

        affine_t point(one);
        point.add_error_term(error_term_t{}, value_t(0.5));
        test_affine(point, value_t(0.5), value_t(1.5));

        affine_t interval(one, two);
        interval.add_error_term(error_term_t{}, value_t(0.25));
        test_affine(interval, value_t(0.75), value_t(2.25));
    }

    SECTION("contains(value)") {
        affine_t empty;
        REQUIRE_FALSE(empty.contains(zero));

        affine_t point(one);
        REQUIRE_FALSE(point.contains(zero));
        REQUIRE(point.contains(one));
        REQUIRE_FALSE(point.contains(two));

        affine_t interval(one, two);
        REQUIRE_FALSE(interval.contains(zero));
        REQUIRE(interval.contains(one));
        REQUIRE(interval.contains(value_t(1.5)));
        REQUIRE(interval.contains(two));
        REQUIRE_FALSE(interval.contains(three));
    }

    SECTION("contains(interval)") {
        affine_t empty;
        REQUIRE_FALSE(empty.contains(interval_t(zero, one)));

        affine_t point(one);
        REQUIRE_FALSE(point.contains(interval_t(zero, one)));
        REQUIRE(point.contains(interval_t(one, one)));
        REQUIRE(point.contains(interval_t{}));
        REQUIRE_FALSE(point.contains(interval_t(one, two)));

        affine_t interval(one, two);
        REQUIRE_FALSE(interval.contains(interval_t(zero, one)));
        REQUIRE(interval.contains(interval_t(one, one)));
        REQUIRE(interval.contains(interval_t(value_t(1.5), value_t(1.75))));
        REQUIRE(interval.contains(interval_t(two, two)));
        REQUIRE_FALSE(interval.contains(interval_t(two, three)));
    }

    SECTION("contains(affine)") {
        affine_t empty;
        affine_t point(one);
        affine_t interval(one, two);

        REQUIRE_FALSE(empty.contains(point));
        REQUIRE_FALSE(empty.contains(interval));

        REQUIRE(point.contains(empty));
        REQUIRE(point.contains(point));
        REQUIRE_FALSE(point.contains(interval));

        REQUIRE(interval.contains(empty));
        REQUIRE(interval.contains(point));
        REQUIRE(interval.contains(interval));
    }

    SECTION("empty") {
        affine_t empty;
        REQUIRE(empty.empty());

        affine_t point(one);
        REQUIRE_FALSE(point.empty());

        affine_t interval(one, two);
        REQUIRE_FALSE(interval.empty());
    }

    SECTION("print_affine_form") {
        affine_t empty;
        REQUIRE(empty.print_affine_form() == "∅");

        affine_t point(one);
        REQUIRE(point.print_affine_form() == "1");

        affine_t interval(one, two);
        REQUIRE(interval.print_affine_form() == "1.5 +/- 0.5");
    }

    SECTION("print_interval_form") {
        affine_t empty;
        REQUIRE(empty.print_interval_form() == "[∅]");

        affine_t point(one);
        std::stringstream corr;
        auto one_str = std::to_string(one);
        corr << "[" << one_str << ", " << one_str << "]";
        REQUIRE(point.print_interval_form() == corr.str());

        affine_t interval(one, two);
        std::stringstream corr2;
        auto two_str = std::to_string(two);
        corr2 << "[" << one_str << ", " << two_str << "]";
        REQUIRE(interval.print_interval_form() == corr2.str());
    }

    // SECTION("operator-") {
    //     auto value  = affine_t(one, two);
    //     auto value2 = -value;
    //     test_interval(value2.range(), -two, -one);
    // }

    // SECTION("operator+=") {
    //     SECTION("Value") {
    //         auto value = affine_t(one, two);
    //         value += value_t(3.0);
    //         test_interval(value.range(), four, value_t(5.0));
    //     }
    //     SECTION("Independent") {
    //         auto value = affine_t(one, two);
    //         value += affine_t(three, four);
    //         test_interval(value.range(), four, value_t(6.0));
    //     }
    //     SECTION("Dependent") {
    //         auto value = affine_t(one, two);
    //         value += value;
    //         test_interval(value.range(), two, four);
    //     }
    // }

    // SECTION("operator-=") {
    //     SECTION("Value") {
    //         auto value = affine_t(one, two);
    //         value -= value_t(3.0);
    //         test_interval(value.range(), -two, -one);
    //     }
    //     SECTION("Independent") {
    //         auto value = affine_t(one, two);
    //         value -= affine_t(three, four);
    //         test_interval(value.range(), -three, -one);
    //     }
    //     SECTION("Dependent") {
    //         auto value = affine_t(one, two);
    //         value -= value;
    //         test_interval(value.range(), zero, zero);
    //     }
    // }

    // SECTION("operator*=") {
    //     SECTION("Value") {
    //         auto value = affine_t(one, two);
    //         value *= value_t(3.0);
    //         test_interval(value.range(), three, value_t(6.0));
    //     }

    //     SECTION("Independent") {
    //         auto value = affine_t(one, two);
    //         value *= affine_t(three, four);
    //         // Tight range is [3, 8]
    //         test_interval(value.range(), value_t(2.5), value_t(8.0));
    //     }
    //     SECTION("Dependent") {
    //         auto value = affine_t(one, two);
    //         value *= value;
    //         // Tight range is [1, 4]
    //         test_interval(value.range(), value_t(0.5), value_t(4.0));

    //         affine_t value2(-two, two);
    //         value2 *= value2;
    //         // Tight range is [-4, 4]
    //         test_interval(value2.range(), -four, four);
    //     }
    // }

    // SECTION("operator/=") {
    //     SECTION("Value") {
    //         auto value = affine_t(one, two);
    //         value /= value_t(3.0);
    //         test_interval(value.range(), value_t(1.0 / 3.0),
    //                       value_t(2.0 / 3.0));
    //     }
    //     SECTION("Independent") {
    //         auto value = affine_t(one, two);
    //         value /= affine_t(three, four);
    //         // Tight range is [1/4, 2/3]
    //         test_interval(value.range(), value_t(0.2083333),
    //                       value_t(2.0 / 3.0));
    //     }
    //     SECTION("Dependent") {
    //         auto value = affine_t(one, two);
    //         value /= value;
    //         // Tight range is [1, 1]
    //         test_interval(value.range(), value_t(5.0 / 8.0),
    //                       value_t(13.0 / 8.0));
    //     }
    // }
}
