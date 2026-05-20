#include "../testing.hpp"
#include <sstream>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Setter", "", sigma::UFloat, sigma::UDouble) {
    using uncertain_t = TestType;
    using testing_t   = sigma::detail_::Setter<uncertain_t>;

    uncertain_t a(3.0, 0.3);
    uncertain_t b(4.0, 0.4);
    uncertain_t c(3.0, 0.3, 0.0);
    uncertain_t d(5.0, 0.5, 0.01);
    uncertain_t e(1.0, 0.001);
    testing_t testing_a(a);
    testing_t testing_b(b);
    testing_t testing_c(c);
    testing_t testing_d(d);

    SECTION("Update the mean") {
        SECTION("Difference above threshold") {
            testing_a.update_mean(2.0);
            test_uncertain(a, 2.0, 0.3, 1);
        }
        SECTION("Difference below threshold") {
            testing_d.update_mean(5.001);
            test_uncertain(d, 5.0, 0.5, 1, 0.01);
        }
    }
    SECTION("Update the derivatives") {
        SECTION("Only existing derivatives") {
            SECTION("Derivative of one") {
                testing_a.update_derivatives(1.0);
                test_uncertain(a, 3.0, 0.3, 1);
            }
            SECTION("Derivative of zero") {
                testing_c.update_derivatives(0.0);
                test_uncertain(c, 3.0, 0.0, 0, 0.0);
            }
            SECTION("Derivative below threshold") {
                testing_d.update_derivatives(0.001);
                test_uncertain(d, 5.0, 0.0, 0, 0.01);
            }
            SECTION("Derivative of negative one") {
                testing_a.update_derivatives(-1.0);
                test_uncertain(a, 3.0, 0.3, 1);
            }
            SECTION("Derivative updated, no eliminations") {
                testing_a.update_derivatives(2.0);
                test_uncertain(a, 3.0, 0.6, 1);
            }
            SECTION("Derivative updated, resulting in eliminations") {
                // Repeated updates to ensure we reduce the derivative below the
                // threshold and eliminate the dependency from the map.
                testing_d.update_derivatives(0.1);
                testing_d.update_derivatives(0.1);
                testing_d.update_derivatives(0.1);
                test_uncertain(d, 5.0, 0.0, 0, 0.01);
            }
        }
        SECTION("One list of derivatives") {
            SECTION("Derivative of zero") {
                testing_c.update_derivatives(c.deps(), 0.0);
                testing_c.update_derivatives(b.deps(), 0.0);
                test_uncertain(c, 3.0, 0.3, 1, 0.0);
            }
            SECTION("Derivative below threshold") {
                testing_d.update_derivatives(d.deps(), 0.001);
                test_uncertain(d, 5.0, 0.5, 1, 0.01);
            }
            SECTION("Only pre-existing entries in map") {
                testing_a.update_derivatives(a.deps(), 1.0);
                test_uncertain(a, 3.0, 0.6, 1);
            }
            SECTION("Pre-existing entries with eliminations") {
                testing_d.update_derivatives(d.deps(), -1.001);
                test_uncertain(d, 5.0, 0, 0, 0.01);
            }
            SECTION("Pre-existing reduced to zero") {
                testing_c.update_derivatives(c.deps(), -1.0);
                test_uncertain(c, 3.0, 0.0, 0, 0.0);
            }
            SECTION("Only new entries in map") {
                testing_a.update_derivatives(b.deps(), 1.0);
                test_uncertain(a, 3.0, 0.5, 2);
            }
            SECTION("Only new entries below threshold") {
                // Setup for b's map
                testing_a.update_derivatives(0.001);
                // Add to d
                testing_d.update_derivatives(a.deps(), 1.0);
                test_uncertain(d, 5.0, 0.5, 1, 0.01);
                // testing_d.update_derivatives(e.deps(), 1.0);
                // test_uncertain(d, 5.0, 0.5, 1, 0.01);
            }
            SECTION("Pre-existing and new entries in map") {
                // Setup for b's map
                testing_b.update_derivatives(3.0);
                testing_b.update_derivatives(a.deps(), 0.2 / 0.3);
                // Add to a
                testing_a.update_derivatives(b.deps(), 1.0);
                test_uncertain(a, 3.0, 1.3, 2);
            }
        }
    }
}
