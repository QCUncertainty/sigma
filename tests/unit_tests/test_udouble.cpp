#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>

TEST_CASE("UDouble") {
    SECTION("Constructors") {
        SECTION("Default") { 
            auto value = sigma::UDouble(); 
            std::cout << value << std::endl;
        }
        SECTION("With Values") { 
            auto value = sigma::UDouble(1.0, 0.1); 
            REQUIRE(value.nominal == 1.0);
            REQUIRE(value.stddev == 0.1);
        }
    }
}