"""Mirrors tests/unit_tests/interval/operations/test_trigonometry.cpp."""

import math
import unittest

import helpers

import sigma


class TrigonometryTests:
    testing_t = None
    info = None

    def setUp(self):
        v = self.info.to_value
        self.pi = v(3.14159265358979323846)
        self.half_pi = v(self.pi / 2.0)

    def v(self, value):
        return self.info.to_value(value)

    def test_sine(self):
        T, v, pi, half_pi = self.testing_t, self.v, self.pi, self.half_pi
        # sin([0, pi/2]) = [0, 1]
        helpers.assert_interval(self, sigma.sin(T(0.0, half_pi)), 0.0, 1.0)
        # An interval spanning a peak encloses the peak, rather than being
        # the sine of the bounds: sin([0, 2 pi]) = [-1, 1], NOT [0, 0]
        helpers.assert_interval(
            self, sigma.sin(T(0.0, v(2.0 * pi))), -1.0, 1.0
        )
        # sin([pi, 3 pi / 2]) = [-1, 0]
        helpers.assert_interval(
            self, sigma.sin(T(pi, v(3.0 * half_pi))), -1.0, 0.0
        )

    def test_cosine(self):
        T, v, pi = self.testing_t, self.v, self.pi
        # cos([0, pi]) = [-1, 1]
        helpers.assert_interval(self, sigma.cos(T(0.0, pi)), -1.0, 1.0)
        # The maximum of cos([-0.1, 0.1]) is the interior point 0
        helpers.assert_interval(
            self, sigma.cos(T(-0.1, 0.1)), math.cos(0.1), 1.0
        )
        # cos([pi, 2 pi]) = [-1, 1]
        helpers.assert_interval(self, sigma.cos(T(pi, v(2.0 * pi))), -1.0, 1.0)

    def test_tangent(self):
        T, v, pi = self.testing_t, self.v, self.pi
        # tan([0, pi/4]) = [0, 1]
        helpers.assert_interval(self, sigma.tan(T(0.0, v(pi / 4.0))), 0.0, 1.0)
        # The tangent is undefined at pi/2
        self.assertRaises(ValueError, sigma.tan, T(1.0, 2.0))
        # An argument at least a period wide contains a pole
        self.assertRaises(ValueError, sigma.tan, T(0.0, v(2.0 * pi)))
        # A branch other than the one around zero is fine:
        # tan([pi, 5 pi / 4]) = [0, 1]
        helpers.assert_interval(
            self, sigma.tan(T(pi, v(v(5.0 * pi) / 4.0))), 0.0, 1.0
        )
        # Approaching the pole without reaching it is fine too
        a = sigma.tan(T(1.0, 1.57))
        self.assertLess(a.lower(), a.upper())
        self.assertTrue(a.contains(math.tan(v(1.57))))

    def test_arcsine(self):
        T, half_pi = self.testing_t, self.half_pi
        helpers.assert_interval(self, sigma.asin(T(0.0, 1.0)), 0.0, half_pi)
        # Undefined outside [-1, 1]
        self.assertRaises(ValueError, sigma.asin, T(-2.0, 2.0))
        self.assertRaises(ValueError, sigma.asin, T(-3.0, -2.0))
        # The ends of the domain are themselves in it
        helpers.assert_interval(
            self, sigma.asin(T(-1.0, 1.0)), -half_pi, half_pi
        )

    def test_arccosine(self):
        T, pi, half_pi = self.testing_t, self.pi, self.half_pi
        helpers.assert_interval(self, sigma.acos(T(-1.0, 1.0)), 0.0, pi)
        # Arccosine is decreasing, so the bounds exchange places
        helpers.assert_interval(self, sigma.acos(T(0.0, 1.0)), 0.0, half_pi)
        self.assertRaises(ValueError, sigma.acos, T(2.0, 3.0))
        self.assertRaises(ValueError, sigma.acos, T(-1.0, 2.0))

    def test_arctangent(self):
        helpers.assert_interval(
            self,
            sigma.atan(self.testing_t(0.0, 1.0)),
            0.0,
            self.v(self.pi / 4.0),
        )

    def test_outward_rounding(self):
        # The trigonometric functions of a representable value are almost
        # never representable, so a degenerate argument must NOT give a
        # degenerate result
        functions = [
            (sigma.sin, math.sin),
            (sigma.cos, math.cos),
            (sigma.tan, math.tan),
            (sigma.asin, math.asin),
            (sigma.acos, math.acos),
            (sigma.atan, math.atan),
        ]
        for value in (0.0, 0.25, 0.5, 1.0):
            v = self.v(value)
            a = self.testing_t(v)
            self.assertEqual(a.width(), 0.0)
            for fxn, ref in functions:
                with self.subTest(v=v, fxn=ref.__name__):
                    self.assertGreater(fxn(a).width(), 0.0)
                    self.assertTrue(fxn(a).contains(ref(v)))

    def test_empty(self):
        a = self.testing_t()
        for fxn in (
            sigma.sin,
            sigma.cos,
            sigma.tan,
            sigma.asin,
            sigma.acos,
            sigma.atan,
        ):
            with self.subTest(fxn=fxn.__name__):
                self.assertTrue(fxn(a).empty())


class TestIFloat(TrigonometryTests, unittest.TestCase):
    testing_t = sigma.IFloat
    info = helpers.FLOAT


class TestIDouble(TrigonometryTests, unittest.TestCase):
    testing_t = sigma.IDouble
    info = helpers.DOUBLE
