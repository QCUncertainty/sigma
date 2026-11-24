"""Mirrors tests/unit_tests/interval/operations/test_exponents.cpp."""

import math
import unittest

import helpers

import sigma


class ExponentsTests:
    testing_t = None
    info = None

    def v(self, value):
        return self.info.to_value(value)

    def test_square_root(self):
        T = self.testing_t
        # sqrt([1, 4]) = [1, 2]
        helpers.assert_interval(self, sigma.sqrt(T(1.0, 4.0)), 1.0, 2.0)
        # sqrt([4, 9]) = [2, 3]
        helpers.assert_interval(self, sigma.sqrt(T(4.0, 9.0)), 2.0, 3.0)

    def test_exponential_function(self):
        T = self.testing_t
        # exp([0, 1]) = [1, e]
        helpers.assert_interval(self, sigma.exp(T(0.0, 1.0)), 1.0, 2.7183)
        # exp([1, 2]) = [e, e^2]
        helpers.assert_interval(self, sigma.exp(T(1.0, 2.0)), 2.7183, 7.3891)

    def test_natural_logarithm(self):
        T = self.testing_t
        # log([1, e]) = [0, 1]
        helpers.assert_interval(self, sigma.log(T(1.0, 2.7183)), 0.0, 1.0)
        # log([1, 1]) = [0, 0]
        helpers.assert_interval(self, sigma.log(T(1.0, 1.0)), 0.0, 0.0)

    def test_power(self):
        T = self.testing_t
        # pow([1, 2], 2) = [1, 4]
        helpers.assert_interval(self, sigma.pow(T(1.0, 2.0), 2), 1.0, 4.0)

        # pow([1, 2], 0.5) = [1, sqrt(2)]
        helpers.assert_interval(
            self, sigma.pow(T(1.0, 2.0), 0.5), 1.0, math.sqrt(2.0)
        )

        # pow([1, 2], -1) = [0.5, 1]
        helpers.assert_interval(self, sigma.pow(T(1.0, 2.0), -1), 0.5, 1.0)

        # Even exponents are not monotonic across an interval spanning 0: the
        # minimum is the interior point x == 0, not either endpoint.
        helpers.assert_interval(self, sigma.pow(T(-1.0, 1.0), 2), 0.0, 1.0)

        # pow([-2, 3], 2) = [0, 9], since |lower| < |upper|
        helpers.assert_interval(self, sigma.pow(T(-2.0, 3.0), 2), 0.0, 9.0)

        # pow([-3, 1], 2) = [0, 9], since |lower| > |upper|
        helpers.assert_interval(self, sigma.pow(T(-3.0, 1.0), 2), 0.0, 9.0)

        # Odd exponents ARE monotonic across all of R
        helpers.assert_interval(self, sigma.pow(T(-3.0, 1.0), 3), -27.0, 1.0)

    def test_outward_rounding(self):
        # log(v), exp(v), and sqrt(v) are almost never representable, so a
        # degenerate argument must NOT give a degenerate result
        for value in (0.3, 0.5, 1.1, 1.5, 2.0, 7.25):
            v = self.v(value)
            with self.subTest(v=v):
                a = self.testing_t(v)
                self.assertEqual(a.width(), 0.0)

                self.assertGreater(sigma.log(a).width(), 0.0)
                self.assertTrue(sigma.log(a).contains(math.log(v)))

                self.assertGreater(sigma.exp(a).width(), 0.0)
                self.assertTrue(sigma.exp(a).contains(math.exp(v)))

                self.assertGreater(sigma.sqrt(a).width(), 0.0)
                self.assertTrue(sigma.sqrt(a).contains(math.sqrt(v)))

                # A fractional power is evaluated as exp(power * log(a))
                self.assertGreater(sigma.pow(a, 0.5).width(), 0.0)
                self.assertTrue(sigma.pow(a, 0.5).contains(math.pow(v, 0.5)))

                # An integer power is repeated multiplication, which may well
                # be exact, so only containment is required of it
                self.assertTrue(sigma.pow(a, 3).contains(math.pow(v, 3)))

    def test_domain_edges(self):
        T = self.testing_t
        # An argument that reaches outside the domain is an error
        self.assertRaises(ValueError, sigma.sqrt, T(-4.0, -1.0))
        self.assertRaises(ValueError, sigma.sqrt, T(-4.0, 9.0))
        self.assertRaises(ValueError, sigma.log, T(-2.0, -1.0))
        self.assertRaises(ValueError, sigma.log, T(-2.0, 3.0))

        # Zero is in the domain of the square root, but not of the logarithm
        helpers.assert_interval(self, sigma.sqrt(T(0.0, 9.0)), 0.0, 3.0)
        self.assertRaises(ValueError, sigma.log, T(0.0, 1.0))

        # A negative base has no real fractional power
        self.assertRaises(ValueError, sigma.pow, T(-1.0, 4.0), 0.5)

        # Zero is fine for a POSITIVE fractional power -- 0^0.5 is 0
        helpers.assert_interval(self, sigma.pow(T(0.0, 4.0), 0.5), 0.0, 2.0)

        # A negative power of a base containing zero divides by zero
        self.assertRaises(ValueError, sigma.pow, T(-1.0, 1.0), -1)
        self.assertRaises(ValueError, sigma.pow, T(0.0, 2.0), -2)
        self.assertRaises(ValueError, sigma.pow, T(0.0, 4.0), -0.5)

        # The empty interval contains nothing out of domain
        self.assertTrue(sigma.sqrt(T()).empty())
        self.assertTrue(sigma.log(T()).empty())
        self.assertTrue(sigma.pow(T(), 0.5).empty())

    def test_power_interval_exponent(self):
        T = self.testing_t
        # A point exponent is forwarded to the scalar overload, so the two
        # must agree exactly
        a = T(1.0, 2.0)
        point_exp = T(2.0, 2.0)
        from_ivl = sigma.pow(a, point_exp)
        from_scalar = sigma.pow(a, 2)
        self.assertEqual(from_ivl.lower(), from_scalar.lower())
        self.assertEqual(from_ivl.upper(), from_scalar.upper())

        # pow([2, 2], [0, 1]) = [1, 2]
        helpers.assert_interval(
            self, sigma.pow(T(2.0, 2.0), T(0.0, 1.0)), 1.0, 2.0
        )

        # Empty base or empty exponent gives the empty interval
        self.assertTrue(sigma.pow(T(), point_exp).empty())
        self.assertTrue(sigma.pow(a, T()).empty())

        # A negative base raised to an integer point exponent is fine
        helpers.assert_interval(
            self, sigma.pow(T(-1.0, 1.0), point_exp), 0.0, 1.0
        )

        # A widened exponent inherits log's domain restriction
        self.assertRaises(ValueError, sigma.pow, T(-1.0, 1.0), T(1.0, 2.0))


class TestIFloat(ExponentsTests, unittest.TestCase):
    testing_t = sigma.IFloat
    info = helpers.FLOAT


class TestIDouble(ExponentsTests, unittest.TestCase):
    testing_t = sigma.IDouble
    info = helpers.DOUBLE
