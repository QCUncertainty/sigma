#include "catch2/catch_test_macros.hpp"
#include <map>
#include <sigma/taylor/monomial.hpp>
#include <sstream>
#include <unordered_set>

using monomial_t = sigma::Monomial;

TEST_CASE("Monomial") {
    auto v0 = monomial_t::make_variable();
    auto v1 = monomial_t::make_variable();

    SECTION("Constructors") {
        SECTION("Default") {
            monomial_t m;
            REQUIRE(m.empty());
            REQUIRE(m.degree() == 0);
            REQUIRE(m.exponents().empty());
            REQUIRE(m.exponent(v0) == 0);
            REQUIRE(m.exponent(v1) == 0);
        }

        SECTION("Single Variable") {
            monomial_t m(v0);
            REQUIRE_FALSE(m.empty());
            REQUIRE(m.degree() == 1);
            REQUIRE(m.exponent(v0) == 1);
            REQUIRE(m.exponent(v1) == 0);
        }

        SECTION("Single Variable with Exponent") {
            monomial_t m(v0, 3);
            REQUIRE(m.degree() == 3);
            REQUIRE(m.exponent(v0) == 3);
            REQUIRE(m.exponent(v1) == 0);
        }

        SECTION("Single Variable, Zero Exponent Throws") {
            REQUIRE_THROWS_AS(monomial_t(v0, 0), std::invalid_argument);
        }

        SECTION("From Exponent Map") {
            monomial_t m(monomial_t::exponents_t{{v0, 2}, {v1, 5}});
            REQUIRE(m.degree() == 7);
            REQUIRE(m.exponent(v0) == 2);
            REQUIRE(m.exponent(v1) == 5);
        }

        SECTION("From Exponent Map, Zero Entries Are Dropped") {
            monomial_t m(monomial_t::exponents_t{{v0, 0}, {v1, 4}});
            REQUIRE(m.degree() == 4);
            REQUIRE(m.exponent(v0) == 0);
            REQUIRE(m.exponents().count(v0) == 0);
            REQUIRE(m.exponents().count(v1) == 1);
        }

        SECTION("Copy Constructor") {
            monomial_t m(v0, 2);
            monomial_t copy(m);
            REQUIRE(copy == m);
        }

        SECTION("Move Constructor") {
            monomial_t m(v0, 2);
            monomial_t moved(std::move(m));
            REQUIRE(moved.exponent(v0) == 2);
        }

        SECTION("Copy Assignment") {
            monomial_t m(v0, 2);
            monomial_t copy;
            copy = m;
            REQUIRE(copy == m);
        }

        SECTION("Move Assignment") {
            monomial_t m(v0, 2);
            monomial_t moved;
            moved = std::move(m);
            REQUIRE(moved.exponent(v0) == 2);
        }
    }

    SECTION("degree") {
        monomial_t constant;
        REQUIRE(constant.degree() == 0);

        monomial_t single(v0, 3);
        REQUIRE(single.degree() == 3);

        monomial_t multi(monomial_t::exponents_t{{v0, 2}, {v1, 3}});
        REQUIRE(multi.degree() == 5);
    }

    SECTION("empty") {
        REQUIRE(monomial_t{}.empty());
        REQUIRE_FALSE(monomial_t(v0).empty());
    }

    SECTION("exponent") {
        monomial_t m(v0, 2);
        REQUIRE(m.exponent(v0) == 2);
        REQUIRE(m.exponent(v1) == 0); // variable not present
    }

    SECTION("operator*=/operator*") {
        SECTION("Shared Variable: Exponents Add") {
            monomial_t x(v0, 2);
            monomial_t y(v0, 3);
            auto z = x * y;
            REQUIRE(z.exponent(v0) == 5);
            REQUIRE(z.degree() == 5);
        }

        SECTION("Disjoint Variables: Union of Entries") {
            monomial_t x(v0, 2);
            monomial_t y(v1, 3);
            auto z = x * y;
            REQUIRE(z.exponent(v0) == 2);
            REQUIRE(z.exponent(v1) == 3);
            REQUIRE(z.degree() == 5);
        }

        SECTION("Multiplying by the Constant Monomial is a No-op") {
            monomial_t x(v0, 2);
            monomial_t one;
            REQUIRE(x * one == x);
            REQUIRE(one * x == x);
        }

        SECTION("operator*= Mutates *this") {
            monomial_t x(v0, 2);
            monomial_t y(v1, 1);
            x *= y;
            REQUIRE(x.exponent(v0) == 2);
            REQUIRE(x.exponent(v1) == 1);
        }
    }

    SECTION("Comparison Operators") {
        monomial_t x(v0, 2);
        monomial_t same_x(v0, 2);
        monomial_t y(v0, 3);

        REQUIRE(x == same_x);
        REQUIRE_FALSE(x == y);
        REQUIRE(x != y);
        REQUIRE_FALSE(x != same_x);
    }

    SECTION("operator< as a std::map key") {
        monomial_t constant;
        monomial_t x(v0, 1);
        monomial_t y(v0, 2);

        std::map<monomial_t, int> coeffs;
        coeffs[constant] = 0;
        coeffs[x]        = 1;
        coeffs[y]        = 2;

        REQUIRE(coeffs.size() == 3);
        REQUIRE(coeffs.at(constant) == 0);
        REQUIRE(coeffs.at(x) == 1);
        REQUIRE(coeffs.at(y) == 2);
    }

    SECTION("std::hash as a std::unordered_set key") {
        monomial_t x(v0, 2);
        monomial_t same_x(v0, 2);
        monomial_t z(v1, 2);

        std::unordered_set<monomial_t> set;
        set.insert(x);
        set.insert(same_x); // duplicate, should not increase size
        set.insert(z);

        REQUIRE(set.size() == 2);
        REQUIRE(set.count(x) == 1);
        REQUIRE(set.count(z) == 1);
    }

    SECTION("Stream Insertion") {
        SECTION("Constant Monomial") {
            std::stringstream ss;
            ss << monomial_t{};
            REQUIRE(ss.str() == "1");
        }

        SECTION("Non-constant Monomial") {
            monomial_t m(v0, 2);
            std::stringstream ss;
            ss << m;
            REQUIRE(ss.str() == m.print());
            REQUIRE_FALSE(ss.str().empty());
        }
    }
}
