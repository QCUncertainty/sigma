#include "../testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_interval;
using testing::test_uncertain;

TEMPLATE_TEST_CASE("Basic (Interval)", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;

    SECTION("Absolute Value") {
        SECTION("All positive") {
            testing_t a(1.0, 3.0);
            test_interval(sigma::abs(a), 1.0, 3.0);
            test_interval(sigma::fabs(a), 1.0, 3.0);

            testing_t a_left_open(1.0, 3.0, true, false);
            REQUIRE(sigma::abs(a_left_open) == a_left_open);
            REQUIRE(sigma::fabs(a_left_open) == a_left_open);

            testing_t a_right_open(1.0, 3.0, false, true);
            test_interval(sigma::abs(a_right_open), 1.0, 3.0);
            test_interval(sigma::fabs(a_right_open), 1.0, 3.0);

            testing_t a_open(1.0, 3.0, true, true);
            REQUIRE(sigma::abs(a_open) == a_open);
            REQUIRE(sigma::fabs(a_open) == a_open);

            testing_t a_empty;
            REQUIRE(sigma::abs(a_empty) == a_empty);
            REQUIRE(sigma::fabs(a_empty) == a_empty);
        }
        SECTION("All negative") {
            testing_t a(-3.0, -1.0);
            test_interval(sigma::abs(a), 1.0, 3.0);
            test_interval(sigma::fabs(a), 1.0, 3.0);

            testing_t a_left_open(-3.0, -1.0, true, false);
            testing_t a_left_open_result(1.0, 3.0, false, true);
            REQUIRE(sigma::abs(a_left_open) == a_left_open_result);
            REQUIRE(sigma::fabs(a_left_open) == a_left_open_result);

            testing_t a_right_open(-3.0, -1.0, false, true);
            testing_t a_right_open_result(1.0, 3.0, true, false);
            REQUIRE(sigma::abs(a_right_open) == a_right_open_result);
            REQUIRE(sigma::fabs(a_right_open) == a_right_open_result);

            testing_t a_open(-3.0, -1.0, true, true);
            testing_t a_open_result(1.0, 3.0, true, true);
            REQUIRE(sigma::abs(a_open) == a_open_result);
            REQUIRE(sigma::fabs(a_open) == a_open_result);
        }
        SECTION("Straddles zero") {
            testing_t a(-2.0, 3.0);
            testing_t b(-3.0, 2.0);
            test_interval(sigma::abs(a), 0.0, 3.0);
            test_interval(sigma::fabs(a), 0.0, 3.0);
            test_interval(sigma::abs(b), 0.0, 3.0);
            test_interval(sigma::fabs(b), 0.0, 3.0);

            testing_t a_left_open(-2.0, 3.0, true, false);
            testing_t a_left_open_result(0.0, 3.0, false, false);
            testing_t b_left_open(-3.0, 2.0, true, false);
            testing_t b_left_open_result(0.0, 3.0, false, true);
            REQUIRE(sigma::abs(a_left_open) == a_left_open_result);
            REQUIRE(sigma::fabs(a_left_open) == a_left_open_result);
            REQUIRE(sigma::abs(b_left_open) == b_left_open_result);
            REQUIRE(sigma::fabs(b_left_open) == b_left_open_result);

            testing_t a_right_open(-2.0, 3.0, false, true);
            testing_t a_right_open_result(0.0, 3.0, false, true);
            testing_t b_right_open(-3.0, 2.0, false, true);
            testing_t b_right_open_result(0.0, 3.0, false, false);
            REQUIRE(sigma::abs(a_right_open) == a_right_open_result);
            REQUIRE(sigma::fabs(a_right_open) == a_right_open_result);
            REQUIRE(sigma::abs(b_right_open) == b_right_open_result);
            REQUIRE(sigma::fabs(b_right_open) == b_right_open_result);

            testing_t a_open(-2.0, 3.0, true, true);
            testing_t a_open_result(0.0, 3.0, false, true);
            testing_t b_open(-3.0, 2.0, true, true);
            testing_t b_open_result(0.0, 3.0, false, true);
            REQUIRE(sigma::abs(a_open) == a_open_result);
            REQUIRE(sigma::fabs(a_open) == a_open_result);
            REQUIRE(sigma::abs(b_open) == b_open_result);
            REQUIRE(sigma::fabs(b_open) == b_open_result);
        }
    }
}

TEMPLATE_TEST_CASE("Basic", "", sigma::UFloat, sigma::UDouble) {
    using testing_t = TestType;

    auto a = testing_t(1.0, 0.1);
    auto b = testing_t(1.3, 0.1);
    auto c = testing_t(2.0, 0.2);
    auto d = testing_t(3.0, 0.3);

    SECTION("Copy sign") {
        REQUIRE(a == sigma::copysign(a, b));
        REQUIRE(-a == sigma::copysign(a, -b));
        REQUIRE(-a == sigma::copysign(a, -1.0));
        REQUIRE(-1.0 == sigma::copysign(1.0, -b));
        test_uncertain(a + sigma::copysign(a, -1.0), 0.0, 0.0, 0);
    }
    SECTION("Absolute Value") {
        REQUIRE(a == sigma::abs(-a));
        REQUIRE(a == sigma::fabs(-a));
    }
    SECTION("Absolute Value Squared") { REQUIRE((c * c) == sigma::abs2(-c)); }
    SECTION("Ceiling") { test_uncertain(sigma::ceil(b), 2.0, 0.0, 0); }
    SECTION("Floor") { test_uncertain(sigma::floor(b), 1.0, 0.0, 0); }
    SECTION("Float modulo") {
        test_uncertain(sigma::fmod(d, c), 1.0, 0.3606, 2);
        test_uncertain(sigma::fmod(d, 2.0), 1.0, 0.3, 1);
        test_uncertain(sigma::fmod(3.0, c), 1.0, 0.2, 1);
    }
    SECTION("Truncation") {
        test_uncertain(sigma::trunc(b), 1.0, 0.0, 0);
        test_uncertain(sigma::trunc(-b), -1.0, 0.0, 0);
    }
    SECTION("Round") {
        test_uncertain(sigma::round(b), 1.0, 0.0, 0);
        test_uncertain(sigma::round(-b), -1.0, 0.0, 0);
        test_uncertain(sigma::round(testing_t{0.5}), 1.0, 0.0, 0);
    }
}
