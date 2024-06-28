#include <catch2/catch.hpp>
#include <sigma/sigma.hpp>

// TODO: Remove this
void debug_print(const sigma::UDouble& x) {
    std::cout << x << std::endl;
    for(const auto& [dep, contrib] : x.deps()) {
        std::cout << "| " << *dep.get() << " : " << contrib << std::endl;
    }
    std::cout << std::endl;
}

TEST_CASE("Uncertain") {
    SECTION("Constructors") {
        SECTION("Default") {
            auto value = sigma::UDouble();
            REQUIRE(value.mean() == 0.0);
            REQUIRE(value.std() == 0.0);
        }
        SECTION("With Values") {
            auto value = sigma::UDouble(1.0, 0.1);
            REQUIRE(value.mean() == 1.0);
            REQUIRE(value.std() == 0.1);
        }
    }
    SECTION("Operations") {
        auto a = sigma::UDouble(1.0, 0.1);
        auto b = sigma::UDouble(2.0, 0.2);
        auto c = sigma::UDouble(2.0, 0.2); // Same value, different variable
        auto d = sigma::UDouble(3.0, 0.3);

        SECTION("Addition") {
            auto x = a + b;
            debug_print(x);
            REQUIRE(x.mean() == 3.0);
            REQUIRE(x.std() == Approx(0.2236).margin(1.0e-4));

            auto y = x + d;
            debug_print(y);
            REQUIRE(y.mean() == 6.0);
            REQUIRE(y.std() == Approx(0.3742).margin(1.0e-4));
        }
        SECTION("Subtraction") {
            auto x = a - b;
            debug_print(x);
            REQUIRE(x.mean() == -1.0);
            REQUIRE(x.std() == Approx(0.2236).margin(1.0e-4));

            auto y = x - d;
            debug_print(y);
            REQUIRE(y.mean() == -4.0);
            REQUIRE(y.std() == Approx(0.3742).margin(1.0e-4));
        }
        SECTION("Multiplication") {
            auto x = a * b;
            debug_print(x);
            REQUIRE(x.mean() == 2.0);
            REQUIRE(x.std() == Approx(0.2828).margin(1.0e-4));

            auto y = x * d;
            debug_print(y);
            REQUIRE(y.mean() == 6.0);
            REQUIRE(y.std() == Approx(1.0392).margin(1.0e-4));
        }
        SECTION("Division") {
            auto x = a / b;
            debug_print(x);
            REQUIRE(x.mean() == 0.5);
            REQUIRE(x.std() == Approx(0.0707).margin(1.0e-4));

            auto y = x / d;
            debug_print(y);
            REQUIRE(y.mean() == Approx(0.1667).margin(1.0e-4));
            REQUIRE(y.std() == Approx(0.0289).margin(1.0e-4));
        }
        SECTION("Mixing Operations") {
            auto x = a + b + (a * b);
            debug_print(x);
            REQUIRE(x.mean() == 5.0);
            REQUIRE(x.std() == Approx(0.5000).margin(1.0e-4));

            auto y = (d * b - a) / b;
            debug_print(y);
            REQUIRE(y.mean() == 2.5);
            REQUIRE(y.std() == Approx(0.3082).margin(1.0e-4));

            auto z = (d * b - a) / c;
            debug_print(z);
            REQUIRE(z.mean() == 2.5);
            REQUIRE(z.std() == Approx(0.4950).margin(1.0e-4));
        }
    }
}