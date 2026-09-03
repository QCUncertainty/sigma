#include "../testing.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>

using testing::test_interval;

TEMPLATE_TEST_CASE("Exponents", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    SECTION("Square Root") {
        // sqrt([1, 4]) = [1, 2]
        auto a = testing_t(value_t{1}, value_t{4});
        test_interval(sigma::sqrt(a), 1.0, 2.0);
        // sqrt([4, 9]) = [2, 3]
        auto b = testing_t(value_t{4}, value_t{9});
        test_interval(sigma::sqrt(b), 2.0, 3.0);
    }
    SECTION("Exponential Function") {
        // exp([0, 1]) = [1, e]
        auto a = testing_t(value_t{0}, value_t{1});
        test_interval(sigma::exp(a), 1.0, 2.7183);
        // exp([1, 2]) = [e, e^2]
        auto b = testing_t(value_t{1}, value_t{2});
        test_interval(sigma::exp(b), 2.7183, 7.3891);
    }
    SECTION("Natural Logarithm") {
        // log([1, e]) = [0, 1]
        auto a = testing_t(value_t{1}, value_t{2.7183});
        test_interval(sigma::log(a), 0.0, 1.0);
        // log([1, 1]) = [0, 0]
        auto b = testing_t(value_t{1}, value_t{1});
        test_interval(sigma::log(b), 0.0, 0.0);
    }
    SECTION("Power") {
        // pow([1, 2], 2) = [1, 4]
        auto a = testing_t(value_t{1}, value_t{2});
        test_interval(sigma::pow(a, 2), 1.0, 4.0);

        // pow([1, 2], 0.5) = [1, sqrt(2)]
        auto b = testing_t(value_t{1}, value_t{2});
        test_interval(sigma::pow(b, 0.5), 1.0, std::sqrt(2.0));

        // pow([1, 2], -1) = [0.5, 1]
        auto c = testing_t(value_t{1}, value_t{2});
        test_interval(sigma::pow(c, -1), 0.5, 1.0);

        // Even exponents are not monotonic across an interval spanning 0:
        // the minimum is the interior point x == 0, not either endpoint.
        // pow([-1, 1], 2) = [0, 1], NOT [1, 1] (what naively evaluating just
        // the endpoints would give).
        auto d = testing_t(value_t{-1}, value_t{1});
        test_interval(sigma::pow(d, 2), 0.0, 1.0);

        // pow([-2, 3], 2) = [0, 9], since |lower| < |upper|.
        auto e = testing_t(value_t{-2}, value_t{3});
        test_interval(sigma::pow(e, 2), 0.0, 9.0);

        // pow([-3, 1], 2) = [0, 9], since |lower| > |upper|.
        auto f = testing_t(value_t{-3}, value_t{1});
        test_interval(sigma::pow(f, 2), 0.0, 9.0);

        // Odd exponents ARE monotonic across all of R, so a spanning
        // interval is unaffected by the even-exponent fix above.
        // pow([-3, 1], 3) = [-27, 1]
        auto g = testing_t(value_t{-3}, value_t{1});
        test_interval(sigma::pow(g, 3), -27.0, 1.0);
    }
    SECTION("Outward Rounding") {
        // log(v), exp(v), and sqrt(v) are almost never representable, so a
        // degenerate argument must NOT give a degenerate result: a zero-width
        // result asserts the function is exact, and excludes the true value.
        for(auto v : {value_t{0.3}, value_t{0.5}, value_t{1.1}, value_t{1.5},
                      value_t{2}, value_t{7.25}}) {
            auto a = testing_t(v);
            REQUIRE(a.width() == value_t{0});

            REQUIRE(sigma::log(a).width() > value_t{0});
            REQUIRE(sigma::log(a).contains(std::log(v)));

            REQUIRE(sigma::exp(a).width() > value_t{0});
            REQUIRE(sigma::exp(a).contains(std::exp(v)));

            REQUIRE(sigma::sqrt(a).width() > value_t{0});
            REQUIRE(sigma::sqrt(a).contains(std::sqrt(v)));

            // A fractional power is evaluated as exp(power * log(a)), so it is
            // inexact for the same reason.
            REQUIRE(sigma::pow(a, 0.5).width() > value_t{0});
            REQUIRE(sigma::pow(a, 0.5).contains(std::pow(v, value_t{0.5})));

            // An integer power is repeated multiplication, which may well be
            // exact, so only containment is required of it. Containment is
            // still a real requirement: the multiplications have to round
            // outward, which they only do if the compiler has been told the
            // rounding mode is not fixed (see the -frounding-math flag the
            // sigma target attaches).
            REQUIRE(sigma::pow(a, 3).contains(std::pow(v, value_t{3})));
        }
    }
    SECTION("Domain Edges") {
        // An argument that reaches outside the domain is an error, not
        // something to be quietly trimmed back to the part that is in domain.
        REQUIRE_THROWS_AS(sigma::sqrt(testing_t(value_t{-4}, value_t{-1})),
                          std::domain_error);
        REQUIRE_THROWS_AS(sigma::sqrt(testing_t(value_t{-4}, value_t{9})),
                          std::domain_error);
        REQUIRE_THROWS_AS(sigma::log(testing_t(value_t{-2}, value_t{-1})),
                          std::domain_error);
        REQUIRE_THROWS_AS(sigma::log(testing_t(value_t{-2}, value_t{3})),
                          std::domain_error);

        // Zero is in the domain of the square root, but not of the logarithm
        test_interval(sigma::sqrt(testing_t(value_t{0}, value_t{9})), 0.0, 3.0);
        REQUIRE_THROWS_AS(sigma::log(testing_t(value_t{0}, value_t{1})),
                          std::domain_error);

        // An OPEN bound at zero is not a value the interval contains, so the
        // logarithm of it is defined -- and unbounded below.
        auto a = sigma::log(testing_t(value_t{0}, value_t{1}, true, false));
        REQUIRE(a.lower() == -std::numeric_limits<value_t>::infinity());
        REQUIRE(a.left_open());
        REQUIRE(a.contains(value_t{0}));

        // A non-integer power runs through the logarithm, so it inherits the
        // same domain: a negative base has no real fractional power.
        REQUIRE_THROWS_AS(sigma::pow(testing_t(value_t{-1}, value_t{4}), 0.5),
                          std::domain_error);

        // Zero is fine for a POSITIVE fractional power, though -- 0^0.5 is 0
        test_interval(sigma::pow(testing_t(value_t{0}, value_t{4}), 0.5), 0.0,
                      2.0);

        // A negative power is a reciprocal, so a base containing zero is the
        // division by zero that operator/ already rejects
        REQUIRE_THROWS_AS(sigma::pow(testing_t(value_t{-1}, value_t{1}), -1),
                          std::domain_error);
        REQUIRE_THROWS_AS(sigma::pow(testing_t(value_t{0}, value_t{2}), -2),
                          std::domain_error);
        REQUIRE_THROWS_AS(sigma::pow(testing_t(value_t{0}, value_t{4}), -0.5),
                          std::domain_error);

        // A bound merely AT zero, left open, does not contain zero
        auto b = sigma::pow(testing_t(value_t{0}, value_t{1}, true, false), -1);
        REQUIRE(b.lower() == value_t{1});
        REQUIRE(b.upper() == std::numeric_limits<value_t>::infinity());

        // The empty interval contains nothing out of domain, so it is
        // returned rather than throwing
        REQUIRE(sigma::sqrt(testing_t()).empty());
        REQUIRE(sigma::log(testing_t()).empty());
        REQUIRE(sigma::pow(testing_t(), 0.5).empty());
    }
    SECTION("Power (interval exponent)") {
        // A point exponent should agree with the plain-scalar overload
        // (loosely -- this overload goes through exp(exponent * log(a)),
        // which is not as tight as the scalar overload's case analysis).
        auto a           = testing_t(value_t{1}, value_t{2});
        auto point_exp   = testing_t(value_t{2}, value_t{2});
        auto from_ivl    = sigma::pow(a, point_exp);
        auto from_scalar = sigma::pow(a, 2);
        REQUIRE(from_ivl.contains(from_scalar));

        // pow([2, 2], [0, 1]) = [1, 2]
        auto base_point = testing_t(value_t{2}, value_t{2});
        auto exp_range  = testing_t(value_t{0}, value_t{1});
        test_interval(sigma::pow(base_point, exp_range), 1.0, 2.0);

        // Empty base or empty exponent gives the empty interval
        REQUIRE(sigma::pow(testing_t(), point_exp).empty());
        REQUIRE(sigma::pow(a, testing_t()).empty());

        // Inherits log's domain restriction to strictly positive bases
        REQUIRE_THROWS_AS(
          sigma::pow(testing_t(value_t{-1}, value_t{1}), point_exp),
          std::domain_error);
    }
}
