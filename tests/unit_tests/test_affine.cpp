#include "testing.hpp"
#include <sigma/sigma.hpp>
#include <type_traits>
#include <unordered_map>

using testing::test_interval;

TEMPLATE_TEST_CASE("Affine", "", float, double) {
    using value_t    = TestType;
    using affine_t   = sigma::Affine<TestType>;
    using interval_t = typename affine_t::interval_t;
    static_assert(
      std::is_same_v<
        typename affine_t::error_terms_t,
        std::unordered_map<typename affine_t::error_term_t, value_t>>);

    value_t zero  = 0.0;
    value_t one   = 1.0;
    value_t two   = 2.0;
    value_t three = 3.0;
    value_t four  = 4.0;
    SECTION("Constructors") {
        SECTION("Default") {
            auto value = affine_t();
            test_interval(value.range(), zero, zero);
        }

        SECTION("From Center") {
            auto value = affine_t(value_t(1.0));
            test_interval(value.range(), one, one);
        }

        SECTION("From Lower and Upper") {
            auto value = affine_t(one, two);
            test_interval(value.range(), one, two);
        }

        SECTION("From Interval") {
            auto value = affine_t(interval_t(one, two));
            test_interval(value.range(), one, two);

            auto value2 = affine_t(interval_t(-two, three));
            test_interval(value2.range(), -two, three);
        }
    }

    SECTION("contains") {
        auto value = affine_t(one, two);
        REQUIRE(value.contains(one));
        REQUIRE(value.contains(value_t(1.5)));
        REQUIRE(value.contains(two));
        REQUIRE_FALSE(value.contains(value_t(0.5)));

        REQUIRE(value.contains(interval_t(one, two)));
        REQUIRE(value.contains(interval_t(value_t(1.5), value_t(1.5))));
        REQUIRE_FALSE(value.contains(interval_t(one, three)));
        REQUIRE_FALSE(value.contains(interval_t(zero, one)));

        REQUIRE(value.contains(affine_t(one, two)));
        REQUIRE_FALSE(value.contains(affine_t(one, three)));
        REQUIRE_FALSE(value.contains(affine_t(zero, one)));
    }

    SECTION("operator-") {
        auto value  = affine_t(one, two);
        auto value2 = -value;
        test_interval(value2.range(), -two, -one);
    }

    SECTION("operator+=") {
        SECTION("Value") {
            auto value = affine_t(one, two);
            value += value_t(3.0);
            test_interval(value.range(), four, value_t(5.0));
        }
        SECTION("Independent") {
            auto value = affine_t(one, two);
            value += affine_t(three, four);
            test_interval(value.range(), four, value_t(6.0));
        }
        SECTION("Dependent") {
            auto value = affine_t(one, two);
            value += value;
            test_interval(value.range(), two, four);
        }
    }

    SECTION("operator-=") {
        SECTION("Value") {
            auto value = affine_t(one, two);
            value -= value_t(3.0);
            test_interval(value.range(), -two, -one);
        }
        SECTION("Independent") {
            auto value = affine_t(one, two);
            value -= affine_t(three, four);
            test_interval(value.range(), -three, -one);
        }
        SECTION("Dependent") {
            auto value = affine_t(one, two);
            value -= value;
            test_interval(value.range(), zero, zero);
        }
    }

    SECTION("operator*=") {
        SECTION("Value") {
            auto value = affine_t(one, two);
            value *= value_t(3.0);
            test_interval(value.range(), three, value_t(6.0));
        }

        SECTION("Independent") {
            auto value = affine_t(one, two);
            value *= affine_t(three, four);
            // Tight range is [3, 8]
            test_interval(value.range(), value_t(2.5), value_t(8.0));
        }
        SECTION("Dependent") {
            auto value = affine_t(one, two);
            value *= value;
            // Tight range is [1, 4]
            test_interval(value.range(), value_t(0.5), value_t(4.0));

            affine_t value2(-two, two);
            value2 *= value2;
            // Tight range is [-4, 4]
            test_interval(value2.range(), -four, four);
        }
    }

    SECTION("operator/=") {
        SECTION("Value") {
            auto value = affine_t(one, two);
            value /= value_t(3.0);
            test_interval(value.range(), value_t(1.0 / 3.0),
                          value_t(2.0 / 3.0));
        }
        SECTION("Independent") {
            auto value = affine_t(one, two);
            value /= affine_t(three, four);
            // Tight range is [1/4, 2/3]
            test_interval(value.range(), value_t(0.2083333),
                          value_t(2.0 / 3.0));
        }
        SECTION("Dependent") {
            auto value = affine_t(one, two);
            value /= value;
            // Tight range is [1, 1]
            test_interval(value.range(), value_t(5.0 / 8.0),
                          value_t(13.0 / 8.0));
        }
    }
}
