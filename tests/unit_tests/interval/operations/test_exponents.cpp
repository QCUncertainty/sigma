#include "../testing.hpp"

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
    }
}
