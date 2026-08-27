#include "../testing.hpp"
#include <cmath>
#include <limits>

using testing::test_interval;

TEMPLATE_TEST_CASE("Trigonometry", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    const auto pi      = value_t{3.14159265358979323846};
    const auto half_pi = pi / value_t{2};

    SECTION("Sine") {
        // sin([0, pi/2]) = [0, 1]
        test_interval(sigma::sin(testing_t(value_t{0}, half_pi)), 0.0, 1.0);

        // An interval spanning a peak encloses the peak, rather than being
        // the sine of the bounds: sin([0, 2 pi]) = [-1, 1], NOT [0, 0].
        test_interval(sigma::sin(testing_t(value_t{0}, value_t{2} * pi)), -1.0,
                      1.0);

        // sin([pi, 3 pi / 2]) = [-1, 0]
        test_interval(sigma::sin(testing_t(pi, value_t{3} * half_pi)), -1.0,
                      0.0);
    }
    SECTION("Cosine") {
        // cos([0, pi]) = [-1, 1]
        test_interval(sigma::cos(testing_t(value_t{0}, pi)), -1.0, 1.0);

        // The maximum of cos([-0.1, 0.1]) is the interior point 0, not either
        // bound.
        test_interval(sigma::cos(testing_t(value_t{-0.1}, value_t{0.1})),
                      std::cos(0.1), 1.0);

        // cos([pi, 2 pi]) = [-1, 1]
        test_interval(sigma::cos(testing_t(pi, value_t{2} * pi)), -1.0, 1.0);
    }
    SECTION("Tangent") {
        // tan([0, pi/4]) = [0, 1]
        test_interval(sigma::tan(testing_t(value_t{0}, pi / value_t{4})), 0.0,
                      1.0);

        // Tangent is unbounded at pi/2, so an interval containing it has no
        // finite enclosure.
        auto a = sigma::tan(testing_t(value_t{1}, value_t{2}));
        REQUIRE(a.lower() == -std::numeric_limits<value_t>::infinity());
        REQUIRE(a.upper() == std::numeric_limits<value_t>::infinity());
        REQUIRE(a.left_open());
        REQUIRE(a.right_open());
    }
    SECTION("Arcsine") {
        // asin([0, 1]) = [0, pi/2]
        test_interval(sigma::asin(testing_t(value_t{0}, value_t{1})), 0.0,
                      half_pi);

        // The part of the argument outside [-1, 1] is discarded
        test_interval(sigma::asin(testing_t(value_t{-2}, value_t{2})), -half_pi,
                      half_pi);

        // An argument entirely outside the domain encloses nothing
        REQUIRE(sigma::asin(testing_t(value_t{-3}, value_t{-2})).empty());
    }
    SECTION("Arccosine") {
        // acos([-1, 1]) = [0, pi]
        test_interval(sigma::acos(testing_t(value_t{-1}, value_t{1})), 0.0, pi);

        // Arccosine is decreasing, so the bounds exchange places:
        // acos([0, 1]) = [0, pi/2]
        test_interval(sigma::acos(testing_t(value_t{0}, value_t{1})), 0.0,
                      half_pi);

        REQUIRE(sigma::acos(testing_t(value_t{2}, value_t{3})).empty());
    }
    SECTION("Arctangent") {
        // atan([0, 1]) = [0, pi/4]
        test_interval(sigma::atan(testing_t(value_t{0}, value_t{1})), 0.0,
                      pi / value_t{4});
    }
    SECTION("Outward Rounding") {
        // The trigonometric functions of a representable value are almost
        // never representable, so a degenerate argument must NOT give a
        // degenerate result.
        for(auto v : {value_t{0}, value_t{0.25}, value_t{0.5}, value_t{1}}) {
            auto a = testing_t(v);
            REQUIRE(a.width() == value_t{0});

            REQUIRE(sigma::sin(a).width() > value_t{0});
            REQUIRE(sigma::sin(a).contains(std::sin(v)));

            REQUIRE(sigma::cos(a).width() > value_t{0});
            REQUIRE(sigma::cos(a).contains(std::cos(v)));

            REQUIRE(sigma::tan(a).width() > value_t{0});
            REQUIRE(sigma::tan(a).contains(std::tan(v)));

            REQUIRE(sigma::asin(a).width() > value_t{0});
            REQUIRE(sigma::asin(a).contains(std::asin(v)));

            REQUIRE(sigma::acos(a).width() > value_t{0});
            REQUIRE(sigma::acos(a).contains(std::acos(v)));

            REQUIRE(sigma::atan(a).width() > value_t{0});
            REQUIRE(sigma::atan(a).contains(std::atan(v)));
        }
    }
    SECTION("Empty") {
        auto a = testing_t();
        REQUIRE(sigma::sin(a).empty());
        REQUIRE(sigma::cos(a).empty());
        REQUIRE(sigma::tan(a).empty());
        REQUIRE(sigma::asin(a).empty());
        REQUIRE(sigma::acos(a).empty());
        REQUIRE(sigma::atan(a).empty());
    }
}
