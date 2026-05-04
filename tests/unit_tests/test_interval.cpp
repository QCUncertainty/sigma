#include "catch2/catch_test_macros.hpp"
#include "testing.hpp"
#include <sigma/sigma.hpp>
#include <sstream>

using testing::test_interval;

TEMPLATE_TEST_CASE("Interval", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;
    using other_t   = typename testing::test_traits<TestType>::other_t;

    testing_t empty;
    testing_t one_two(1.0, 2.0);
    testing_t one_two_left_open(1.0, 2.0, true, false);
    testing_t one_two_right_open(1.0, 2.0, false, true);
    testing_t one_two_open(1.0, 2.0, true, true);

    SECTION("Constructors") {
        SECTION("Default") {
            REQUIRE(empty.empty());
            REQUIRE(empty.width() == 0.0);
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
        REQUIRE(empty.width() == 0.0);
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

    SECTION("operator<<(std::ostream, Interval)") {
        value_t lo = 1.0, hi = 2.0;
        auto value = testing_t(lo, hi);
        std::stringstream ss, corr;
        ss << value;
        corr << value.median() << "+/-" << value.radius();
        REQUIRE(ss.str() == corr.str());
    }
    SECTION("Comparisons") {
        auto first = testing_t(1.0, 2.0);
        SECTION("Same") {
            auto second = first;
            REQUIRE(first == second);
            REQUIRE_FALSE(first != second);
        }
        SECTION("Different Value Type") {
            auto second = other_t(1.0, 2.0);
            REQUIRE_FALSE(first == second);
        }
        SECTION("Different Lower") {
            auto second = testing_t(0.5, 2.0);
            REQUIRE_FALSE(first == second);
            REQUIRE(first != second);
        }
        SECTION("Different Upper") {
            auto second = testing_t(1.0, 3.0);
            REQUIRE_FALSE(first == second);
            REQUIRE(first != second);
        }
        SECTION("Relative") {
            // [1,2] is certainly less than [3,4]
            auto second = testing_t(3.0, 4.0);
            // [1.5, 2.5] overlaps with [1,2]
            auto third = testing_t(1.5, 2.5);
            SECTION("Less than") {
                REQUIRE(first < second);
                REQUIRE(third < second);
                REQUIRE_FALSE(second < first);
                REQUIRE_FALSE(first < third);
            }
            SECTION("Greater than") {
                REQUIRE(second > first);
                REQUIRE(second > third);
                REQUIRE_FALSE(first > second);
                REQUIRE_FALSE(third > first);
            }
            SECTION("Less than or equal") {
                REQUIRE(first <= second);
                REQUIRE(third <= second);
                REQUIRE_FALSE(second <= first);
                REQUIRE_FALSE(first <= third);
            }
            SECTION("Greater than or equal") {
                REQUIRE(second >= first);
                REQUIRE(second >= third);
                REQUIRE_FALSE(first >= second);
                REQUIRE_FALSE(third >= first);
            }
        }
    }
    SECTION("contains") {
        auto interval = testing_t(value_t{1}, value_t{3});

        SECTION("interior") { REQUIRE(interval.contains(value_t{2})); }

        SECTION("endpoints are included") {
            REQUIRE(interval.contains(value_t{1}));
            REQUIRE(interval.contains(value_t{3}));
        }

        SECTION("outside") {
            REQUIRE_FALSE(interval.contains(value_t{0.99}));
            REQUIRE_FALSE(interval.contains(value_t{3.01}));
        }

        SECTION("degenerate interval (single point)") {
            auto point = testing_t(value_t{5}, value_t{5});
            REQUIRE(point.contains(value_t{5}));
            REQUIRE_FALSE(point.contains(value_t{4.99}));
            REQUIRE_FALSE(point.contains(value_t{5.01}));
        }

        SECTION("default constructed (empty)") {
            testing_t empty{};
            REQUIRE(empty.contains(empty));
            REQUIRE_FALSE(empty.contains(value_t{0}));
            REQUIRE_FALSE(empty.contains(value_t{-0.01}));
            REQUIRE_FALSE(empty.contains(value_t{0.01}));
        }
    }
    SECTION("Arithmetic") {
        // a = [1, 3], b = [2, 4]
        auto a = testing_t(value_t{1}, value_t{3});
        auto b = testing_t(value_t{2}, value_t{4});

        SECTION("Negation") { test_interval(-a, -3.0, -1.0); }
        SECTION("Addition") {
            SECTION("With Interval") { test_interval(a + b, 3.0, 7.0); }
            SECTION("With Scalar") {
                test_interval(a + value_t{1}, 2.0, 4.0);
                test_interval(value_t{1} + a, 2.0, 4.0);
            }
        }
        SECTION("Addition Assignment") {
            SECTION("With Interval") {
                auto x = a;
                x += b;
                test_interval(x, 3.0, 7.0);
            }
            SECTION("With Scalar") {
                auto x = a;
                x += value_t{1};
                test_interval(x, 2.0, 4.0);
            }
        }
        SECTION("Subtraction") {
            SECTION("With Interval") { test_interval(a - b, -3.0, 1.0); }
            SECTION("With Scalar") {
                test_interval(a - value_t{1}, 0.0, 2.0);
                test_interval(value_t{1} - a, -2.0, 0.0);
            }
        }
        SECTION("Subtraction Assignment") {
            SECTION("With Interval") {
                auto x = a;
                x -= b;
                test_interval(x, -3.0, 1.0);
            }
            SECTION("With Scalar") {
                auto x = a;
                x -= value_t{1};
                test_interval(x, 0.0, 2.0);
            }
        }
        SECTION("Multiplication") {
            SECTION("By Interval") { test_interval(a * b, 2.0, 12.0); }
            SECTION("By Scalar") {
                test_interval(a * value_t{2}, 2.0, 6.0);
                test_interval(value_t{2} * a, 2.0, 6.0);
            }
        }
        SECTION("Multiplication Assignment") {
            SECTION("By Interval") {
                auto x = a;
                x *= b;
                test_interval(x, 2.0, 12.0);
            }
            SECTION("By Scalar") {
                auto x = a;
                x *= value_t{2};
                test_interval(x, 2.0, 6.0);
            }
        }
        SECTION("Division") {
            SECTION("By Interval") { test_interval(a / b, 0.25, 1.5); }
            SECTION("By Scalar") {
                test_interval(a / value_t{2}, 0.5, 1.5);
                test_interval(value_t{2} / a, 0.6667, 2.0);
            }
        }
        SECTION("Division Assignment") {
            SECTION("By Interval") {
                auto x = a;
                x /= b;
                test_interval(x, 0.25, 1.5);
            }
            SECTION("By Scalar") {
                auto x = a;
                x /= value_t{2};
                test_interval(x, 0.5, 1.5);
            }
        }
    }
}
