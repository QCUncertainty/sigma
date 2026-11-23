"""Mirrors tests/unit_tests/triplex/operations/test_hyperbolic.cpp."""

import math
import unittest

import helpers

import sigma


class HyperbolicTests:
    testing_t = None
    info = None

    def setUp(self):
        self.a = self.testing_t(0.2, 0.3, 0.7)
        self.empty = self.testing_t()

    def check(self, fxn, ref, a):
        helpers.assert_result(
            self, self.info, fxn(a), fxn(a.interval()), ref(a.nominal())
        )
        self.assertTrue(fxn(self.empty).empty())

    def test_hyperbolic_sine(self):
        self.check(sigma.sinh, math.sinh, self.a)

    def test_hyperbolic_cosine(self):
        self.check(sigma.cosh, math.cosh, self.a)

    def test_hyperbolic_tangent(self):
        self.check(sigma.tanh, math.tanh, self.a)

    def test_inverse_hyperbolic_sine(self):
        self.check(sigma.asinh, math.asinh, self.a)

    def test_inverse_hyperbolic_cosine(self):
        self.check(sigma.acosh, math.acosh, self.testing_t(1.5, 2.0, 4.0))
        self.assertRaises(ValueError, sigma.acosh, self.a)

    def test_inverse_hyperbolic_tangent(self):
        self.check(sigma.atanh, math.atanh, self.a)
        out = self.testing_t(-1.0, 0.0, 0.5)
        self.assertRaises(ValueError, sigma.atanh, out)


class TestTPFloat(HyperbolicTests, unittest.TestCase):
    testing_t = sigma.TPFloat
    info = helpers.FLOAT


class TestTPDouble(HyperbolicTests, unittest.TestCase):
    testing_t = sigma.TPDouble
    info = helpers.DOUBLE
