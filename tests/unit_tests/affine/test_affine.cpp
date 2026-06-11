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
            REQUIRE(copy_empty == empty);

            affine_t point(one);
            affine_t copy_point(point);
            test_affine(copy_point, one, one);
            REQUIRE(copy_point == point);

            affine_t interval(one, two);
            affine_t copy_interval(interval);
            test_affine(copy_interval, one, two);
            REQUIRE(copy_interval == interval);
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
            REQUIRE(copy_empty == empty);

            affine_t point(one);
            affine_t copy_point;
            auto pcopy_point = &(copy_point = point);
            REQUIRE(pcopy_point == &copy_point);
            test_affine(copy_point, one, one);
            REQUIRE(copy_point == point);

            affine_t interval(one, two);
            affine_t copy_interval;
            auto pcopy_interval = &(copy_interval = interval);
            REQUIRE(pcopy_interval == &copy_interval);
            test_affine(copy_interval, one, two);
            REQUIRE(copy_interval == interval);
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

    SECTION("operator-") {
        affine_t empty;
        REQUIRE((-empty).empty());

        affine_t point(one);
        auto neg_point = -point;
        test_affine(neg_point, -one, -one);

        affine_t interval(one, two);
        auto neg_interval = -interval;
        test_affine(neg_interval, -two, -one);
    }

    SECTION("operator+=(value)") {
        affine_t empty;
        auto pempty = &(empty += one);
        REQUIRE(pempty == &empty);
        test_affine(empty, one, one);

        affine_t point(one);
        auto ppoint = &(point += two);
        REQUIRE(ppoint == &point);
        test_affine(point, three, three);

        affine_t interval(one, two);
        auto pinterval = &(interval += three);
        REQUIRE(pinterval == &interval);
        test_affine(interval, four, value_t(5.0));
    }

    SECTION("operator+=(Affine)") {
        affine_t empty;
        auto pempty = &(empty += affine_t(one, two));
        REQUIRE(pempty == &empty);
        test_affine(*pempty, one, two);

        affine_t point(one);
        auto ppoint = &(point += affine_t(two, three));
        REQUIRE(ppoint == &point);
        test_affine(point, three, four);

        affine_t interval(one, two);
        auto pinterval = &(interval += affine_t(three, four));
        REQUIRE(pinterval == &interval);
        test_affine(interval, four, value_t(6.0));

        // Test the addition of dependent errors
        // x = 1.5 +/- 0.5 e_0 +/- 1.0 e_1
        affine_t dependent(one, two);
        dependent.add_error_term(error_term_t{}, one);

        // y = 1 -/+ 1 e_1
        affine_t other_dependent;
        other_dependent.set_center(one);
        other_dependent.add_error_term(error_term_t{}, -one);
        auto pdependent = &(dependent += other_dependent);
        REQUIRE(pdependent == &dependent);
        test_affine(dependent, two, three);
    }

    SECTION("operator+(value)") {
        // Implemented in terms of operator+= so just spot check
        affine_t empty;
        auto sum_empty = empty + one;
        test_affine(sum_empty, one, one);

        affine_t point(one);
        auto sum_point = point + two;
        test_affine(sum_point, three, three);

        affine_t interval(one, two);
        auto sum_interval = interval + three;
        test_affine(sum_interval, four, value_t(5.0));
    }

    SECTION("operator+(Affine)") {
        // Implemented in terms of operator+= so just spot check
        affine_t empty;
        auto sum_empty = empty + affine_t(one, two);
        test_affine(sum_empty, one, two);

        affine_t point(one);
        auto sum_point = point + affine_t(two, three);
        test_affine(sum_point, three, four);

        affine_t interval(one, two);
        auto sum_interval = interval + affine_t(three, four);
        test_affine(sum_interval, four, value_t(6.0));
    }

    SECTION("operator-=(value)") {
        affine_t empty;
        auto pempty = &(empty -= one);
        REQUIRE(pempty == &empty);
        test_affine(empty, -one, -one);

        affine_t point(one);
        auto ppoint = &(point -= two);
        REQUIRE(ppoint == &point);
        test_affine(point, -one, -one);

        affine_t interval(one, two);
        auto pinterval = &(interval -= three);
        REQUIRE(pinterval == &interval);
        test_affine(interval, value_t(-2.0), value_t(-1.0));
    }

    SECTION("operator-=(Affine)") {
        affine_t empty;
        auto pempty = &(empty -= affine_t(one, two));
        REQUIRE(pempty == &empty);
        test_affine(*pempty, value_t(-2.0), value_t(-1.0));

        affine_t point(one);
        auto ppoint = &(point -= affine_t(two, three));
        REQUIRE(ppoint == &point);
        test_affine(point, value_t(-2.0), value_t(-1.0));

        affine_t point2(one);
        auto ppoint2 = &(point2 -= affine_t{});
        REQUIRE(ppoint2 == &point2);
        test_affine(point2, one, one);

        affine_t interval(one, two);
        auto pinterval = &(interval -= affine_t(three, four));
        REQUIRE(pinterval == &interval);
        test_affine(interval, value_t(-3.0), value_t(-1.0));

        // Test the subtraction of dependent errors
        affine_t dependent(one, two);
        auto pdependent = &(dependent -= dependent);
        REQUIRE(pdependent == &dependent);
        test_affine(dependent, zero, zero);
    }

    SECTION("operator-(value)") {
        // Implemented in terms of operator-= so just spot check
        affine_t empty;
        auto diff_empty = empty - one;
        test_affine(diff_empty, -one, -one);

        affine_t point(one);
        auto diff_point = point - two;
        test_affine(diff_point, -one, -one);

        affine_t interval(one, two);
        auto diff_interval = interval - three;
        test_affine(diff_interval, value_t(-2.0), value_t(-1.0));
    }

    SECTION("operator-(Affine)") {
        // Implemented in terms of operator-= so just spot check
        affine_t empty;
        auto diff_empty = empty - affine_t(one, two);
        test_affine(diff_empty, value_t(-2.0), value_t(-1.0));

        affine_t point(one);
        auto diff_point = point - affine_t(two, three);
        test_affine(diff_point, value_t(-2.0), value_t(-1.0));

        affine_t point2(one);
        auto diff_point2 = point2 - affine_t{};
        test_affine(diff_point2, one, one);

        affine_t interval(one, two);
        auto diff_interval = interval - affine_t(three, four);
        test_affine(diff_interval, value_t(-3.0), value_t(-1.0));
    }

    SECTION("operator*=(value)") {
        affine_t empty;
        auto pempty = &(empty *= one);
        REQUIRE(pempty == &empty);
        REQUIRE(empty.empty());

        affine_t point(one);
        auto ppoint = &(point *= two);
        REQUIRE(ppoint == &point);
        test_affine(point, two, two);

        affine_t interval(one, two);
        auto pinterval = &(interval *= two);
        REQUIRE(pinterval == &interval);
        test_affine(interval, two, four);
    }

    SECTION("operator*=(Affine)") {
        affine_t empty;
        auto pempty = &(empty *= affine_t(one, two));
        REQUIRE(pempty == &empty);
        REQUIRE(empty.empty());

        affine_t point(one);
        auto ppoint = &(point *= affine_t(two, three));
        REQUIRE(ppoint == &point);
        test_affine(point, two, three);

        affine_t interval(one, two);
        auto pinterval = &(interval *= affine_t(three, four));
        REQUIRE(pinterval == &interval);
        // N.b. the tight range is [3, 8], but the correct answer is [2.5, 8]
        // due to nonlinearity bound overestimating the error.
        test_affine(interval, value_t(2.5), value_t(8.0));

        // Test the multiplication of dependent errors
        affine_t dependent(one, two);
        auto pdependent = &(dependent *= dependent);
        REQUIRE(pdependent == &dependent);
        // N.b. the tight range is [1, 4], but the correct answer is [0.5, 4]
        // due to nonlinearity bound overestimating the error.
        test_affine(dependent, value_t(0.5), value_t(4.0));

        affine_t other_dependent(-two, two);
        auto pother_dependent = &(other_dependent *= other_dependent);
        REQUIRE(pother_dependent == &other_dependent);
        test_affine(other_dependent, value_t(-4.0), value_t(4.0));
    }

    SECTION("operator*(value)") {
        // Implemented in terms of operator*= so just spot check
        affine_t empty;
        auto prod_empty = empty * one;
        REQUIRE(prod_empty.empty());

        affine_t point(one);
        auto prod_point = point * two;
        test_affine(prod_point, two, two);

        affine_t interval(one, two);
        auto prod_interval = interval * two;
        test_affine(prod_interval, two, four);
    }

    SECTION("operator*(Affine)") {
        // Implemented in terms of operator*= so just spot check
        affine_t empty;
        auto prod_empty = empty * affine_t(one, two);
        REQUIRE(prod_empty.empty());

        affine_t point(one);
        auto prod_point = point * affine_t(two, three);
        test_affine(prod_point, two, three);

        affine_t interval(one, two);
        auto prod_interval = interval * affine_t(three, four);
        test_affine(prod_interval, value_t(2.5), value_t(8.0));
    }

    SECTION("operator/=(value)") {
        affine_t empty;
        auto pempty = &(empty /= one);
        REQUIRE(pempty == &empty);
        REQUIRE(empty.empty());

        affine_t point(one);
        auto ppoint = &(point /= two);
        REQUIRE(ppoint == &point);
        test_affine(point, value_t(0.5), value_t(0.5));

        affine_t interval(one, two);
        auto pinterval = &(interval /= two);
        REQUIRE(pinterval == &interval);
        test_affine(interval, value_t(0.5), value_t(1.0));

        REQUIRE_THROWS_AS(point /= zero, std::domain_error);
    }

    SECTION("operator/=(Affine)") {
        affine_t empty;
        auto pempty = &(empty /= affine_t(one, two));
        REQUIRE(pempty == &empty);
        REQUIRE(empty.empty());

        affine_t point(one);
        auto ppoint = &(point /= affine_t(two, three));
        REQUIRE(ppoint == &point);
        test_affine(point, value_t(1.0 / 3.0), value_t(0.5));

        affine_t interval(one, two);
        auto pinterval = &(interval /= affine_t(three, four));
        REQUIRE(pinterval == &interval);
        // N.b. the tight range is [1/4, 2/3], but the correct answer is
        // [0.2083333, 0.6666667] due to nonlinearity bound overestimating the
        // error.
        test_affine(interval, value_t(0.2083333), value_t(2.0 / 3.0));

        affine_t interval2(one, two);
        auto pinterval2 = &(interval2 /= interval2);
        REQUIRE(pinterval2 == &interval2);
        // N.b. the tight range is [1, 1] but the correct answer is [5/8, 13/8]
        // due to nonlinearity bound overestimating the error.
        test_affine(interval2, value_t(5.0 / 8.0), value_t(13.0 / 8.0));

        REQUIRE_THROWS_AS(point /= affine_t(zero, zero), std::domain_error);
    }

    SECTION("operator/(value)") {
        // Implemented in terms of operator/= so just spot check
        affine_t empty;
        auto quot_empty = empty / one;
        REQUIRE(quot_empty.empty());

        affine_t point(one);
        auto quot_point = point / two;
        test_affine(quot_point, value_t(0.5), value_t(0.5));

        affine_t interval(one, two);
        auto quot_interval = interval / two;
        test_affine(quot_interval, value_t(0.5), value_t(1.0));

        affine_t interval2(one, two);
        REQUIRE_THROWS_AS(interval2 / zero, std::domain_error);
    }

    SECTION("operator/(Affine)") {
        // Implemented in terms of operator/= so just spot check
        affine_t empty;
        auto quot_empty = empty / affine_t(one, two);
        REQUIRE(quot_empty.empty());

        affine_t point(one);
        auto quot_point = point / affine_t(two, three);
        test_affine(quot_point, value_t(1.0 / 3.0), value_t(0.5));

        affine_t interval(one, two);
        auto quot_interval = interval / affine_t(three, four);
        test_affine(quot_interval, value_t(0.2083333), value_t(2.0 / 3.0));

        affine_t interval2(one, two);
        auto quot_interval2 = interval2 / interval2;
        test_affine(quot_interval2, value_t(5.0 / 8.0), value_t(13.0 / 8.0));

        REQUIRE_THROWS_AS(point / affine_t(zero, zero), std::domain_error);
    }

    SECTION("apply_affine_transform") {
        affine_t empty;
        auto transform_empty = empty.apply_affine_transform(one, one, one);
        test_affine(transform_empty, zero, two);

        affine_t point(one);
        auto transform_point = point.apply_affine_transform(two, three, four);
        test_affine(transform_point, value_t(1.0), value_t(9.0));

        // interval = 1.5 +/- 0.5 e_0
        // 2 * 1.5 + 3 +/- 2 * 0.5 e_0 +/- 4 e_1 = 6 +/- 1 e_0 +/- 4 e_1
        affine_t interval(one, two);
        auto transform_interval =
          interval.apply_affine_transform(two, three, four);
        test_affine(transform_interval, value_t(1.0), value_t(11.0));
    }

    SECTION("multiplicative_inverse") {
        affine_t empty;
        REQUIRE_THROWS_AS(empty.multiplicative_inverse(), std::domain_error);

        affine_t point(one);
        auto inv_point = point.multiplicative_inverse();
        test_affine(inv_point, one, one);

        affine_t interval(one, two);
        auto inv_interval = interval.multiplicative_inverse();
        test_affine(inv_interval, value_t(0.5), value_t(1.0));

        affine_t interval2(-two, two);
        REQUIRE_THROWS_AS(interval2.multiplicative_inverse(),
                          std::domain_error);
    }

    SECTION("operator==") {
        affine_t empty;
        affine_t point(one);
        affine_t interval(one, two);

        REQUIRE(empty == empty);
        REQUIRE(empty == affine_t{});
        REQUIRE_FALSE(empty == point);
        REQUIRE_FALSE(empty == interval);

        REQUIRE(point == point);
        REQUIRE(point == affine_t(one, one));
        REQUIRE_FALSE(point == empty);
        REQUIRE_FALSE(point == interval);

        REQUIRE(interval == interval);
        REQUIRE_FALSE(interval == affine_t(one, two)); // Different error terms
        REQUIRE_FALSE(interval == empty);
        REQUIRE_FALSE(interval == point);

        affine_t interval2(interval.center(), interval.error_terms());
        REQUIRE(interval2 == interval);
    }

    SECTION("operator!=") {
        affine_t empty;
        affine_t point(one);
        affine_t interval(one, two);

        REQUIRE_FALSE(empty != empty);
        REQUIRE_FALSE(empty != affine_t{});
        REQUIRE(empty != point);
        REQUIRE(empty != interval);

        REQUIRE_FALSE(point != point);
        REQUIRE_FALSE(point != affine_t(one, one));
        REQUIRE(point != empty);
        REQUIRE(point != interval);

        REQUIRE_FALSE(interval != interval);
        REQUIRE(interval != affine_t(one, two)); // Different error terms
        REQUIRE(interval != empty);
        REQUIRE(interval != point);

        affine_t interval2(interval.center(), interval.error_terms());
        REQUIRE_FALSE(interval2 != interval);
    }

    SECTION("operator<<") {
        affine_t empty;
        std::stringstream ss_empty;
        ss_empty << empty;
        REQUIRE(ss_empty.str() == "∅");

        affine_t point(one);
        std::stringstream ss_point;
        ss_point << point;
        REQUIRE(ss_point.str() == "1+/-0");

        affine_t interval(one, two);
        std::stringstream ss_interval;
        ss_interval << interval;
        REQUIRE(ss_interval.str() == "1.5+/-0.5");
    }

    SECTION("operator*(value, affine)") {
        affine_t empty;
        auto prod_empty = one * empty;
        REQUIRE(prod_empty.empty());

        affine_t point(one);
        auto prod_point = two * point;
        test_affine(prod_point, two, two);

        affine_t interval(one, two);
        auto prod_interval = two * interval;
        test_affine(prod_interval, two, four);
    }
}
