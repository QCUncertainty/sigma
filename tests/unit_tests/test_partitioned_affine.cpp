#include "testing.hpp"
#include <sigma/sigma.hpp>

using testing::test_interval;

TEMPLATE_TEST_CASE("PartitionedAffine", "", float, double) {
    using value_t     = TestType;
    using pinterval_t = sigma::PartitionedAffine<TestType>;
    using interval_t  = typename pinterval_t::interval_t;

    value_t zero  = 0.0;
    value_t one   = 1.0;
    value_t two   = 2.0;
    value_t three = 3.0;
    value_t four  = 4.0;

    SECTION("Constructors") {
        SECTION("Default") {
            pinterval_t value;
            REQUIRE(value.range() == interval_t());
            REQUIRE(value.num_partitions() == 0);
        }

        SECTION("From Lower and Upper") {
            pinterval_t value(zero, four);
            test_interval(value.range(), zero, four);
            REQUIRE(value.num_partitions() == 10);

            pinterval_t value2(zero, zero, 20);
            test_interval(value2.range(), zero, zero);
            REQUIRE(value2.num_partitions() == 0);
        }

        SECTION("From Interval") {
            pinterval_t value(interval_t(one, two));
            test_interval(value.range(), one, two);
            REQUIRE(value.num_partitions() == 10);

            pinterval_t value2(interval_t(-two, three));
            test_interval(value2.range(), -two, three);
            REQUIRE(value2.num_partitions() == 10);

            pinterval_t value3(interval_t(-two, -one), 20);
            test_interval(value3.range(), -two, -one);
            REQUIRE(value3.num_partitions() == 20);
        }
    }

    SECTION("num_partitions") {
        pinterval_t defaulted;
        REQUIRE(defaulted.num_partitions() == 0);

        pinterval_t value(interval_t(one, two));
        REQUIRE(value.num_partitions() == 10);

        pinterval_t value2(interval_t(one, two), 20);
        REQUIRE(value2.num_partitions() == 20);
    }

    SECTION("center") {
        pinterval_t value(interval_t(one, three));
        REQUIRE(value.center() == Catch::Approx(2.0));

        pinterval_t value2(interval_t(-one, two), 20);
        REQUIRE(value2.center() == Catch::Approx(0.5));

        pinterval_t value3(interval_t(-four, -one), 20);
        REQUIRE(value3.center() == Catch::Approx(-2.5));

        REQUIRE_THROWS_AS(pinterval_t{}.center(), std::domain_error);
    }

    SECTION("range") {
        pinterval_t empty;
        REQUIRE(empty.range() == interval_t());

        pinterval_t value(interval_t(one, two));
        test_interval(value.range(), one, two);

        pinterval_t value2(interval_t(one, two), 20);
        test_interval(value2.range(), one, two);
    }

    SECTION("radius") {
        pinterval_t value(interval_t(one, two));
        REQUIRE(value.radius() == Catch::Approx(0.5));

        pinterval_t value2(interval_t(one, two), 20);
        REQUIRE(value2.radius() == Catch::Approx(0.5));

        pinterval_t value3(interval_t(-four, -one), 20);
        REQUIRE(value3.radius() == Catch::Approx(1.5));

        pinterval_t empty;
        REQUIRE(empty.radius() == 0.0);

        pinterval_t scalar(one, one);
        REQUIRE(scalar.radius() == 0.0);
    }

    SECTION("contains") {
        auto value = pinterval_t(one, two);
        REQUIRE(value.contains(one));
        REQUIRE(value.contains(value_t(1.5)));
        REQUIRE(value.contains(two));
        REQUIRE_FALSE(value.contains(value_t(0.5)));

        REQUIRE(value.contains(interval_t(one, two)));
        REQUIRE(value.contains(interval_t(value_t(1.5), value_t(1.5))));
        REQUIRE_FALSE(value.contains(interval_t(one, three)));
        REQUIRE_FALSE(value.contains(interval_t(zero, value_t(1.5))));

        REQUIRE(value.contains(pinterval_t(one, two)));
        REQUIRE_FALSE(value.contains(pinterval_t(one, three)));
        REQUIRE_FALSE(value.contains(pinterval_t(zero, one)));
    }

    SECTION("operator-") {
        pinterval_t value(interval_t(one, two));
        auto value2 = -value;
        test_interval(value2.range(), -two, -one);

        pinterval_t value3(interval_t(-four, -one), 20);
        auto value4 = -value3;
        test_interval(value4.range(), one, four);

        pinterval_t empty;
        auto empty2 = -empty;
        REQUIRE(empty2.range() == interval_t());
    }

    SECTION("operator+=") {
        SECTION("Independent") {
            pinterval_t value(one, two);
            value += pinterval_t(three, four);
            test_interval(value.range(), four, value_t(6.0));

            pinterval_t value2(interval_t(-one, two), 20);
            value2 += pinterval_t(interval_t(three, four), 20);
            test_interval(value2.range(), two, value_t(6.0));

            pinterval_t value3(interval_t(-two, -one), 20);
            value3 += pinterval_t(interval_t(three, four), 20);
            test_interval(value3.range(), one, three);

            pinterval_t value4(one, two);
            pinterval_t value_temp(-four, -three);
            value4 += value_temp;
            test_interval(value4.range(), -three, -one);
        }
        SECTION("Dependent (same object; shared error_term keys merge)") {
            pinterval_t value(one, two);
            value += value;
            test_interval(value.range(), two, four);

            pinterval_t value2(-one, two);
            value2 += value2;
            test_interval(value2.range(), -two, four);

            pinterval_t value3(-two, -one);
            value3 += value3;
            test_interval(value3.range(), -four, -two);
        }
    }

    SECTION("operator-=") {
        SECTION("Independent") {
            pinterval_t value(one, two);
            value -= pinterval_t(three, four);
            test_interval(value.range(), -three, -one);

            pinterval_t value2(interval_t(-one, two), 20);
            value2 -= pinterval_t(interval_t(three, four), 20);
            test_interval(value2.range(), -value_t(5.0), -one);

            pinterval_t value3(interval_t(-two, -one), 20);
            value3 -= pinterval_t(interval_t(three, four), 20);
            test_interval(value3.range(), -value_t(6.0), -four);
        }
        SECTION("Dependent (same object; x - x, Minkowski on certificate)") {
            pinterval_t value(one, two);
            value -= value;
            // cert + neg(cert): [1,2] + [-2,-1] = [-1,1] (independent IA on
            // m_certificate_)
            test_interval(value.range(), -one, one);

            pinterval_t value2(interval_t(-one, two), 20);
            value2 -= value2;
            test_interval(value2.range(), -three, three);

            pinterval_t value3(interval_t(-two, -one), 20);
            value3 -= value3;
            test_interval(value3.range(), -one, one);
        }
    }

    SECTION("operator*=") {
        // SECTION("Value") {
        //     auto value = affine_t(one, two);
        //     value *= value_t(3.0);
        //     test_interval(value.range(), three, value_t(6.0));
        // }

        // SECTION("Independent") {
        //     pinterval_t value(one, two, 10);
        //     value *= pinterval_t(three, four, 10);
        //     test_interval(value.range(), 3.0, 8.0);

        //     pinterval_t value2(three, value_t(8.0), 100);
        //     value2 *= pinterval_t(interval_t(-three, -two), 100);
        //     test_interval(value2.range(), -24.0, -6.0);

        //     value *= pinterval_t(interval_t(-two, two));
        //     test_interval(value.range(), value_t(-6.0), value_t(16.0));
        // }
        SECTION("Dependent") {
            // pinterval_t value(-one, two);
            // value *= value;
            // test_interval(value.range(), -0.23750012, 4.202500);
        }
    }

    //     SECTION("operator/=") {
    //         // SECTION("Value") {
    //         //     auto value = paffine_t(one, two);
    //         //     value /= value_t(3.0);
    //         //     test_interval(value.range(), value_t(1.0 / 3.0),
    //         //                   value_t(2.0 / 3.0));
    //         //     test_interval(value.traditional_interval(), value_t(1.0
    //         / 3.0),
    //         //                   value_t(2.0 / 3.0));
    //         // }
    //         // SECTION("Independent") {
    //         //     auto value = paffine_t(one, two);
    //         //     value /= paffine_t(three, four);
    //         //     std::cout << value.print_interval_form() << std::endl;
    //         //     std::cout << value.range().print_interval_form() <<
    //         std::endl;
    //         //     test_interval(value.range(), value_t(0.25), value_t(2.0
    //         / 3.0));
    //         //     test_interval(value.traditional_interval(), value_t(0.25),
    //         //                   value_t(2.0 / 3.0));
    //         // }
    //         // SECTION("Dependent") {
    //         //     auto value = paffine_t(one, two);
    //         //     value /= value;
    //         //     std::cout << value.range().print_interval_form() <<
    //         std::endl;
    //         //     test_interval(value.range(), 0.991306067, 1.013248);
    //         //     test_interval(value.traditional_interval(),
    //         0.909008, 1.100100);
    //         // }
    //     }
}
