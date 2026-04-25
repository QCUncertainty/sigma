#include "testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_interval;

TEMPLATE_TEST_CASE("Affine", "", float, double) {
    using value_t    = TestType;
    using affine_t   = sigma::Affine<TestType>;
    using interval_t = typename affine_t::interval_t;

    value_t zero  = 0.0;
    value_t one   = 1.0;
    value_t two   = 2.0;
    value_t three = 3.0;
    value_t four  = 4.0;
    SECTION("Constructors") {
        SECTION("Default") {
            auto value = affine_t();
            REQUIRE(value.range() == interval_t());
        }

        SECTION("From Center") {
            auto value = affine_t(value_t(1.0));
            test_interval(value.range(), one, one);
            test_interval(value.traditional_interval(), one, one);
        }

        SECTION("From Lower and Upper") {
            auto value = affine_t(one, two);
            test_interval(value.range(), one, two);
            test_interval(value.traditional_interval(), one, two);
        }

        SECTION("From Interval") {
            auto value = affine_t(interval_t(one, two));
            test_interval(value.range(), one, two);
            test_interval(value.traditional_interval(), one, two);

            auto value2 = affine_t(interval_t(-two, three));
            test_interval(value2.range(), -two, three);
            test_interval(value2.traditional_interval(), -two, three);
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
        test_interval(value2.traditional_interval(), -two, -one);
    }

    SECTION("operator+=") {
        SECTION("Value") {
            auto value = affine_t(one, two);
            value += value_t(3.0);
            test_interval(value.range(), four, value_t(5.0));
            test_interval(value.traditional_interval(), four, value_t(5.0));
        }
        SECTION("Independent") {
            auto value = affine_t(one, two);
            value += affine_t(three, four);
            test_interval(value.range(), four, value_t(6.0));
            test_interval(value.traditional_interval(), four, value_t(6.0));
        }
        SECTION("Dependent") {
            auto value = affine_t(one, two);
            value += value;
            test_interval(value.range(), two, four);
            test_interval(value.traditional_interval(), two, four);
        }
    }

    SECTION("operator-=") {
        SECTION("Value") {
            auto value = affine_t(one, two);
            value -= value_t(3.0);
            test_interval(value.range(), -two, -one);
            test_interval(value.traditional_interval(), -two, -one);
        }
        SECTION("Independent") {
            auto value = affine_t(one, two);
            value -= affine_t(three, four);
            test_interval(value.range(), -three, -one);
            test_interval(value.traditional_interval(), -three, -one);
        }
        SECTION("Dependent") {
            auto value = affine_t(one, two);
            value -= value;
            test_interval(value.range(), zero, zero);
            test_interval(value.traditional_interval(), -one, one);
        }
    }

    SECTION("operator*=") {
        SECTION("Value") {
            auto value = affine_t(one, two);
            value *= value_t(3.0);
            test_interval(value.range(), three, value_t(6.0));
            test_interval(value.traditional_interval(), three, value_t(6.0));
        }

        SECTION("Independent") {
            auto value = affine_t(one, two);
            value *= affine_t(three, four);
            // Without intersection with m_interval_ lower bound is 2.5
            // With intersection with m_interval_ lower bound is 3.0
            test_interval(value.range(), three, value_t(8.0));
            test_interval(value.traditional_interval(), three, value_t(8.0));
        }
        SECTION("Dependent") {
            auto value = affine_t(one, two);
            value *= value;
            // Without intersection with m_interval_ lower bound is 0.5
            // With intersection with m_interval_ lower bound is 1.0
            test_interval(value.range(), one, four);
            test_interval(value.traditional_interval(), one, four);
        }
    }

    SECTION("operator/=") {
        SECTION("Value") {
            auto value = affine_t(one, two);
            value /= value_t(3.0);
            test_interval(value.range(), value_t(1.0 / 3.0),
                          value_t(2.0 / 3.0));
            test_interval(value.traditional_interval(), value_t(1.0 / 3.0),
                          value_t(2.0 / 3.0));
        }
        SECTION("Independent") {
            auto value = affine_t(one, two);
            value /= affine_t(three, four);
            // Without intersection with m_interval_ lower bound is 0.2083333
            // With intersection with m_interval_ lower bound is 0.25
            test_interval(value.range(), value_t(0.25), value_t(2.0 / 3.0));
            test_interval(value.traditional_interval(), value_t(0.25),
                          value_t(2.0 / 3.0));
        }
        SECTION("Dependent") {
            auto value = affine_t(one, two);
            value /= value;

            test_interval(value.range(), value_t(5.0 / 8.0),
                          value_t(13.0 / 8.0));
            test_interval(value.traditional_interval(), value_t(1.0 / 2.0),
                          two);
        }
    }
}
