"""Mirrors tests/unit_tests/interval/operations/test_basic.cpp."""

import unittest

import helpers

import sigma


class BasicTests:
    testing_t = None

    def test_absolute_value(self):
        T = self.testing_t
        cases = [
            ("All positive", T(1.0, 3.0), 1.0, 3.0),
            ("All negative", T(-3.0, -1.0), 1.0, 3.0),
            ("Straddles zero", T(-2.0, 3.0), 0.0, 3.0),
        ]
        for name, a, lo, hi in cases:
            with self.subTest(name):
                helpers.assert_interval(self, sigma.abs(a), lo, hi)
                helpers.assert_interval(self, sigma.fabs(a), lo, hi)


class TestIFloat(BasicTests, unittest.TestCase):
    testing_t = sigma.IFloat


class TestIDouble(BasicTests, unittest.TestCase):
    testing_t = sigma.IDouble
