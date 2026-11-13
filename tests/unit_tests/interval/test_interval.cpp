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

    SECTION("Constructors") {
        SECTION("Default") { REQUIRE(empty.empty()); }

        SECTION("With value") {
            testing_t one(1.0);
            test_interval(one, 1.0, 1.0);
            REQUIRE_FALSE(one.empty());
        }

        SECTION("With Lower and Upper") {
            SECTION("Closed Interval") {
                test_interval(one_two, 1.0, 2.0);
                REQUIRE_FALSE(one_two.empty());
            }
        }
        SECTION("Copy") {
            testing_t copy_empty(empty);
            REQUIRE(copy_empty == empty);
            REQUIRE(copy_empty.empty());

            testing_t copy_one_two(one_two);
            test_interval(copy_one_two, 1.0, 2.0);
            REQUIRE(copy_one_two == one_two);
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
        }
    }

    SECTION("width") {
        REQUIRE(one_two.width() == 1.0);

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
        REQUIRE_THROWS_AS(empty.median(), std::domain_error);
    }

    SECTION("radius") {
        REQUIRE(one_two.radius() == 0.5);
        REQUIRE_THROWS_AS(empty.radius(), std::domain_error);
    }

    SECTION("empty") {
        REQUIRE(empty.empty() == true);
        REQUIRE(one_two.empty() == false);
    }

    SECTION("lower") {
        REQUIRE(one_two.lower() == 1.0);
        REQUIRE_THROWS_AS(empty.lower(), std::domain_error);
    }

    SECTION("upper") {
        REQUIRE(one_two.upper() == 2.0);
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
        REQUIRE(one_two.contains(on_left));
        REQUIRE(one_two.contains(inside));
        REQUIRE(one_two.contains(on_right));
        REQUIRE_FALSE(one_two.contains(outside_right));
    }

    SECTION("contains with an infinite bound") {
        // An infinite bound marks an unbounded direction, not a member
        constexpr auto inf = std::numeric_limits<value_t>::infinity();
        testing_t unbounded(value_t{0}, inf);
        REQUIRE(unbounded.contains(value_t{1}));
        REQUIRE_FALSE(unbounded.contains(inf));
        REQUIRE(unbounded.contains(unbounded));
        REQUIRE(unbounded.contains(one_two));
        REQUIRE_FALSE(one_two.contains(unbounded));
    }

    SECTION("std::hash") {
        std::hash<testing_t> hasher;
        REQUIRE_NOTHROW(hasher(empty));
        REQUIRE(hasher(empty) == hasher(testing_t()));
    }

    SECTION("set_union") {
        SECTION("Union with Empty") {
            REQUIRE(empty.set_union(empty) == empty);
            REQUIRE(empty.set_union(one_two) == one_two);
        }
        SECTION("Expands left bound") {
            // We say "one" because "one_point_five" is too long
            testing_t zero_one(0.0, 1.5);

            testing_t zero_two(0.0, 2.0);

            REQUIRE(one_two.set_union(zero_one) == zero_two);
        }

        SECTION("Expands right bound") {
            testing_t one_three(1.5, 3.0);

            testing_t one_three2(1.0, 3.0);

            REQUIRE(one_two.set_union(one_three) == one_three2);
        }

        SECTION("Shared bound") {
            // Bounds are members, so intervals that only touch still overlap
            REQUIRE(one_two.set_union(testing_t(2.0, 3.0)) ==
                    testing_t(1.0, 3.0));
        }

        testing_t disjoint(4.0, 5.0);
        REQUIRE_THROWS_AS(one_two.set_union(disjoint), std::domain_error);
    }

    SECTION("set_intersection") {
        SECTION("Intersection with Empty") {
            REQUIRE(empty.set_intersection(empty) == empty);
            REQUIRE(empty.set_intersection(one_two) == empty);
        }
        SECTION("Expands left bound") {
            // We say "one" because "one_point_five" is too long
            testing_t zero_one(0.0, 1.5);

            testing_t one(1.0, 1.5);

            REQUIRE(one_two.set_intersection(zero_one) == one);
        }

        SECTION("Expands right bound") {
            testing_t one_three(1.5, 3.0);

            testing_t one_two2(1.5, 2.0);

            REQUIRE(one_two.set_intersection(one_three) == one_two2);
        }

        SECTION("Shared bound") {
            REQUIRE(one_two.set_intersection(testing_t(2.0, 3.0)) ==
                    testing_t(2.0, 2.0));
        }

        testing_t disjoint(4.0, 5.0);
        REQUIRE(one_two.set_intersection(disjoint) == testing_t());
    }

    SECTION("operator-(void)") {
        REQUIRE(-empty == empty);
        REQUIRE(-one_two == testing_t(-2.0, -1.0));
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
            }
            SECTION("Non-empty") {
                testing_t three_four(3.0, 4.0);

                testing_t four_six(4.0, 6.0);

                REQUIRE(one_two + three_four == four_six);
            }
        }
        SECTION("With Scalar") {
            value_t two(2.0);

            testing_t two_two(two, two);
            testing_t three_four(3.0, 4.0);

            REQUIRE(empty + two == two_two);
            REQUIRE(one_two + two == three_four);
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
            }
            SECTION("Non-empty") {
                testing_t three_four(3.0, 4.0);

                testing_t three_one(-3.0, -1.0);

                REQUIRE(one_two - three_four == three_one);
            }
        }
        SECTION("With Scalar") {
            value_t two(2.0);

            testing_t two_two(two, two);
            testing_t one_zero(-1.0, 0.0);

            REQUIRE(empty - two == -two_two);
            REQUIRE(two - empty == two_two);
            REQUIRE(one_two - two == one_zero);
            REQUIRE(two - one_two == -one_zero);
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
            }
            SECTION("Non-empty") {
                SECTION("result == [ll, hh]") {
                    testing_t rhs(3.0, 4.0);

                    testing_t result(3.0, 8.0);

                    REQUIRE(one_two * rhs == result);
                }
                SECTION("result == [lh, ll]") {
                    testing_t rhs(-4.0, 3.0);
                    testing_t lhs(-3.0, 2.0);

                    testing_t result(-9.0, 12.0);

                    REQUIRE(lhs * rhs == result);
                }
                SECTION("result == [lh, hl]") {
                    testing_t lhs(-4.0, -3.0);

                    testing_t result(-8.0, -3.0);

                    REQUIRE(lhs * one_two == result);
                }
                SECTION("result == [lh, hh]") {
                    testing_t lhs(-3.0, 4.0);

                    testing_t result(-6.0, 8.0);

                    REQUIRE(lhs * one_two == result);
                }
                SECTION("result == [hl, ll]") {
                    testing_t lhs(-4.0, 3.0);
                    testing_t rhs(-3.0, 2.0);

                    testing_t result(-9.0, 12.0);

                    REQUIRE(lhs * rhs == result);
                }
                SECTION("result == [hl, lh]") {
                    testing_t rhs(-4.0, -3.0);

                    testing_t result(-8.0, -3.0);

                    REQUIRE(one_two * rhs == result);
                }
                SECTION("result == [hl, hh]") {
                    testing_t rhs(-3.0, 4.0);

                    testing_t result(-6.0, 8.0);

                    REQUIRE(one_two * rhs == result);
                }
                SECTION("result == [hh,ll]") {
                    testing_t lhs(-4.0, -3.0);
                    testing_t rhs(-2.0, -1.0);

                    testing_t result(3.0, 8.0);

                    REQUIRE(lhs * rhs == result);
                }
            }
            SECTION("value") {
                value_t two(2.0);

                testing_t result(2.0, 4.0);

                REQUIRE(one_two * two == result);
                REQUIRE(two * one_two == result);
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
            }
            SECTION("Non-empty") {
                // The implementation consists of calling
                // 1. 1.0 / rhs.
                // 2. multiplying lhs by the result of 1.
                // Step 1 is tested in the "With Value" section below and works.
                // Step 2 is tested in the operator* section above and works.
                // We just spot check here.

                testing_t result(0.5, 2.0);

                REQUIRE(one_two / one_two == result);

                REQUIRE_THROWS_AS(one_two / value_t(0.0), std::domain_error);
                REQUIRE_THROWS_AS(one_two / testing_t(-1.0, 1.0),
                                  std::domain_error);
                // A bound is a member, so a divisor bounded by zero contains
                // it. Dividing by one that only approaches zero needs a
                // GeneralInterval.
                REQUIRE_THROWS_AS(one_two / testing_t(0.0, 1.0),
                                  std::domain_error);
                REQUIRE_THROWS_AS(one_two / testing_t(-1.0, 0.0),
                                  std::domain_error);
            }
        }
        SECTION("With Value") {
            value_t two(2.0);

            // Result when two is on left side
            testing_t result_lhs(1.0, 2.0);

            // Result when two is on right side
            testing_t result_rhs(0.5, 1.0);

            REQUIRE(empty / two == empty);
            REQUIRE(two / empty == empty);

            REQUIRE(one_two / two == result_rhs);
            REQUIRE(two / one_two == result_lhs);

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

    SECTION("print_interval_form") {
        std::stringstream ss;
        ss << std::to_string(one_two.lower()) << ", "
           << std::to_string(one_two.upper());
        auto guts = ss.str();

        REQUIRE(one_two.print_interval_form() == "[" + guts + "]");
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
        REQUIRE_FALSE(one_two == empty);

        REQUIRE(empty == testing_t());
        REQUIRE_FALSE(empty == one_two);
    }

    SECTION("operator!=") {
        // Just negates operator== so spot check is fine.
        REQUIRE(one_two != testing_t(1.0, 3.0));
        REQUIRE_FALSE(one_two != one_two);
    }

    SECTION("Relational operators (ordering by median, not a rigorous "
            "enclosure comparison)") {
        testing_t two_three(2.0, 3.0);

        REQUIRE(one_two < two_three);
        REQUIRE_FALSE(two_three < one_two);
        REQUIRE(two_three > one_two);
        REQUIRE(one_two <= testing_t(1.0, 2.0));
        REQUIRE(one_two >= testing_t(1.0, 2.0));

        // The empty interval sorts before every non-empty one, and is
        // equivalent only to itself.
        REQUIRE(empty < one_two);
        REQUIRE_FALSE(one_two < empty);
        REQUIRE_FALSE(empty < testing_t());
    }
}
