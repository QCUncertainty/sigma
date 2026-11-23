"""Mirrors tests/unit_tests/triplex/operations/test_exponents.cpp."""

import math
import unittest

import helpers

import sigma


class ExponentsTests:
    testing_t = None
    info = None

    def setUp(self):
        self.a = self.testing_t(0.5, 0.8, 2.0)
        self.empty = self.testing_t()

    def check(self, r, bounds, nominal):
        helpers.assert_result(self, self.info, r, bounds, nominal)

    def test_square_root(self):
        a = self.a
        self.check(
            sigma.sqrt(a), sigma.sqrt(a.interval()), math.sqrt(a.nominal())
        )
        self.assertTrue(sigma.sqrt(self.empty).empty())
        negative = self.testing_t(-1.0, 0.5, 1.0)
        self.assertRaises(ValueError, sigma.sqrt, negative)

    def test_exponential_function(self):
        a = self.a
        self.check(
            sigma.exp(a), sigma.exp(a.interval()), math.exp(a.nominal())
        )
        self.assertTrue(sigma.exp(self.empty).empty())

        # exp(<-1, 0, 1>) has nominal value e^0 = 1, nowhere near the median
        # of [1/e, e]
        r = sigma.exp(self.testing_t(-1.0, 0.0, 1.0))
        helpers.assert_triplex(self, r, math.exp(-1.0), 1.0, math.exp(1.0))
        self.assertEqual(r.nominal(), 1.0)
        self.assertAlmostEqual(r.median(), math.cosh(1.0), delta=1.0e-4)

    def test_natural_logarithm(self):
        a = self.a
        self.check(
            sigma.log(a), sigma.log(a.interval()), math.log(a.nominal())
        )
        self.assertTrue(sigma.log(self.empty).empty())
        zero = self.testing_t(0.0, 0.5, 1.0)
        self.assertRaises(ValueError, sigma.log, zero)

    def test_power_with_a_scalar_exponent(self):
        T, a, v = self.testing_t, self.a, self.info.to_value
        m, ivl = a.nominal(), a.interval()
        self.check(sigma.pow(a, 2), sigma.pow(ivl, 2), v(math.pow(m, 2)))
        self.check(sigma.pow(a, -1), sigma.pow(ivl, -1), v(math.pow(m, -1)))
        self.check(sigma.pow(a, 0.5), sigma.pow(ivl, 0.5), v(math.pow(m, 0.5)))
        self.check(sigma.pow(a, 0), sigma.pow(ivl, 0), 1.0)
        self.assertTrue(sigma.pow(self.empty, 2).empty())

        # An even power of an interval straddling zero
        straddle = T(-2.0, -1.0, 3.0)
        self.assertEqual(sigma.pow(straddle, 2), T(0.0, 1.0, 9.0))

        self.assertRaises(ValueError, sigma.pow, straddle, -1)
        self.assertRaises(ValueError, sigma.pow, straddle, 0.5)

    def test_power_with_a_triplex_exponent(self):
        T, a = self.testing_t, self.a
        e = T(1.0, 1.5, 2.0)
        self.check(
            sigma.pow(a, e),
            sigma.pow(a.interval(), e.interval()),
            math.pow(a.nominal(), e.nominal()),
        )
        helpers.assert_invariant(self, sigma.pow(a, e))

        # A point exponent goes through the scalar overload, so a negative
        # base is fine
        negative = T(-2.0, -1.5, -1.0)
        self.assertEqual(sigma.pow(negative, T(2.0)), T(1.0, 2.25, 4.0))

        self.assertTrue(sigma.pow(self.empty, e).empty())
        self.assertTrue(sigma.pow(a, self.empty).empty())
        self.assertRaises(ValueError, sigma.pow, negative, e)


class TestTPFloat(ExponentsTests, unittest.TestCase):
    testing_t = sigma.TPFloat
    info = helpers.FLOAT


class TestTPDouble(ExponentsTests, unittest.TestCase):
    testing_t = sigma.TPDouble
    info = helpers.DOUBLE
