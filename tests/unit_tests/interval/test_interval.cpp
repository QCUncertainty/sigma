#include "catch2/catch_test_macros.hpp"
#include "testing.hpp"
#include <limits>
#include <sstream>

using testing::test_interval;

TEMPLATE_TEST_CASE("Interval", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    testing_t empty;
    testing_t one_two(1.0, 2.0);
    testing_t one_two_left_open(1.0, 2.0, true, false);
    testing_t one_two_right_open(1.0, 2.0, false, true);
    testing_t one_two_open(1.0, 2.0, true, true);

    SECTION("Constructors") {
        SECTION("Default") {
            REQUIRE(empty.empty());
            REQUIRE(empty.left_open() == false);
            REQUIRE(empty.right_open() == false);
        }

        SECTION("With value") {
            testing_t one(1.0);
            test_interval(one, 1.0, 1.0);
            REQUIRE_FALSE(one.empty());
            REQUIRE(one.left_open() == false);
            REQUIRE(one.right_open() == false);
        }

        SECTION("With Lower and Upper") {
            SECTION("Closed Interval") {
                test_interval(one_two, 1.0, 2.0);
                REQUIRE_FALSE(one_two.empty());
                REQUIRE(one_two.left_open() == false);
                REQUIRE(one_two.right_open() == false);
            }
            SECTION("Left Open Interval") {
                test_interval(one_two_left_open, 1.0, 2.0);
                REQUIRE_FALSE(one_two_left_open.empty());
                REQUIRE(one_two_left_open.left_open() == true);
                REQUIRE(one_two_left_open.right_open() == false);
            }
            SECTION("Right Open Interval") {
                test_interval(one_two_right_open, 1.0, 2.0);
                REQUIRE_FALSE(one_two_right_open.empty());
                REQUIRE(one_two_right_open.left_open() == false);
                REQUIRE(one_two_right_open.right_open() == true);
            }
            SECTION("Open Interval") {
                test_interval(one_two_open, 1.0, 2.0);
                REQUIRE_FALSE(one_two_open.empty());
                REQUIRE(one_two_open.left_open() == true);
                REQUIRE(one_two_open.right_open() == true);
            }
            SECTION("Edge case: open interval with single value") {
                testing_t is_empty(1.0, 1.0, true, true);
                REQUIRE(is_empty == empty);
                REQUIRE(is_empty.empty());
                REQUIRE(is_empty.left_open() == false);
                REQUIRE(is_empty.right_open() == false);
            }
        }
        SECTION("Copy") {
            testing_t copy_empty(empty);
            REQUIRE(copy_empty == empty);
            REQUIRE(copy_empty.empty());

            testing_t copy_one_two(one_two);
            test_interval(copy_one_two, 1.0, 2.0);
            REQUIRE(copy_one_two == one_two);

            testing_t copy_one_two_left_open(one_two_left_open);
            test_interval(copy_one_two_left_open, 1.0, 2.0);
            REQUIRE(copy_one_two_left_open == one_two_left_open);

            testing_t copy_one_two_right_open(one_two_right_open);
            test_interval(copy_one_two_right_open, 1.0, 2.0);
            REQUIRE(copy_one_two_right_open == one_two_right_open);

            testing_t copy_one_two_open(one_two_open);
            test_interval(copy_one_two_open, 1.0, 2.0);
            REQUIRE(copy_one_two_open == one_two_open);
        }
        SECTION("Move") {
            testing_t copy_empty(empty);
            testing_t move_empty(std::move(empty));
            REQUIRE(move_empty == copy_empty);
            REQUIRE(move_empty.empty());

            testing_t copy_one_two(one_two);
            testing_t move_one_two(std::move(one_two));
            test_interval(move_one_two, 1.0, 2.0);
            REQUIRE(move_one_two == copy_one_two);

            testing_t copy_one_two_left_open(one_two_left_open);
            testing_t move_one_two_left_open(std::move(one_two_left_open));
            test_interval(move_one_two_left_open, 1.0, 2.0);
            REQUIRE(move_one_two_left_open == copy_one_two_left_open);

            testing_t copy_one_two_right_open(one_two_right_open);
            testing_t move_one_two_right_open(std::move(one_two_right_open));
            test_interval(move_one_two_right_open, 1.0, 2.0);
            REQUIRE(move_one_two_right_open == copy_one_two_right_open);

            testing_t copy_one_two_open(one_two_open);
            testing_t move_one_two_open(std::move(one_two_open));
            test_interval(move_one_two_open, 1.0, 2.0);
            REQUIRE(move_one_two_open == copy_one_two_open);
        }
        SECTION("Copy Assignment") {
            testing_t copy_empty(empty);
            auto pcopy_empty = &(copy_empty = empty);
            REQUIRE(pcopy_empty == &copy_empty);
            REQUIRE(copy_empty == empty);

            testing_t copy_one_two(one_two);
            auto pcopy_one_two = &(copy_one_two = one_two);
            REQUIRE(pcopy_one_two == &copy_one_two);
            REQUIRE(copy_one_two == one_two);

            testing_t copy_one_two_left_open(one_two_left_open);
            auto pcopy_one_two_left_open =
              &(copy_one_two_left_open = one_two_left_open);
            REQUIRE(pcopy_one_two_left_open == &copy_one_two_left_open);
            REQUIRE(copy_one_two_left_open == one_two_left_open);

            testing_t copy_one_two_right_open(one_two_right_open);
            auto pcopy_one_two_right_open =
              &(copy_one_two_right_open = one_two_right_open);
            REQUIRE(pcopy_one_two_right_open == &copy_one_two_right_open);
            REQUIRE(copy_one_two_right_open == one_two_right_open);

            testing_t copy_one_two_open(one_two_open);
            auto pcopy_one_two_open = &(copy_one_two_open = one_two_open);
            REQUIRE(pcopy_one_two_open == &copy_one_two_open);
            REQUIRE(copy_one_two_open == one_two_open);
        }
        SECTION("Move Assignment") {
            testing_t copy_empty(empty);
            testing_t move_empty;
            auto pmove_empty = &(move_empty = std::move(copy_empty));
            REQUIRE(pmove_empty == &move_empty);
            REQUIRE(move_empty == copy_empty);

            testing_t copy_one_two(one_two);
            testing_t move_one_two;
            auto pmove_one_two = &(move_one_two = std::move(copy_one_two));
            REQUIRE(pmove_one_two == &move_one_two);
            REQUIRE(move_one_two == copy_one_two);

            testing_t copy_one_two_left_open(one_two_left_open);
            testing_t move_one_two_left_open;
            auto pmove_one_two_left_open =
              &(move_one_two_left_open = std::move(copy_one_two_left_open));
            REQUIRE(pmove_one_two_left_open == &move_one_two_left_open);
            REQUIRE(move_one_two_left_open == copy_one_two_left_open);

            testing_t copy_one_two_right_open(one_two_right_open);
            testing_t move_one_two_right_open;
            auto pmove_one_two_right_open =
              &(move_one_two_right_open = std::move(copy_one_two_right_open));
            REQUIRE(pmove_one_two_right_open == &move_one_two_right_open);
            REQUIRE(move_one_two_right_open == copy_one_two_right_open);

            testing_t copy_one_two_open(one_two_open);
            testing_t move_one_two_open;
            auto pmove_one_two_open =
              &(move_one_two_open = std::move(copy_one_two_open));
            REQUIRE(pmove_one_two_open == &move_one_two_open);
            REQUIRE(move_one_two_open == copy_one_two_open);
        }
    }

    SECTION("width") {
        REQUIRE(one_two.width() == 1.0);
        REQUIRE(one_two_left_open.width() == 1.0);
        REQUIRE(one_two_right_open.width() == 1.0);
        REQUIRE(one_two_open.width() == 1.0);

        testing_t one_three(1.0, 3.0);
        REQUIRE(one_three.width() == 2.0);

        testing_t one(1.0, 1.0);
        REQUIRE(one.width() == 0.0);

        testing_t value(-1.2, 0.0);
        REQUIRE(value.width() == value_t(1.2));

        REQUIRE_THROWS_AS(empty.width(), std::domain_error);
    }

    SECTION("median") {
        REQUIRE(one_two.median() == 1.5);
        REQUIRE(one_two_left_open.median() == 1.5);
        REQUIRE(one_two_right_open.median() == 1.5);
        REQUIRE(one_two_open.median() == 1.5);
        REQUIRE_THROWS_AS(empty.median(), std::domain_error);
    }

    SECTION("radius") {
        REQUIRE(one_two.radius() == 0.5);
        REQUIRE(one_two_left_open.radius() == 0.5);
        REQUIRE(one_two_right_open.radius() == 0.5);
        REQUIRE(one_two_open.radius() == 0.5);
        REQUIRE_THROWS_AS(empty.radius(), std::domain_error);
    }

    SECTION("empty") {
        REQUIRE(empty.empty() == true);
        REQUIRE(one_two.empty() == false);
        REQUIRE(one_two_left_open.empty() == false);
        REQUIRE(one_two_right_open.empty() == false);
        REQUIRE(one_two_open.empty() == false);
    }

    SECTION("left_open") {
        REQUIRE(one_two.left_open() == false);
        REQUIRE(one_two_left_open.left_open() == true);
        REQUIRE(one_two_right_open.left_open() == false);
        REQUIRE(one_two_open.left_open() == true);
    }

    SECTION("left_closed") {
        REQUIRE(one_two.left_closed() == true);
        REQUIRE(one_two_left_open.left_closed() == false);
        REQUIRE(one_two_right_open.left_closed() == true);
        REQUIRE(one_two_open.left_closed() == false);
    }

    SECTION("right_open") {
        REQUIRE(one_two.right_open() == false);
        REQUIRE(one_two_left_open.right_open() == false);
        REQUIRE(one_two_right_open.right_open() == true);
        REQUIRE(one_two_open.right_open() == true);
    }

    SECTION("right_closed") {
        REQUIRE(one_two.right_closed() == true);
        REQUIRE(one_two_left_open.right_closed() == true);
        REQUIRE(one_two_right_open.right_closed() == false);
        REQUIRE(one_two_open.right_closed() == false);
    }

    SECTION("lower") {
        REQUIRE(one_two.lower() == 1.0);
        REQUIRE(one_two_left_open.lower() == 1.0);
        REQUIRE(one_two_right_open.lower() == 1.0);
        REQUIRE(one_two_open.lower() == 1.0);
        REQUIRE_THROWS_AS(empty.lower(), std::domain_error);
    }

    SECTION("upper") {
        REQUIRE(one_two.upper() == 2.0);
        REQUIRE(one_two_left_open.upper() == 2.0);
        REQUIRE(one_two_right_open.upper() == 2.0);
        REQUIRE(one_two_open.upper() == 2.0);
        REQUIRE_THROWS_AS(empty.upper(), std::domain_error);
    }

    SECTION("contains (value)") {
        REQUIRE_FALSE(empty.contains(0.0));

        auto one_m_epsilon = std::nextafter(one_two.lower(), value_t(0.0));
        auto one_p_epsilon = std::nextafter(one_two.lower(), value_t(3.0));
        auto two_m_epsilon = std::nextafter(one_two.upper(), value_t(0.0));
        auto two_p_epsilon = std::nextafter(one_two.upper(), value_t(3.0));

        testing_t one(1.0);
        REQUIRE_FALSE(one.contains(one_m_epsilon));
        REQUIRE(one.contains(1.0));
        REQUIRE_FALSE(one.contains(one_p_epsilon));

        REQUIRE_FALSE(one_two.contains(one_m_epsilon));
        REQUIRE(one_two.contains(1.0));
        REQUIRE(one_two.contains(one_p_epsilon));
        REQUIRE(one_two.contains(two_m_epsilon));
        REQUIRE(one_two.contains(2.0));
        REQUIRE_FALSE(one_two.contains(two_p_epsilon));

        REQUIRE_FALSE(one_two_left_open.contains(one_m_epsilon));
        REQUIRE_FALSE(one_two_left_open.contains(1.0));
        REQUIRE(one_two_left_open.contains(one_p_epsilon));
        REQUIRE(one_two_left_open.contains(two_m_epsilon));
        REQUIRE(one_two_left_open.contains(2.0));
        REQUIRE_FALSE(one_two_left_open.contains(two_p_epsilon));

        REQUIRE_FALSE(one_two_right_open.contains(one_m_epsilon));
        REQUIRE(one_two_right_open.contains(1.0));
        REQUIRE(one_two_right_open.contains(one_p_epsilon));
        REQUIRE(one_two_right_open.contains(two_m_epsilon));
        REQUIRE_FALSE(one_two_right_open.contains(2.0));
        REQUIRE_FALSE(one_two_right_open.contains(two_p_epsilon));

        REQUIRE_FALSE(one_two_open.contains(one_m_epsilon));
        REQUIRE_FALSE(one_two_open.contains(1.0));
        REQUIRE(one_two_open.contains(one_p_epsilon));
        REQUIRE(one_two_open.contains(two_m_epsilon));
        REQUIRE_FALSE(one_two_open.contains(2.0));
        REQUIRE_FALSE(one_two_open.contains(two_p_epsilon));
    }

    SECTION("contains (interval)") {
        REQUIRE(empty.contains(testing_t()));
        REQUIRE_FALSE(empty.contains(one_two));

        auto one_m_epsilon = std::nextafter(one_two.lower(), value_t(0.0));
        auto one_p_epsilon = std::nextafter(one_two.lower(), value_t(3.0));
        auto two_m_epsilon = std::nextafter(one_two.upper(), value_t(0.0));
        auto two_p_epsilon = std::nextafter(one_two.upper(), value_t(3.0));

        testing_t outside_left(one_m_epsilon, one_p_epsilon);
        testing_t on_left(value_t(1.0), one_p_epsilon);
        testing_t inside(one_p_epsilon, two_m_epsilon);
        testing_t on_right(two_m_epsilon, value_t(2.0));
        testing_t outside_right(two_m_epsilon, two_p_epsilon);

        REQUIRE(one_two.contains(testing_t()));
        REQUIRE_FALSE(one_two.contains(outside_left));
        REQUIRE(one_two.contains(one_two));
        REQUIRE(one_two.contains(one_two_left_open));
        REQUIRE(one_two.contains(on_left));
        REQUIRE(one_two.contains(inside));
        REQUIRE(one_two.contains(one_two_open));
        REQUIRE(one_two.contains(on_right));
        REQUIRE(one_two.contains(one_two_right_open));
        REQUIRE_FALSE(one_two.contains(outside_right));

        REQUIRE(one_two_left_open.contains(testing_t()));
        REQUIRE_FALSE(one_two_left_open.contains(outside_left));
        REQUIRE_FALSE(one_two_left_open.contains(one_two));
        REQUIRE(one_two_left_open.contains(one_two_left_open));
        REQUIRE_FALSE(one_two_left_open.contains(on_left));
        REQUIRE(one_two_left_open.contains(inside));
        REQUIRE(one_two_left_open.contains(one_two_open));
        REQUIRE(one_two_left_open.contains(on_right));
        REQUIRE_FALSE(one_two_left_open.contains(one_two_right_open));
        REQUIRE_FALSE(one_two_left_open.contains(outside_right));

        REQUIRE(one_two_right_open.contains(testing_t()));
        REQUIRE_FALSE(one_two_right_open.contains(outside_left));
        REQUIRE_FALSE(one_two_right_open.contains(one_two));
        REQUIRE_FALSE(one_two_right_open.contains(one_two_left_open));
        REQUIRE(one_two_right_open.contains(on_left));
        REQUIRE(one_two_right_open.contains(inside));
        REQUIRE(one_two_right_open.contains(one_two_open));
        REQUIRE_FALSE(one_two_right_open.contains(on_right));
        REQUIRE(one_two_right_open.contains(one_two_right_open));
        REQUIRE_FALSE(one_two_right_open.contains(outside_right));

        REQUIRE(one_two_open.contains(testing_t()));
        REQUIRE_FALSE(one_two_open.contains(outside_left));
        REQUIRE_FALSE(one_two_open.contains(one_two));
        REQUIRE_FALSE(one_two_open.contains(one_two_left_open));
        REQUIRE_FALSE(one_two_open.contains(on_left));
        REQUIRE(one_two_open.contains(inside));
        REQUIRE(one_two_open.contains(one_two_open));
        REQUIRE_FALSE(one_two_open.contains(on_right));
        REQUIRE_FALSE(one_two_open.contains(one_two_right_open));
        REQUIRE_FALSE(one_two_open.contains(outside_right));
    }

    SECTION("set_union") {
        SECTION("Union with Empty") {
            REQUIRE(empty.set_union(empty) == empty);
            REQUIRE(empty.set_union(one_two) == one_two);
            REQUIRE(empty.set_union(one_two_left_open) == one_two_left_open);
            REQUIRE(empty.set_union(one_two_right_open) == one_two_right_open);
            REQUIRE(empty.set_union(one_two_open) == one_two_open);
        }
        SECTION("Same interval different openness") {
            REQUIRE(one_two.set_union(empty) == one_two);
            REQUIRE(one_two.set_union(one_two) == one_two);
            REQUIRE(one_two.set_union(one_two_left_open) == one_two);
            REQUIRE(one_two.set_union(one_two_right_open) == one_two);
            REQUIRE(one_two.set_union(one_two_open) == one_two);

            REQUIRE(one_two_left_open.set_union(empty) == one_two_left_open);
            REQUIRE(one_two_left_open.set_union(one_two) == one_two);
            REQUIRE(one_two_left_open.set_union(one_two_left_open) ==
                    one_two_left_open);
            REQUIRE(one_two_left_open.set_union(one_two_right_open) == one_two);
            REQUIRE(one_two_left_open.set_union(one_two_open) ==
                    one_two_left_open);

            REQUIRE(one_two_right_open.set_union(empty) == one_two_right_open);
            REQUIRE(one_two_right_open.set_union(one_two) == one_two);
            REQUIRE(one_two_right_open.set_union(one_two_left_open) == one_two);
            REQUIRE(one_two_right_open.set_union(one_two_right_open) ==
                    one_two_right_open);
            REQUIRE(one_two_right_open.set_union(one_two_open) ==
                    one_two_right_open);

            REQUIRE(one_two_open.set_union(empty) == one_two_open);
            REQUIRE(one_two_open.set_union(one_two) == one_two);
            REQUIRE(one_two_open.set_union(one_two_left_open) ==
                    one_two_left_open);
            REQUIRE(one_two_open.set_union(one_two_right_open) ==
                    one_two_right_open);
            REQUIRE(one_two_open.set_union(one_two_open) == one_two_open);
        }
        SECTION("Expands left bound") {
            // We say "one" because "one_point_five" is too long
            testing_t zero_one(0.0, 1.5);
            testing_t zero_one_left_open(0.0, 1.5, true, false);
            testing_t zero_one_right_open(0.0, 1.5, false, true);
            testing_t zero_one_open(0.0, 1.5, true, true);

            testing_t zero_two(0.0, 2.0);
            testing_t zero_two_left_open(0.0, 2.0, true, false);
            testing_t zero_two_right_open(0.0, 2.0, false, true);
            testing_t zero_two_open(0.0, 2.0, true, true);

            REQUIRE(one_two.set_union(zero_one) == zero_two);
            REQUIRE(one_two.set_union(zero_one_left_open) ==
                    zero_two_left_open);
            REQUIRE(one_two.set_union(zero_one_right_open) == zero_two);
            REQUIRE(one_two.set_union(zero_one_open) == zero_two_left_open);

            REQUIRE(one_two_left_open.set_union(zero_one) == zero_two);
            REQUIRE(one_two_left_open.set_union(zero_one_left_open) ==
                    zero_two_left_open);
            REQUIRE(one_two_left_open.set_union(zero_one_right_open) ==
                    zero_two);
            REQUIRE(one_two_left_open.set_union(zero_one_open) ==
                    zero_two_left_open);

            REQUIRE(one_two_right_open.set_union(zero_one) ==
                    zero_two_right_open);
            REQUIRE(one_two_right_open.set_union(zero_one_left_open) ==
                    zero_two_open);
            REQUIRE(one_two_right_open.set_union(zero_one_right_open) ==
                    zero_two_right_open);
            REQUIRE(one_two_right_open.set_union(zero_one_open) ==
                    zero_two_open);

            REQUIRE(one_two_open.set_union(zero_one) == zero_two_right_open);
            REQUIRE(one_two_open.set_union(zero_one_left_open) ==
                    zero_two_open);
            REQUIRE(one_two_open.set_union(zero_one_right_open) ==
                    zero_two_right_open);
            REQUIRE(one_two_open.set_union(zero_one_open) == zero_two_open);
        }

        SECTION("Expands right bound") {
            testing_t one_three(1.5, 3.0);
            testing_t one_three_left_open(1.5, 3.0, true, false);
            testing_t one_three_right_open(1.5, 3.0, false, true);
            testing_t one_three_open(1.5, 3.0, true, true);

            testing_t one_three2(1.0, 3.0);
            testing_t one_three2_left_open(1.0, 3.0, true, false);
            testing_t one_three2_right_open(1.0, 3.0, false, true);
            testing_t one_three2_open(1.0, 3.0, true, true);

            REQUIRE(one_two.set_union(one_three) == one_three2);
            REQUIRE(one_two.set_union(one_three_left_open) == one_three2);
            REQUIRE(one_two.set_union(one_three_right_open) ==
                    one_three2_right_open);
            REQUIRE(one_two.set_union(one_three_open) == one_three2_right_open);

            REQUIRE(one_two_left_open.set_union(one_three) ==
                    one_three2_left_open);
            REQUIRE(one_two_left_open.set_union(one_three_left_open) ==
                    one_three2_left_open);
            REQUIRE(one_two_left_open.set_union(one_three_right_open) ==
                    one_three2_open);
            REQUIRE(one_two_left_open.set_union(one_three_open) ==
                    one_three2_open);

            REQUIRE(one_two_right_open.set_union(one_three) == one_three2);
            REQUIRE(one_two_right_open.set_union(one_three_left_open) ==
                    one_three2);
            REQUIRE(one_two_right_open.set_union(one_three_right_open) ==
                    one_three2_right_open);
            REQUIRE(one_two_right_open.set_union(one_three_open) ==
                    one_three2_right_open);

            REQUIRE(one_two_open.set_union(one_three) == one_three2_left_open);
            REQUIRE(one_two_open.set_union(one_three_left_open) ==
                    one_three2_left_open);
            REQUIRE(one_two_open.set_union(one_three_right_open) ==
                    one_three2_open);
            REQUIRE(one_two_open.set_union(one_three_open) == one_three2_open);
        }

        testing_t disjoint(4.0, 5.0);
        REQUIRE_THROWS_AS(one_two.set_union(disjoint), std::domain_error);
    }

    SECTION("set_intersection") {
        SECTION("Intersection with Empty") {
            REQUIRE(empty.set_intersection(empty) == empty);
            REQUIRE(empty.set_intersection(one_two) == empty);
            REQUIRE(empty.set_intersection(one_two_left_open) == empty);
            REQUIRE(empty.set_intersection(one_two_right_open) == empty);
            REQUIRE(empty.set_intersection(one_two_open) == empty);
        }
        SECTION("Same interval different openness") {
            REQUIRE(one_two.set_intersection(empty) == empty);
            REQUIRE(one_two.set_intersection(one_two) == one_two);
            REQUIRE(one_two.set_intersection(one_two_left_open) ==
                    one_two_left_open);
            REQUIRE(one_two.set_intersection(one_two_right_open) ==
                    one_two_right_open);
            REQUIRE(one_two.set_intersection(one_two_open) == one_two_open);

            REQUIRE(one_two_left_open.set_intersection(empty) == empty);
            REQUIRE(one_two_left_open.set_intersection(one_two) ==
                    one_two_left_open);
            REQUIRE(one_two_left_open.set_intersection(one_two_left_open) ==
                    one_two_left_open);
            REQUIRE(one_two_left_open.set_intersection(one_two_right_open) ==
                    one_two_open);
            REQUIRE(one_two_left_open.set_intersection(one_two_open) ==
                    one_two_open);

            REQUIRE(one_two_right_open.set_intersection(empty) == empty);
            REQUIRE(one_two_right_open.set_intersection(one_two) ==
                    one_two_right_open);
            REQUIRE(one_two_right_open.set_intersection(one_two_left_open) ==
                    one_two_open);
            REQUIRE(one_two_right_open.set_intersection(one_two_right_open) ==
                    one_two_right_open);
            REQUIRE(one_two_right_open.set_intersection(one_two_open) ==
                    one_two_open);

            REQUIRE(one_two_open.set_intersection(empty) == empty);
            REQUIRE(one_two_open.set_intersection(one_two) == one_two_open);
            REQUIRE(one_two_open.set_intersection(one_two_left_open) ==
                    one_two_open);
            REQUIRE(one_two_open.set_intersection(one_two_right_open) ==
                    one_two_open);
            REQUIRE(one_two_open.set_intersection(one_two_open) ==
                    one_two_open);
        }
        SECTION("Expands left bound") {
            // We say "one" because "one_point_five" is too long
            testing_t zero_one(0.0, 1.5);
            testing_t zero_one_left_open(0.0, 1.5, true, false);
            testing_t zero_one_right_open(0.0, 1.5, false, true);
            testing_t zero_one_open(0.0, 1.5, true, true);

            testing_t one(1.0, 1.5);
            testing_t one_left_open(1.0, 1.5, true, false);
            testing_t one_right_open(1.0, 1.5, false, true);
            testing_t one_open(1.0, 1.5, true, true);

            REQUIRE(one_two.set_intersection(zero_one) == one);
            REQUIRE(one_two.set_intersection(zero_one_left_open) == one);
            REQUIRE(one_two.set_intersection(zero_one_right_open) ==
                    one_right_open);
            REQUIRE(one_two.set_intersection(zero_one_open) == one_right_open);

            REQUIRE(one_two_left_open.set_intersection(zero_one) ==
                    one_left_open);
            REQUIRE(one_two_left_open.set_intersection(zero_one_left_open) ==
                    one_left_open);
            REQUIRE(one_two_left_open.set_intersection(zero_one_right_open) ==
                    one_open);
            REQUIRE(one_two_left_open.set_intersection(zero_one_open) ==
                    one_open);

            REQUIRE(one_two_right_open.set_intersection(zero_one) == one);
            REQUIRE(one_two_right_open.set_intersection(zero_one_left_open) ==
                    one);
            REQUIRE(one_two_right_open.set_intersection(zero_one_right_open) ==
                    one_right_open);
            REQUIRE(one_two_right_open.set_intersection(zero_one_open) ==
                    one_right_open);

            REQUIRE(one_two_open.set_intersection(zero_one) == one_left_open);
            REQUIRE(one_two_open.set_intersection(zero_one_left_open) ==
                    one_left_open);
            REQUIRE(one_two_open.set_intersection(zero_one_right_open) ==
                    one_open);
            REQUIRE(one_two_open.set_intersection(zero_one_open) == one_open);
        }

        SECTION("Expands right bound") {
            testing_t one_three(1.5, 3.0);
            testing_t one_three_left_open(1.5, 3.0, true, false);
            testing_t one_three_right_open(1.5, 3.0, false, true);
            testing_t one_three_open(1.5, 3.0, true, true);

            testing_t one_two2(1.5, 2.0);
            testing_t one_two2_left_open(1.5, 2.0, true, false);
            testing_t one_two2_right_open(1.5, 2.0, false, true);
            testing_t one_two2_open(1.5, 2.0, true, true);

            REQUIRE(one_two.set_intersection(one_three) == one_two2);
            REQUIRE(one_two.set_intersection(one_three_left_open) ==
                    one_two2_left_open);
            REQUIRE(one_two.set_intersection(one_three_right_open) == one_two2);
            REQUIRE(one_two.set_intersection(one_three_open) ==
                    one_two2_left_open);

            REQUIRE(one_two_left_open.set_intersection(one_three) == one_two2);
            REQUIRE(one_two_left_open.set_intersection(one_three_left_open) ==
                    one_two2_left_open);
            REQUIRE(one_two_left_open.set_intersection(one_three_right_open) ==
                    one_two2);
            REQUIRE(one_two_left_open.set_intersection(one_three_open) ==
                    one_two2_left_open);

            REQUIRE(one_two_right_open.set_intersection(one_three) ==
                    one_two2_right_open);
            REQUIRE(one_two_right_open.set_intersection(one_three_left_open) ==
                    one_two2_open);
            REQUIRE(one_two_right_open.set_intersection(one_three_right_open) ==
                    one_two2_right_open);
            REQUIRE(one_two_right_open.set_intersection(one_three_open) ==
                    one_two2_open);

            REQUIRE(one_two_open.set_intersection(one_three) ==
                    one_two2_right_open);
            REQUIRE(one_two_open.set_intersection(one_three_left_open) ==
                    one_two2_open);
            REQUIRE(one_two_open.set_intersection(one_three_right_open) ==
                    one_two2_right_open);
            REQUIRE(one_two_open.set_intersection(one_three_open) ==
                    one_two2_open);
        }

        testing_t disjoint(4.0, 5.0);
        REQUIRE(one_two.set_intersection(disjoint) == testing_t());
    }

    SECTION("operator-(void)") {
        REQUIRE(-empty == empty);
        REQUIRE(-one_two == testing_t(-2.0, -1.0));
        REQUIRE(-one_two_left_open == testing_t(-2.0, -1.0, false, true));
        REQUIRE(-one_two_right_open == testing_t(-2.0, -1.0, true, false));
        REQUIRE(-one_two_open == testing_t(-2.0, -1.0, true, true));
    }

    SECTION("operator+=") {
        // We just spot check here and rely on operator+ for exhaustive testing
        auto pone_two = &(one_two += one_two);
        REQUIRE(pone_two == &one_two);
        REQUIRE(one_two == testing_t(2.0, 4.0));

        auto pempty = &(empty += value_t(2.0));
        REQUIRE(pempty == &empty);
        REQUIRE(empty == testing_t(2.0, 2.0));
    }

    SECTION("operator+") {
        SECTION("With Interval") {
            SECTION("Empty") {
                REQUIRE(empty + one_two == one_two);
                REQUIRE(one_two + empty == one_two);
                REQUIRE(empty + one_two_left_open == one_two_left_open);
                REQUIRE(one_two_left_open + empty == one_two_left_open);
                REQUIRE(empty + one_two_right_open == one_two_right_open);
                REQUIRE(one_two_right_open + empty == one_two_right_open);
                REQUIRE(empty + one_two_open == one_two_open);
                REQUIRE(one_two_open + empty == one_two_open);
            }
            SECTION("Non-empty") {
                testing_t three_four(3.0, 4.0);
                testing_t three_four_left_open(3.0, 4.0, true, false);
                testing_t three_four_right_open(3.0, 4.0, false, true);
                testing_t three_four_open(3.0, 4.0, true, true);

                testing_t four_six(4.0, 6.0);
                testing_t four_six_left_open(4.0, 6.0, true, false);
                testing_t four_six_right_open(4.0, 6.0, false, true);
                testing_t four_six_open(4.0, 6.0, true, true);

                REQUIRE(one_two + three_four == four_six);
                REQUIRE(one_two + three_four_left_open == four_six_left_open);
                REQUIRE(one_two + three_four_right_open == four_six_right_open);
                REQUIRE(one_two + three_four_open == four_six_open);

                REQUIRE(one_two_left_open + three_four == four_six_left_open);
                REQUIRE(one_two_left_open + three_four_left_open ==
                        four_six_left_open);
                REQUIRE(one_two_left_open + three_four_right_open ==
                        four_six_open);
                REQUIRE(one_two_left_open + three_four_open == four_six_open);

                REQUIRE(one_two_right_open + three_four == four_six_right_open);
                REQUIRE(one_two_right_open + three_four_left_open ==
                        four_six_open);
                REQUIRE(one_two_right_open + three_four_right_open ==
                        four_six_right_open);
                REQUIRE(one_two_right_open + three_four_open == four_six_open);

                REQUIRE(one_two_open + three_four == four_six_open);
                REQUIRE(one_two_open + three_four_left_open == four_six_open);
                REQUIRE(one_two_open + three_four_right_open == four_six_open);
                REQUIRE(one_two_open + three_four_open == four_six_open);
            }
        }
        SECTION("With Scalar") {
            value_t two(2.0);

            testing_t two_two(two, two);
            testing_t three_four(3.0, 4.0);
            testing_t three_four_left_open(3.0, 4.0, true, false);
            testing_t three_four_right_open(3.0, 4.0, false, true);
            testing_t three_four_open(3.0, 4.0, true, true);

            REQUIRE(empty + two == two_two);
            REQUIRE(one_two + two == three_four);
            REQUIRE(one_two_left_open + two == three_four_left_open);
            REQUIRE(one_two_right_open + two == three_four_right_open);
            REQUIRE(one_two_open + two == three_four_open);
        }
    }

    SECTION("operator-=") {
        // We just spot check here and rely on operator- for exhaustive testing
        testing_t copy_one_two(one_two);
        auto pone_two = &(one_two -= copy_one_two);
        REQUIRE(pone_two == &one_two);
        REQUIRE(one_two == testing_t(-1.0, 1.0));

        auto pempty = &(empty -= value_t(2.0));
        REQUIRE(pempty == &empty);
        REQUIRE(empty == testing_t(-2.0, -2.0));
    }

    SECTION("operator-") {
        SECTION("With Interval") {
            SECTION("Empty") {
                REQUIRE(empty - one_two == -one_two);
                REQUIRE(one_two - empty == one_two);
                REQUIRE(empty - one_two_left_open == -one_two_left_open);
                REQUIRE(one_two_left_open - empty == one_two_left_open);
                REQUIRE(empty - one_two_right_open == -one_two_right_open);
                REQUIRE(one_two_right_open - empty == one_two_right_open);
                REQUIRE(empty - one_two_open == -one_two_open);
                REQUIRE(one_two_open - empty == one_two_open);
            }
            SECTION("Non-empty") {
                testing_t three_four(3.0, 4.0);
                testing_t three_four_left_open(3.0, 4.0, true, false);
                testing_t three_four_right_open(3.0, 4.0, false, true);
                testing_t three_four_open(3.0, 4.0, true, true);

                testing_t three_one(-3.0, -1.0);
                testing_t three_one_left_open(-3.0, -1.0, true, false);
                testing_t three_one_right_open(-3.0, -1.0, false, true);
                testing_t three_one_open(-3.0, -1.0, true, true);

                REQUIRE(one_two - three_four == three_one);
                REQUIRE(one_two - three_four_left_open == three_one_right_open);
                REQUIRE(one_two - three_four_right_open == three_one_left_open);
                REQUIRE(one_two - three_four_open == three_one_open);

                REQUIRE(one_two_left_open - three_four == three_one_left_open);
                REQUIRE(one_two_left_open - three_four_left_open ==
                        three_one_open);
                REQUIRE(one_two_left_open - three_four_right_open ==
                        three_one_left_open);
                REQUIRE(one_two_left_open - three_four_open == three_one_open);

                REQUIRE(one_two_right_open - three_four ==
                        three_one_right_open);
                REQUIRE(one_two_right_open - three_four_left_open ==
                        three_one_right_open);
                REQUIRE(one_two_right_open - three_four_right_open ==
                        three_one_open);
                REQUIRE(one_two_right_open - three_four_open == three_one_open);

                REQUIRE(one_two_open - three_four == three_one_open);
                REQUIRE(one_two_open - three_four_left_open == three_one_open);
                REQUIRE(one_two_open - three_four_right_open == three_one_open);
                REQUIRE(one_two_open - three_four_open == three_one_open);
            }
        }
        SECTION("With Scalar") {
            value_t two(2.0);

            testing_t two_two(two, two);
            testing_t one_zero(-1.0, 0.0);
            testing_t one_zero_left_open(-1.0, 0.0, true, false);
            testing_t one_zero_right_open(-1.0, 0.0, false, true);
            testing_t one_zero_open(-1.0, 0.0, true, true);

            REQUIRE(empty - two == -two_two);
            REQUIRE(two - empty == two_two);
            REQUIRE(one_two - two == one_zero);
            REQUIRE(two - one_two == -one_zero);
            REQUIRE(one_two_left_open - two == one_zero_left_open);
            REQUIRE(two - one_two_left_open == -one_zero_left_open);
            REQUIRE(one_two_right_open - two == one_zero_right_open);
            REQUIRE(two - one_two_right_open == -one_zero_right_open);
            REQUIRE(one_two_open - two == one_zero_open);
            REQUIRE(two - one_two_open == -one_zero_open);
        }
    }

    SECTION("operator*=") {
        // Spot check here and rely on operator* for exhaustive testing
        auto pone_two = &(one_two *= one_two);
        REQUIRE(pone_two == &one_two);
        REQUIRE(one_two == testing_t(1.0, 4.0));

        auto pempty = &(empty *= value_t(2.0));
        REQUIRE(pempty == &empty);
        REQUIRE(empty == testing_t());
    }

    SECTION("operator*") {
        SECTION("With Interval") {
            SECTION("Empty") {
                REQUIRE(empty * one_two == empty);
                REQUIRE(one_two * empty == empty);
                REQUIRE(empty * one_two_left_open == empty);
                REQUIRE(one_two_left_open * empty == empty);
                REQUIRE(empty * one_two_right_open == empty);
                REQUIRE(one_two_right_open * empty == empty);
                REQUIRE(empty * one_two_open == empty);
                REQUIRE(one_two_open * empty == empty);
            }
            SECTION("Non-empty") {
                SECTION("result == [ll, hh]") {
                    testing_t rhs(3.0, 4.0);
                    testing_t rhs_left_open(3.0, 4.0, true, false);
                    testing_t rhs_right_open(3.0, 4.0, false, true);
                    testing_t rhs_open(3.0, 4.0, true, true);

                    testing_t result(3.0, 8.0);
                    testing_t result_left_open(3.0, 8.0, true, false);
                    testing_t result_right_open(3.0, 8.0, false, true);
                    testing_t result_open(3.0, 8.0, true, true);

                    REQUIRE(one_two * rhs == result);
                    REQUIRE(one_two * rhs_left_open == result_left_open);
                    REQUIRE(one_two * rhs_right_open == result_right_open);
                    REQUIRE(one_two * rhs_open == result_open);

                    REQUIRE(one_two_left_open * rhs == result_left_open);
                    REQUIRE(one_two_left_open * rhs_left_open ==
                            result_left_open);
                    REQUIRE(one_two_left_open * rhs_right_open == result_open);
                    REQUIRE(one_two_left_open * rhs_open == result_open);

                    REQUIRE(one_two_right_open * rhs == result_right_open);
                    REQUIRE(one_two_right_open * rhs_left_open == result_open);
                    REQUIRE(one_two_right_open * rhs_right_open ==
                            result_right_open);
                    REQUIRE(one_two_right_open * rhs_open == result_open);

                    REQUIRE(one_two_open * rhs == result_open);
                    REQUIRE(one_two_open * rhs_left_open == result_open);
                    REQUIRE(one_two_open * rhs_right_open == result_open);
                    REQUIRE(one_two_open * rhs_open == result_open);
                }
                SECTION("result == [lh, ll]") {
                    testing_t rhs(-4.0, 3.0);
                    testing_t rhs_left_open(-4.0, 3.0, true, false);
                    testing_t rhs_right_open(-4.0, 3.0, false, true);
                    testing_t rhs_open(-4.0, 3.0, true, true);
                    testing_t lhs(-3.0, 2.0);
                    testing_t lhs_left_open(-3.0, 2.0, true, false);
                    testing_t lhs_right_open(-3.0, 2.0, false, true);
                    testing_t lhs_open(-3.0, 2.0, true, true);

                    testing_t result(-9.0, 12.0);
                    testing_t result_left_open(-9.0, 12.0, true, false);
                    testing_t result_right_open(-9.0, 12.0, false, true);
                    testing_t result_open(-9.0, 12.0, true, true);

                    REQUIRE(lhs * rhs == result);
                    REQUIRE(lhs * rhs_left_open == result_right_open);
                    REQUIRE(lhs * rhs_right_open == result_left_open);
                    REQUIRE(lhs * rhs_open == result_open);

                    REQUIRE(lhs_left_open * rhs == result_open);
                    REQUIRE(lhs_left_open * rhs_left_open == result_open);
                    REQUIRE(lhs_left_open * rhs_right_open == result_open);
                    REQUIRE(lhs_left_open * rhs_open == result_open);

                    REQUIRE(lhs_right_open * rhs == result);
                    REQUIRE(lhs_right_open * rhs_left_open ==
                            result_right_open);
                    REQUIRE(lhs_right_open * rhs_right_open ==
                            result_left_open);
                    REQUIRE(lhs_right_open * rhs_open == result_open);

                    REQUIRE(lhs_open * rhs == result_open);
                    REQUIRE(lhs_open * rhs_left_open == result_open);
                    REQUIRE(lhs_open * rhs_right_open == result_open);
                    REQUIRE(lhs_open * rhs_open == result_open);
                }
                SECTION("result == [lh, hl]") {
                    testing_t lhs(-4.0, -3.0);
                    testing_t lhs_left_open(-4.0, -3.0, true, false);
                    testing_t lhs_right_open(-4.0, -3.0, false, true);
                    testing_t lhs_open(-4.0, -3.0, true, true);

                    testing_t result(-8.0, -3.0);
                    testing_t result_left_open(-8.0, -3.0, true, false);
                    testing_t result_right_open(-8.0, -3.0, false, true);
                    testing_t result_open(-8.0, -3.0, true, true);

                    REQUIRE(lhs * one_two == result);
                    REQUIRE(lhs * one_two_left_open == result_right_open);
                    REQUIRE(lhs * one_two_right_open == result_left_open);
                    REQUIRE(lhs * one_two_open == result_open);

                    REQUIRE(lhs_left_open * one_two == result_left_open);
                    REQUIRE(lhs_left_open * one_two_left_open == result_open);
                    REQUIRE(lhs_left_open * one_two_right_open ==
                            result_left_open);
                    REQUIRE(lhs_left_open * one_two_open == result_open);

                    REQUIRE(lhs_right_open * one_two == result_right_open);
                    REQUIRE(lhs_right_open * one_two_left_open ==
                            result_right_open);
                    REQUIRE(lhs_right_open * one_two_right_open == result_open);
                    REQUIRE(lhs_right_open * one_two_open == result_open);

                    REQUIRE(lhs_open * one_two == result_open);
                    REQUIRE(lhs_open * one_two_left_open == result_open);
                    REQUIRE(lhs_open * one_two_right_open == result_open);
                    REQUIRE(lhs_open * one_two_open == result_open);
                }
                SECTION("result == [lh, hh]") {
                    testing_t lhs(-3.0, 4.0);
                    testing_t lhs_left_open(-3.0, 4.0, true, false);
                    testing_t lhs_right_open(-3.0, 4.0, false, true);
                    testing_t lhs_open(-3.0, 4.0, true, true);

                    testing_t result(-6.0, 8.0);
                    testing_t result_left_open(-6.0, 8.0, true, false);
                    testing_t result_right_open(-6.0, 8.0, false, true);
                    testing_t result_open(-6.0, 8.0, true, true);

                    REQUIRE(lhs * one_two == result);
                    REQUIRE(lhs * one_two_left_open == result);
                    REQUIRE(lhs * one_two_right_open == result_open);
                    REQUIRE(lhs * one_two_open == result_open);

                    REQUIRE(lhs_left_open * one_two == result_left_open);
                    REQUIRE(lhs_left_open * one_two_left_open ==
                            result_left_open);
                    REQUIRE(lhs_left_open * one_two_right_open == result_open);
                    REQUIRE(lhs_left_open * one_two_open == result_open);

                    REQUIRE(lhs_right_open * one_two == result_right_open);
                    REQUIRE(lhs_right_open * one_two_left_open ==
                            result_right_open);
                    REQUIRE(lhs_right_open * one_two_right_open == result_open);
                    REQUIRE(lhs_right_open * one_two_open == result_open);

                    REQUIRE(lhs_open * one_two == result_open);
                    REQUIRE(lhs_open * one_two_left_open == result_open);
                    REQUIRE(lhs_open * one_two_right_open == result_open);
                    REQUIRE(lhs_open * one_two_open == result_open);
                }
                SECTION("result == [hl, ll]") {
                    testing_t lhs(-4.0, 3.0);
                    testing_t lhs_left_open(-4.0, 3.0, true, false);
                    testing_t lhs_right_open(-4.0, 3.0, false, true);
                    testing_t lhs_open(-4.0, 3.0, true, true);
                    testing_t rhs(-3.0, 2.0);
                    testing_t rhs_left_open(-3.0, 2.0, true, false);
                    testing_t rhs_right_open(-3.0, 2.0, false, true);
                    testing_t rhs_open(-3.0, 2.0, true, true);

                    testing_t result(-9.0, 12.0);
                    testing_t result_left_open(-9.0, 12.0, true, false);
                    testing_t result_right_open(-9.0, 12.0, false, true);
                    testing_t result_open(-9.0, 12.0, true, true);

                    REQUIRE(lhs * rhs == result);
                    REQUIRE(lhs * rhs_left_open == result_open);
                    REQUIRE(lhs * rhs_right_open == result);
                    REQUIRE(lhs * rhs_open == result_open);

                    REQUIRE(lhs_left_open * rhs == result_right_open);
                    REQUIRE(lhs_left_open * rhs_left_open == result_open);
                    REQUIRE(lhs_left_open * rhs_right_open ==
                            result_right_open);
                    REQUIRE(lhs_left_open * rhs_open == result_open);

                    REQUIRE(lhs_right_open * rhs == result_left_open);
                    REQUIRE(lhs_right_open * rhs_left_open == result_open);
                    REQUIRE(lhs_right_open * rhs_right_open ==
                            result_left_open);
                    REQUIRE(lhs_right_open * rhs_open == result_open);

                    REQUIRE(lhs_open * rhs == result_open);
                    REQUIRE(lhs_open * rhs_left_open == result_open);
                    REQUIRE(lhs_open * rhs_right_open == result_open);
                    REQUIRE(lhs_open * rhs_open == result_open);
                }
                SECTION("result == [hl, lh]") {
                    testing_t rhs(-4.0, -3.0);
                    testing_t rhs_left_open(-4.0, -3.0, true, false);
                    testing_t rhs_right_open(-4.0, -3.0, false, true);
                    testing_t rhs_open(-4.0, -3.0, true, true);

                    testing_t result(-8.0, -3.0);
                    testing_t result_left_open(-8.0, -3.0, true, false);
                    testing_t result_right_open(-8.0, -3.0, false, true);
                    testing_t result_open(-8.0, -3.0, true, true);

                    REQUIRE(one_two * rhs == result);
                    REQUIRE(one_two * rhs_left_open == result_left_open);
                    REQUIRE(one_two * rhs_right_open == result_right_open);
                    REQUIRE(one_two * rhs_open == result_open);

                    REQUIRE(one_two_left_open * rhs == result_right_open);
                    REQUIRE(one_two_left_open * rhs_left_open == result_open);
                    REQUIRE(one_two_left_open * rhs_right_open ==
                            result_right_open);
                    REQUIRE(one_two_left_open * rhs_open == result_open);

                    REQUIRE(one_two_right_open * rhs == result_left_open);
                    REQUIRE(one_two_right_open * rhs_left_open ==
                            result_left_open);
                    REQUIRE(one_two_right_open * rhs_right_open == result_open);
                    REQUIRE(one_two_right_open * rhs_open == result_open);

                    REQUIRE(one_two_open * rhs == result_open);
                    REQUIRE(one_two_open * rhs_left_open == result_open);
                    REQUIRE(one_two_open * rhs_right_open == result_open);
                    REQUIRE(one_two_open * rhs_open == result_open);
                }
                SECTION("result == [hl, hh]") {
                    testing_t rhs(-3.0, 4.0);
                    testing_t rhs_left_open(-3.0, 4.0, true, false);
                    testing_t rhs_right_open(-3.0, 4.0, false, true);
                    testing_t rhs_open(-3.0, 4.0, true, true);

                    testing_t result(-6.0, 8.0);
                    testing_t result_left_open(-6.0, 8.0, true, false);
                    testing_t result_right_open(-6.0, 8.0, false, true);
                    testing_t result_open(-6.0, 8.0, true, true);

                    REQUIRE(one_two * rhs == result);
                    REQUIRE(one_two * rhs_left_open == result_left_open);
                    REQUIRE(one_two * rhs_right_open == result_right_open);
                    REQUIRE(one_two * rhs_open == result_open);

                    REQUIRE(one_two_left_open * rhs == result);
                    REQUIRE(one_two_left_open * rhs_left_open ==
                            result_left_open);
                    REQUIRE(one_two_left_open * rhs_right_open ==
                            result_right_open);
                    REQUIRE(one_two_left_open * rhs_open == result_open);

                    REQUIRE(one_two_right_open * rhs == result_open);
                    REQUIRE(one_two_right_open * rhs_left_open == result_open);
                    REQUIRE(one_two_right_open * rhs_right_open == result_open);
                    REQUIRE(one_two_right_open * rhs_open == result_open);

                    REQUIRE(one_two_open * rhs == result_open);
                    REQUIRE(one_two_open * rhs_left_open == result_open);
                    REQUIRE(one_two_open * rhs_right_open == result_open);
                    REQUIRE(one_two_open * rhs_open == result_open);
                }
                SECTION("result == [hh,ll]") {
                    testing_t lhs(-4.0, -3.0);
                    testing_t lhs_left_open(-4.0, -3.0, true, false);
                    testing_t lhs_right_open(-4.0, -3.0, false, true);
                    testing_t lhs_open(-4.0, -3.0, true, true);
                    testing_t rhs(-2.0, -1.0);
                    testing_t rhs_left_open(-2.0, -1.0, true, false);
                    testing_t rhs_right_open(-2.0, -1.0, false, true);
                    testing_t rhs_open(-2.0, -1.0, true, true);

                    testing_t result(3.0, 8.0);
                    testing_t result_left_open(3.0, 8.0, true, false);
                    testing_t result_right_open(3.0, 8.0, false, true);
                    testing_t result_open(3.0, 8.0, true, true);

                    REQUIRE(lhs * rhs == result);
                    REQUIRE(lhs * rhs_left_open == result_right_open);
                    REQUIRE(lhs * rhs_right_open == result_left_open);
                    REQUIRE(lhs * rhs_open == result_open);

                    REQUIRE(lhs_left_open * rhs == result_right_open);
                    REQUIRE(lhs_left_open * rhs_left_open == result_right_open);
                    REQUIRE(lhs_left_open * rhs_right_open == result_open);
                    REQUIRE(lhs_left_open * rhs_open == result_open);

                    REQUIRE(lhs_right_open * rhs == result_left_open);
                    REQUIRE(lhs_right_open * rhs_left_open == result_open);
                    REQUIRE(lhs_right_open * rhs_right_open ==
                            result_left_open);
                    REQUIRE(lhs_right_open * rhs_open == result_open);

                    REQUIRE(lhs_open * rhs == result_open);
                    REQUIRE(lhs_open * rhs_left_open == result_open);
                    REQUIRE(lhs_open * rhs_right_open == result_open);
                    REQUIRE(lhs_open * rhs_open == result_open);
                }
            }
            SECTION("value") {
                value_t two(2.0);

                testing_t result(2.0, 4.0);
                testing_t result_left_open(2.0, 4.0, true, false);
                testing_t result_right_open(2.0, 4.0, false, true);
                testing_t result_open(2.0, 4.0, true, true);

                REQUIRE(one_two * two == result);
                REQUIRE(two * one_two == result);
                REQUIRE(one_two_left_open * two == result_left_open);
                REQUIRE(two * one_two_left_open == result_left_open);
                REQUIRE(one_two_right_open * two == result_right_open);
                REQUIRE(two * one_two_right_open == result_right_open);
                REQUIRE(one_two_open * two == result_open);
                REQUIRE(two * one_two_open == result_open);
            }
        }
    }

    SECTION("operator/=") {
        // Spot check here and rely on operator/ for exhaustive testing
        testing_t copy_one_two(one_two);
        auto pone_two = &(one_two /= copy_one_two);
        REQUIRE(pone_two == &one_two);
        REQUIRE(one_two == testing_t(value_t(1.0 / 2.0), value_t(2.0)));

        auto pempty = &(empty /= value_t(2.0));
        REQUIRE(pempty == &empty);
        REQUIRE(empty == testing_t());
    }

    SECTION("operator/") {
        SECTION("With Interval") {
            SECTION("Empty") {
                REQUIRE(empty / one_two == empty);
                REQUIRE(one_two / empty == empty);
                REQUIRE(empty / one_two_left_open == empty);
                REQUIRE(one_two_left_open / empty == empty);
                REQUIRE(empty / one_two_right_open == empty);
            }
            SECTION("Non-empty") {
                // The implementation consists of calling
                // 1. 1.0 / rhs.
                // 2. multiplying lhs by the result of 1.
                // Step 1 is tested in the "With Value" section below and works.
                // Step 2 is tested in the operator* section above and works.
                // We just spot check here.

                testing_t result(0.5, 2.0);
                testing_t result_left_open(0.5, 2.0, true, false);
                testing_t result_right_open(0.5, 2.0, false, true);
                testing_t result_open(0.5, 2.0, true, true);

                REQUIRE(one_two / one_two == result);

                REQUIRE(one_two_left_open / one_two == result_left_open);
                REQUIRE(one_two / one_two_left_open == result_right_open);

                REQUIRE(one_two_right_open / one_two == result_right_open);
                REQUIRE(one_two / one_two_right_open == result_left_open);

                REQUIRE(one_two_open / one_two == result_open);
                REQUIRE(one_two / one_two_right_open == result_left_open);

                REQUIRE_THROWS_AS(one_two / value_t(0.0), std::domain_error);
                REQUIRE_THROWS_AS(one_two / testing_t(-1.0, 1.0),
                                  std::domain_error);
            }
        }
        SECTION("With Value") {
            value_t two(2.0);

            // Result when two is on left side
            testing_t result_lhs(1.0, 2.0);
            testing_t result_lhs_left_open(1.0, 2.0, true, false);
            testing_t result_lhs_right_open(1.0, 2.0, false, true);
            testing_t result_lhs_open(1.0, 2.0, true, true);

            // Result when two is on right side
            testing_t result_rhs(0.5, 1.0);
            testing_t result_rhs_left_open(0.5, 1.0, true, false);
            testing_t result_rhs_right_open(0.5, 1.0, false, true);
            testing_t result_rhs_open(0.5, 1.0, true, true);
            testing_t result_open(1.0, 2.0, true, true);

            REQUIRE(empty / two == empty);
            REQUIRE(two / empty == empty);

            REQUIRE(one_two / two == result_rhs);
            REQUIRE(two / one_two == result_lhs);

            REQUIRE(one_two_left_open / two == result_rhs_left_open);
            REQUIRE(two / one_two_left_open == result_lhs_right_open);

            REQUIRE(one_two_right_open / two == result_rhs_right_open);
            REQUIRE(two / one_two_right_open == result_lhs_left_open);

            REQUIRE(one_two_open / two == result_rhs_open);
            REQUIRE(two / one_two_open == result_lhs_open);

            REQUIRE_THROWS_AS(one_two / value_t(0.0), std::domain_error);
            REQUIRE_THROWS_AS(two / testing_t(-1.0, 1.0), std::domain_error);
        }
    }

    SECTION("Outward Rounding") {
        // A product or a quotient of representable values is usually not
        // itself representable, so the bounds have to round outward. Computing
        // them in the ambient round-to-nearest mode instead gives a result
        // that can exclude the true value, and for point operands claims the
        // operation is exact.
        auto two   = testing_t(value_t{2});
        auto three = testing_t(value_t{3});

        auto quotient = two / three;
        REQUIRE(quotient.width() > value_t{0});
        REQUIRE(quotient.contains(value_t{2} / value_t{3}));

        auto product = testing_t(value_t{0.1}) * testing_t(value_t{0.3});
        REQUIRE(product.width() > value_t{0});
        REQUIRE(product.contains(value_t{0.1} * value_t{0.3}));

        // Addition and subtraction have always gone through boost, and are
        // covered here so the whole of the arithmetic is pinned in one place.
        auto sum = testing_t(value_t{0.1}) + testing_t(value_t{0.2});
        REQUIRE(sum.width() > value_t{0});
        REQUIRE(sum.contains(value_t{0.1} + value_t{0.2}));

        // An exact result stays exact -- the bounds are directed, not padded
        REQUIRE((two * three).width() == value_t{0});
        REQUIRE((two * three) == testing_t(value_t{6}));
    }

    SECTION("Division by an interval bounded by zero") {
        // Zero is not in (0, 1], so [1, 2] / (0, 1] is defined even though
        // boost, which does not track openness, would report the whole line.
        testing_t open_at_zero(value_t{0}, value_t{1}, true, false);
        auto quotient = one_two / open_at_zero;
        REQUIRE(quotient.lower() == value_t{1});
        REQUIRE(quotient.upper() == std::numeric_limits<value_t>::infinity());
        REQUIRE(quotient.right_open());

        // Zero in the divisor is still an error
        REQUIRE_THROWS_AS(one_two / testing_t(value_t{0}, value_t{1}),
                          std::domain_error);
    }

    SECTION("Multiplication by a negative scalar") {
        // -2 * [1, 2) == (-4, -2]: the lower bound of the result comes from
        // the OPEN upper bound of the interval, and the upper bound from its
        // closed lower bound, so the openness swaps rather than being copied.
        auto result = value_t{-2} * one_two_right_open;
        REQUIRE(result == testing_t(value_t{-4}, value_t{-2}, true, false));
    }

    SECTION("print_interval_form") {
        std::stringstream ss;
        ss << std::to_string(one_two.lower()) << ", "
           << std::to_string(one_two.upper());
        auto guts = ss.str();

        REQUIRE(one_two.print_interval_form() == "[" + guts + "]");
        REQUIRE(one_two_left_open.print_interval_form() == "(" + guts + "]");
        REQUIRE(one_two_right_open.print_interval_form() == "[" + guts + ")");
        REQUIRE(one_two_open.print_interval_form() == "(" + guts + ")");
        REQUIRE(empty.print_interval_form() == "[∅]");
    }

    SECTION("operator<<(std::ostream, Interval)") {
        value_t lo = 1.0, hi = 2.0;
        auto value = testing_t(lo, hi);
        std::stringstream ss, corr;
        ss << value;
        corr << value.median() << "+/-" << value.radius();
        REQUIRE(ss.str() == corr.str());
    }

    SECTION("operator==") {
        REQUIRE(one_two == testing_t(1.0, 2.0));
        REQUIRE_FALSE(one_two == one_two_left_open);
        REQUIRE_FALSE(one_two == one_two_right_open);
        REQUIRE_FALSE(one_two == one_two_open);
        REQUIRE_FALSE(one_two == empty);

        REQUIRE(one_two_left_open == testing_t(1.0, 2.0, true, false));
        REQUIRE_FALSE(one_two_left_open == one_two);
        REQUIRE_FALSE(one_two_left_open == one_two_right_open);
        REQUIRE_FALSE(one_two_left_open == one_two_open);
        REQUIRE_FALSE(one_two_left_open == empty);

        REQUIRE(one_two_right_open == testing_t(1.0, 2.0, false, true));
        REQUIRE_FALSE(one_two_right_open == one_two);
        REQUIRE_FALSE(one_two_right_open == one_two_left_open);
        REQUIRE_FALSE(one_two_right_open == one_two_open);
        REQUIRE_FALSE(one_two_right_open == empty);

        REQUIRE(one_two_open == testing_t(1.0, 2.0, true, true));
        REQUIRE_FALSE(one_two_open == one_two);
        REQUIRE_FALSE(one_two_open == one_two_left_open);
        REQUIRE_FALSE(one_two_open == one_two_right_open);
        REQUIRE_FALSE(one_two_open == empty);

        REQUIRE(empty == testing_t());
        REQUIRE_FALSE(empty == one_two);
        REQUIRE_FALSE(empty == one_two_left_open);
        REQUIRE_FALSE(empty == one_two_right_open);
        REQUIRE_FALSE(empty == one_two_open);
    }

    SECTION("operator!=") {
        // Just negates operator== so spot check is fine.
        REQUIRE(one_two != one_two_left_open);
        REQUIRE_FALSE(one_two != one_two);
    }
}
