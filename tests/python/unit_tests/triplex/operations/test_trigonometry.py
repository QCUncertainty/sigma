"""Mirrors tests/unit_tests/triplex/operations/test_trigonometry.cpp."""

import math
import unittest

import helpers

import sigma


class TrigonometryTests:
    testing_t = None
    info = None

    def setUp(self):
        self.a = self.testing_t(0.2, 0.3, 0.7)
        self.empty = self.testing_t()

    def check(self, fxn, ref):
        a = self.a
        helpers.assert_result(
            self, self.info, fxn(a), fxn(a.interval()), ref(a.nominal())
        )
        self.assertTrue(fxn(self.empty).empty())

    def test_sine(self):
        self.check(sigma.sin, math.sin)

    def test_cosine(self):
        self.check(sigma.cos, math.cos)

    def test_tangent(self):
        self.check(sigma.tan, math.tan)
        # [1, 2] contains pi/2, even though the nominal value does not
        pole = self.testing_t(1.0, 1.2, 2.0)
        self.assertRaises(ValueError, sigma.tan, pole)

    def test_arcsine(self):
        self.check(sigma.asin, math.asin)
        out = self.testing_t(0.5, 0.9, 1.5)
        self.assertRaises(ValueError, sigma.asin, out)

    def test_arccosine(self):
        self.check(sigma.acos, math.acos)
        out = self.testing_t(0.5, 0.9, 1.5)
        self.assertRaises(ValueError, sigma.acos, out)

    def test_arctangent(self):
        self.check(sigma.atan, math.atan)


class TestTPFloat(TrigonometryTests, unittest.TestCase):
    testing_t = sigma.TPFloat
    info = helpers.FLOAT


class TestTPDouble(TrigonometryTests, unittest.TestCase):
    testing_t = sigma.TPDouble
    info = helpers.DOUBLE
