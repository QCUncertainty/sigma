"""Mirrors tests/unit_tests/interval/operations/test_hyperbolic.cpp."""

import math
import unittest

import helpers

import sigma


class HyperbolicTests:
    testing_t = None
    info = None

    def test_hyperbolic_sine(self):
        T = self.testing_t
        helpers.assert_interval(
            self, sigma.sinh(T(0.0, 1.0)), 0.0, math.sinh(1.0)
        )
        # sinh is increasing across all of R
        helpers.assert_interval(
            self, sigma.sinh(T(-1.0, 2.0)), math.sinh(-1.0), math.sinh(2.0)
        )

    def test_hyperbolic_cosine(self):
        T = self.testing_t
        # The minimum is the interior point 0, not either bound
        helpers.assert_interval(
            self, sigma.cosh(T(-1.0, 2.0)), 1.0, math.cosh(2.0)
        )
        # The maximum comes from whichever bound is farther from zero
        helpers.assert_interval(
            self, sigma.cosh(T(-3.0, 1.0)), 1.0, math.cosh(3.0)
        )
        # cosh is decreasing on the negatives, so the bounds exchange places
        helpers.assert_interval(
            self, sigma.cosh(T(-2.0, -1.0)), math.cosh(1.0), math.cosh(2.0)
        )

    def test_hyperbolic_tangent(self):
        helpers.assert_interval(
            self, sigma.tanh(self.testing_t(0.0, 1.0)), 0.0, math.tanh(1.0)
        )

    def test_inverse_hyperbolic_sine(self):
        helpers.assert_interval(
            self, sigma.asinh(self.testing_t(0.0, 1.0)), 0.0, math.asinh(1.0)
        )

    def test_inverse_hyperbolic_cosine(self):
        T = self.testing_t
        helpers.assert_interval(
            self, sigma.acosh(T(1.0, 2.0)), 0.0, math.acosh(2.0)
        )
        # Undefined below one
        self.assertRaises(ValueError, sigma.acosh, T(0.0, 2.0))
        self.assertRaises(ValueError, sigma.acosh, T(0.0, 0.5))

    def test_inverse_hyperbolic_tangent(self):
        T = self.testing_t
        helpers.assert_interval(
            self, sigma.atanh(T(0.0, 0.5)), 0.0, math.atanh(0.5)
        )
        # The domain is open, so a CLOSED bound at plus or minus one is an
        # error
        self.assertRaises(ValueError, sigma.atanh, T(-1.0, 0.0))
        self.assertRaises(ValueError, sigma.atanh, T(0.0, 1.0))
        self.assertRaises(ValueError, sigma.atanh, T(2.0, 3.0))

    def test_outward_rounding(self):
        # The hyperbolic functions of a representable value are almost never
        # representable, so a degenerate argument must NOT give a degenerate
        # result
        functions = [
            (sigma.sinh, math.sinh),
            (sigma.cosh, math.cosh),
            (sigma.tanh, math.tanh),
            (sigma.asinh, math.asinh),
            (sigma.atanh, math.atanh),
        ]
        for value in (0.0, 0.25, 0.5):
            v = self.info.to_value(value)
            a = self.testing_t(v)
            self.assertEqual(a.width(), 0.0)
            for fxn, ref in functions:
                with self.subTest(v=v, fxn=ref.__name__):
                    self.assertGreater(fxn(a).width(), 0.0)
                    self.assertTrue(fxn(a).contains(ref(v)))

            shifted = self.info.to_value(v + 1.5)
            b = self.testing_t(shifted)
            with self.subTest(v=v, fxn="acosh"):
                self.assertGreater(sigma.acosh(b).width(), 0.0)
                self.assertTrue(sigma.acosh(b).contains(math.acosh(shifted)))

    def test_empty(self):
        a = self.testing_t()
        for fxn in (
            sigma.sinh,
            sigma.cosh,
            sigma.tanh,
            sigma.asinh,
            sigma.acosh,
            sigma.atanh,
        ):
            with self.subTest(fxn=fxn.__name__):
                self.assertTrue(fxn(a).empty())


class TestIFloat(HyperbolicTests, unittest.TestCase):
    testing_t = sigma.IFloat
    info = helpers.FLOAT


class TestIDouble(HyperbolicTests, unittest.TestCase):
    testing_t = sigma.IDouble
    info = helpers.DOUBLE
