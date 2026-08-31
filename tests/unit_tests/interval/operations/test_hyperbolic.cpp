#include "../testing.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>

using testing::test_interval;

TEMPLATE_TEST_CASE("Hyperbolic", "", sigma::IFloat, sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;

    SECTION("Hyperbolic Sine") {
        // sinh([0, 1]) = [0, 1.1752]
        test_interval(sigma::sinh(testing_t(value_t{0}, value_t{1})), 0.0,
                      std::sinh(1.0));

        // sinh is increasing across all of R
        test_interval(sigma::sinh(testing_t(value_t{-1}, value_t{2})),
                      std::sinh(-1.0), std::sinh(2.0));
    }
    SECTION("Hyperbolic Cosine") {
        // The minimum of cosh([-1, 2]) is the interior point 0, not either
        // bound: cosh([-1, 2]) = [1, cosh(2)], NOT [cosh(-1), cosh(2)].
        test_interval(sigma::cosh(testing_t(value_t{-1}, value_t{2})), 1.0,
                      std::cosh(2.0));

        // The maximum comes from whichever bound is farther from zero
        test_interval(sigma::cosh(testing_t(value_t{-3}, value_t{1})), 1.0,
                      std::cosh(3.0));

        // cosh is decreasing on the negatives, so the bounds exchange places
        test_interval(sigma::cosh(testing_t(value_t{-2}, value_t{-1})),
                      std::cosh(1.0), std::cosh(2.0));
    }
    SECTION("Hyperbolic Tangent") {
        // tanh([0, 1]) = [0, 0.7616]
        test_interval(sigma::tanh(testing_t(value_t{0}, value_t{1})), 0.0,
                      std::tanh(1.0));
    }
    SECTION("Inverse Hyperbolic Sine") {
        // asinh([0, 1]) = [0, 0.8814]
        test_interval(sigma::asinh(testing_t(value_t{0}, value_t{1})), 0.0,
                      std::asinh(1.0));
    }
    SECTION("Inverse Hyperbolic Cosine") {
        // acosh([1, 2]) = [0, 1.3170]
        test_interval(sigma::acosh(testing_t(value_t{1}, value_t{2})), 0.0,
                      std::acosh(2.0));

        // The inverse hyperbolic cosine is undefined below one, so an
        // argument that reaches below it is an error rather than something to
        // trim back
        REQUIRE_THROWS_AS(sigma::acosh(testing_t(value_t{0}, value_t{2})),
                          std::domain_error);
        REQUIRE_THROWS_AS(sigma::acosh(testing_t(value_t{0}, value_t{0.5})),
                          std::domain_error);
    }
    SECTION("Inverse Hyperbolic Tangent") {
        // atanh([0, 0.5]) = [0, 0.5493]
        test_interval(sigma::atanh(testing_t(value_t{0}, value_t{0.5})), 0.0,
                      std::atanh(0.5));

        // The domain is open, so plus and minus one are themselves outside
        // it, and a CLOSED bound there is an error
        REQUIRE_THROWS_AS(sigma::atanh(testing_t(value_t{-1}, value_t{0})),
                          std::domain_error);
        REQUIRE_THROWS_AS(sigma::atanh(testing_t(value_t{0}, value_t{1})),
                          std::domain_error);
        REQUIRE_THROWS_AS(sigma::atanh(testing_t(value_t{2}, value_t{3})),
                          std::domain_error);

        // An OPEN bound there is not a value the interval contains, so the
        // argument is in domain -- and the result is unbounded on that side
        auto a = sigma::atanh(testing_t(value_t{-1}, value_t{0}, true, false));
        REQUIRE(a.lower() == -std::numeric_limits<value_t>::infinity());
        REQUIRE(a.left_open());
        REQUIRE(a.contains(value_t{0}));
    }
    SECTION("Outward Rounding") {
        // The hyperbolic functions of a representable value are almost never
        // representable, so a degenerate argument must NOT give a degenerate
        // result.
        for(auto v : {value_t{0}, value_t{0.25}, value_t{0.5}}) {
            auto a = testing_t(v);
            REQUIRE(a.width() == value_t{0});

            REQUIRE(sigma::sinh(a).width() > value_t{0});
            REQUIRE(sigma::sinh(a).contains(std::sinh(v)));

            REQUIRE(sigma::cosh(a).width() > value_t{0});
            REQUIRE(sigma::cosh(a).contains(std::cosh(v)));

            REQUIRE(sigma::tanh(a).width() > value_t{0});
            REQUIRE(sigma::tanh(a).contains(std::tanh(v)));

            REQUIRE(sigma::asinh(a).width() > value_t{0});
            REQUIRE(sigma::asinh(a).contains(std::asinh(v)));

            REQUIRE(sigma::atanh(a).width() > value_t{0});
            REQUIRE(sigma::atanh(a).contains(std::atanh(v)));

            auto b = testing_t(v + value_t{1.5});
            REQUIRE(sigma::acosh(b).width() > value_t{0});
            REQUIRE(sigma::acosh(b).contains(std::acosh(v + value_t{1.5})));
        }
    }
    SECTION("Empty") {
        auto a = testing_t();
        REQUIRE(sigma::sinh(a).empty());
        REQUIRE(sigma::cosh(a).empty());
        REQUIRE(sigma::tanh(a).empty());
        REQUIRE(sigma::asinh(a).empty());
        REQUIRE(sigma::acosh(a).empty());
        REQUIRE(sigma::atanh(a).empty());
    }
}
