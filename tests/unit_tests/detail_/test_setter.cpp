#include "../testing.hpp"
#include <sigma/sigma.hpp>
#include <sstream>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Setter", "", sigma::UFloat, sigma::UDouble) {
    using uncertain_t = TestType;
    using testing_t   = sigma::detail_::Setter<uncertain_t>;

    uncertain_t a(3.0, 0.3);
    uncertain_t b(4.0, 0.4);
    testing_t testing_a(a);
    testing_t testing_b(b);

    SECTION("Update the mean") {
        testing_a.update_mean(2.0);
        test_uncertain(a, 2.0, 0.3, 1);
    }
    SECTION("Update the derivatives") {
        SECTION("Only existing derivatives") {
            testing_a.update_derivatives(2.0);
            test_uncertain(a, 3.0, 0.6, 1);
        }
        SECTION("One list of derivatives") {
            SECTION("Derivative of Zero") {
                testing_a.update_derivatives(a.deps(), 0.0);
                testing_a.update_derivatives(b.deps(), 0.0);
                test_uncertain(a, 3.0, 0.3, 1);
            }
            SECTION("Only pre-existing entries in map") {
                testing_a.update_derivatives(a.deps(), 1.0);
                test_uncertain(a, 3.0, 0.6, 1);
            }
            SECTION("Pre-existing reduced to zero") {
                testing_a.update_derivatives(a.deps(), -1.0);
                test_uncertain(a, 3.0, 0.0, 0);
            }
            SECTION("Only new entries in map") {
                testing_a.update_derivatives(b.deps(), 1.0);
                test_uncertain(a, 3.0, 0.5, 2);
            }
            SECTION("Pre-existing and new entries in map") {
                // Setup for b's map
                testing_b.update_derivatives(3.0);
                testing_b.update_derivatives(a.deps(), 0.2 / 0.3);
                // Add to derivatives to a
                testing_a.update_derivatives(b.deps(), 1.0);
                test_uncertain(a, 3.0, 1.3, 2);
            }
        }
    }
}
