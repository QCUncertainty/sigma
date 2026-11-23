"""Mirrors tests/unit_tests/triplex/operations/test_basic.cpp."""

import unittest

import helpers

import sigma


class BasicTests:
    testing_t = None
    info = None

    def test_absolute_value(self):
        T = self.testing_t
        pos = T(1.0, 2.0, 3.0)
        neg = T(-3.0, -2.0, -1.0)
        mixed = T(-3.0, -1.0, 2.0)
        for a in (pos, neg, mixed):
            with self.subTest(a=str(a)):
                helpers.assert_result(
                    self,
                    self.info,
                    sigma.abs(a),
                    sigma.abs(a.interval()),
                    abs(a.nominal()),
                )
                helpers.assert_result(
                    self,
                    self.info,
                    sigma.fabs(a),
                    sigma.fabs(a.interval()),
                    abs(a.nominal()),
                )
        # |<-3, -1, 2>| = <0, 1, 3>: the nominal value is |m|, not inferred
        # from the bounds
        self.assertEqual(sigma.abs(mixed), T(0.0, 1.0, 3.0))
        self.assertTrue(sigma.abs(T()).empty())


class TestTPFloat(BasicTests, unittest.TestCase):
    testing_t = sigma.TPFloat
    info = helpers.FLOAT


class TestTPDouble(BasicTests, unittest.TestCase):
    testing_t = sigma.TPDouble
    info = helpers.DOUBLE
