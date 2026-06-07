#include "catch2/catch_test_macros.hpp"
#include "testing.hpp"
#include <sstream>

using testing::test_affine;

TEMPLATE_TEST_CASE("Affine", "", sigma::AFloat, sigma::ADouble) {}
