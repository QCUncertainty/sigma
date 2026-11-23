"""Mirrors tests/unit_tests/triplex/test_triplex.cpp.

Only the Triplex<Interval<T>> instantiations are exposed to Python, so the
cases that only apply to Triplex<GeneralInterval<T>> are not mirrored.
"""

import copy
import math
import unittest

import helpers

import sigma


class TriplexTests:
    testing_t = None
    interval_t = None
    other_t = None
    info = None

    def setUp(self):
        T = self.testing_t
        self.defaulted = T()
        self.point = T(2.0)
        self.x = T(1.0, 2.0, 5.0)
        self.y = T(-1.0, 1.0, 3.0)

    def v(self, value):
        return self.info.to_value(value)

    # -- Ctors ----------------------------------------------------------------

    def test_ctor_default(self):
        self.assertTrue(self.defaulted.empty())

    def test_ctor_value(self):
        T = self.testing_t
        self.assertFalse(self.point.empty())
        self.assertEqual(self.point.lower(), 2.0)
        self.assertEqual(self.point.nominal(), 2.0)
        self.assertEqual(self.point.upper(), 2.0)

        self.assertEqual(T(3.0), T(3.0, 3.0, 3.0))

        # There is no finite nominal value for a non-finite value
        self.assertRaises(ValueError, T, math.inf)
        self.assertRaises(ValueError, T, -math.inf)
        self.assertRaises(ValueError, T, math.nan)

    def test_ctor_bounds_and_nominal_value(self):
        T = self.testing_t
        inf = math.inf
        self.assertEqual(self.x.lower(), 1.0)
        self.assertEqual(self.x.nominal(), 2.0)
        self.assertEqual(self.x.upper(), 5.0)

        # The nominal value may sit on a closed bound
        self.assertEqual(T(1.0, 1.0, 5.0).nominal(), 1.0)
        self.assertEqual(T(1.0, 5.0, 5.0).nominal(), 5.0)

        # Unbounded intervals are fine, as long as the nominal value is finite
        self.assertEqual(T(-inf, 0.0, inf).nominal(), 0.0)

        # Nominal value outside the bounds
        self.assertRaises(ValueError, T, 1.0, 0.0, 5.0)
        self.assertRaises(ValueError, T, 1.0, 6.0, 5.0)

        # Nothing is reordered: Interval would sort (5, 1) into [1, 5]
        self.assertRaises(ValueError, T, 5.0, 2.0, 1.0)

        # Non-finite nominal values
        self.assertRaises(ValueError, T, -inf, inf, inf)
        self.assertRaises(ValueError, T, -inf, -inf, inf)
        self.assertRaises(ValueError, T, -inf, math.nan, inf)

    def test_ctor_interval_and_nominal_value(self):
        T, Ivl = self.testing_t, self.interval_t
        self.assertEqual(T(Ivl(1.0, 5.0), 2.0), self.x)
        self.assertRaises(ValueError, T, Ivl(1.0, 5.0), 6.0)
        self.assertRaises(ValueError, T, Ivl(), 0.0)

    def test_no_construction_from_an_interval_alone(self):
        # The counterpart of the C++ STATIC_REQUIREs
        self.assertRaises(TypeError, self.testing_t, self.interval_t(1.0, 5.0))

    def test_copy(self):
        for copier in (copy.copy, copy.deepcopy):
            with self.subTest(copier=copier.__name__):
                c = copier(self.x)
                self.assertIsNot(c, self.x)
                self.assertEqual(c, self.x)

    # -- Accessors ------------------------------------------------------------

    def test_width_and_median_are_those_of_the_bounds(self):
        self.assertEqual(self.x.width(), 4.0)
        self.assertEqual(self.x.median(), 3.0)
        # ...and the median is not the nominal value
        self.assertNotEqual(self.x.median(), self.x.nominal())

    def test_radii(self):
        x = self.x
        self.assertEqual(x.lower_radius(), 1.0)
        self.assertEqual(x.upper_radius(), 3.0)
        self.assertEqual(x.lower_radius() + x.upper_radius(), x.width())
        self.assertEqual(self.point.lower_radius(), 0.0)
        self.assertEqual(self.point.upper_radius(), 0.0)

    def test_interval(self):
        self.assertEqual(self.x.interval(), self.interval_t(1.0, 5.0))
        self.assertTrue(self.defaulted.interval().empty())

    def test_empty_triplex_has_no_values(self):
        d = self.defaulted
        for accessor in (
            d.lower,
            d.nominal,
            d.upper,
            d.width,
            d.median,
            d.lower_radius,
            d.upper_radius,
        ):
            with self.subTest(accessor=accessor.__name__):
                self.assertRaises(ValueError, accessor)

    # -- contains -------------------------------------------------------------

    def test_contains_value(self):
        self.assertTrue(self.x.contains(1.0))
        self.assertTrue(self.x.contains(4.0))
        self.assertTrue(self.x.contains(5.0))
        self.assertFalse(self.x.contains(0.0))
        self.assertFalse(self.defaulted.contains(0.0))

    def test_contains_triplex(self):
        # Only the bounds matter, not the nominal values
        x = self.x
        self.assertTrue(x.contains(self.testing_t(2.0, 4.0, 5.0)))
        self.assertTrue(x.contains(x))
        self.assertTrue(x.contains(self.defaulted))
        self.assertFalse(x.contains(self.y))
        self.assertFalse(self.defaulted.contains(x))

    # -- Arithmetic -----------------------------------------------------------

    def test_negation(self):
        self.assertEqual(-self.x, self.testing_t(-5.0, -2.0, -1.0))
        self.assertTrue((-self.defaulted).empty())

    def test_addition(self):
        T, x, y = self.testing_t, self.x, self.y
        r = x + y
        self.assertEqual(r.interval(), x.interval() + y.interval())
        self.assertEqual(r.nominal(), 3.0)
        self.assertEqual(r, T(0.0, 3.0, 8.0))

        self.assertEqual(x + 1.0, T(2.0, 3.0, 6.0))
        self.assertEqual(1.0 + x, T(2.0, 3.0, 6.0))

        z = copy.copy(x)
        z_id = z
        z += y
        self.assertIs(z, z_id)
        self.assertEqual(z, r)

        # As for intervals, the empty triplex is the additive identity
        self.assertEqual(self.defaulted + x, x)
        self.assertEqual(x + self.defaulted, x)

    def test_subtraction(self):
        T, x, y = self.testing_t, self.x, self.y
        r = x - y
        self.assertEqual(r.interval(), x.interval() - y.interval())
        self.assertEqual(r, T(-2.0, 1.0, 6.0))

        self.assertEqual(x - 1.0, T(0.0, 1.0, 4.0))
        self.assertEqual(1.0 - x, T(-4.0, -1.0, 0.0))

        z = copy.copy(x)
        z_id = z
        z -= y
        self.assertIs(z, z_id)
        self.assertEqual(z, r)

        self.assertEqual(self.defaulted - x, -x)
        self.assertEqual(x - self.defaulted, x)

    def test_multiplication(self):
        T, x, y = self.testing_t, self.x, self.y
        r = x * y
        self.assertEqual(r.interval(), x.interval() * y.interval())
        self.assertEqual(r, T(-5.0, 2.0, 15.0))

        self.assertEqual(x * 2.0, T(2.0, 4.0, 10.0))
        self.assertEqual(2.0 * x, T(2.0, 4.0, 10.0))

        z = copy.copy(x)
        z_id = z
        z *= y
        self.assertIs(z, z_id)
        self.assertEqual(z, r)

        self.assertTrue((self.defaulted * x).empty())
        self.assertTrue((x * self.defaulted).empty())

    def test_division(self):
        T, x, y = self.testing_t, self.x, self.y
        d = T(2.0, 4.0, 8.0)
        r = x / d
        self.assertEqual(r.interval(), x.interval() / d.interval())
        self.assertEqual(r.nominal(), 0.5)

        self.assertEqual(x / 2.0, T(0.5, 1.0, 2.5))
        self.assertEqual((8.0 / d).nominal(), 2.0)

        z = copy.copy(x)
        z_id = z
        z /= d
        self.assertIs(z, z_id)
        self.assertEqual(z, r)

        self.assertTrue((self.defaulted / x).empty())
        self.assertTrue((x / self.defaulted).empty())

        # A divisor containing zero is an error, even with a nonzero nominal
        # value
        with self.assertRaises(ValueError):
            x / y
        with self.assertRaises(ValueError):
            x / 0.0
        # ...and the strong guarantee holds
        w = copy.copy(x)
        with self.assertRaises(ValueError):
            w /= y
        self.assertEqual(w, x)

    def test_self_assignment_forms(self):
        z = copy.copy(self.y)
        z *= z
        self.assertEqual(z, self.testing_t(-3.0, 1.0, 9.0))

    def test_nominal_value_not_affected_by_dependency_problem(self):
        # <-1, 1, 3> * <-1, 1, 3> = <-3, 1, 9>. The median of the interval
        # result, 3, drifts; the nominal value does not.
        sq = self.y * self.y
        self.assertEqual(sq, self.testing_t(-3.0, 1.0, 9.0))
        self.assertEqual(sq.median(), 3.0)

        # X - X has the needlessly wide bounds of the interval case, but its
        # nominal value is exactly zero
        self.assertEqual(self.y - self.y, self.testing_t(-4.0, 0.0, 4.0))

    def test_comparability_with_plain_floating_point(self):
        # The nominal value of a triplex computation must be the same
        # computation in plain value_t on the nominal values. The reference
        # rounds to value_t after every operation, as C++ does. That makes it
        # bit for bit for double; for float, the math functions are double
        # precision, so the reference is only close (see assert_nominal).
        T, c = self.testing_t, self.v
        a = T(0.5, 0.7, 0.9)
        b = T(1.1, 1.3, 2.0)

        r = (
            sigma.exp(a) * sigma.sin(b) / (a + b)
            - sigma.sqrt(a * b)
            + sigma.pow(a, 3)
            + sigma.log(b)
            - sigma.atan(a) * sigma.cosh(b)
            + 2.0 * sigma.tanh(a)
        )

        am = a.nominal()
        bm = b.nominal()
        t1 = c(math.exp(am))
        t2 = c(math.sin(bm))
        t3 = c(t1 * t2)
        t4 = c(am + bm)
        t5 = c(t3 / t4)
        t6 = c(am * bm)
        t7 = c(math.sqrt(t6))
        t8 = c(t5 - t7)
        t9 = c(math.pow(am, 3))
        t10 = c(t8 + t9)
        t11 = c(math.log(bm))
        t12 = c(t10 + t11)
        t13 = c(math.atan(am))
        t14 = c(math.cosh(bm))
        t15 = c(t13 * t14)
        t16 = c(t12 - t15)
        t17 = c(math.tanh(am))
        t18 = c(2.0 * t17)
        expected = c(t16 + t18)

        helpers.assert_nominal(
            self, self.info, r.nominal(), expected, rel_tol=1.0e-5
        )
        helpers.assert_invariant(self, r)
        # The bounds enclose the nominal value but are not centered on it
        self.assertNotEqual(r.median(), r.nominal())

    def test_iterating_on_the_nominal_value_iterates_plain_floating_point(
        self,
    ):
        # The fixed-point iteration x <- cos(x). A procedure that fed the
        # median back in would drift away from the plain result.
        t = self.testing_t(0.5, 0.6, 1.0)
        plain = self.v(0.6)
        for i in range(50):
            t = sigma.cos(t)
            plain = self.v(math.cos(plain))
            with self.subTest(i=i):
                helpers.assert_nominal(
                    self, self.info, t.nominal(), plain, rel_tol=1.0e-5
                )
                helpers.assert_invariant(self, t)

    def test_non_finite_nominal_values(self):
        # Plain floating point overflows here while the interval does not
        # fail. The overflowed nominal value is kept, and is not a member.
        huge = self.testing_t(self.info.max_value)
        r = huge * 2.0
        self.assertEqual(r.nominal(), math.inf)
        self.assertTrue(math.isinf(r.upper()))
        self.assertFalse(r.contains(r.nominal()))

        # Further arithmetic carries the plain floating point result along
        # without raising, e.g., inf - inf is NaN
        self.assertTrue(math.isnan((r - r).nominal()))

    # -- Comparisons ----------------------------------------------------------

    def test_equality(self):
        T, x = self.testing_t, self.x
        self.assertEqual(x, T(1.0, 2.0, 5.0))
        self.assertEqual(self.defaulted, T())
        # Same bounds, different nominal value
        self.assertNotEqual(x, T(1.0, 3.0, 5.0))
        # Same nominal value, different bounds
        self.assertNotEqual(x, T(1.0, 2.0, 6.0))
        self.assertNotEqual(x, self.defaulted)
        self.assertNotEqual(self.defaulted, x)

    def test_different_value_types(self):
        other = self.other_t(1.0, 2.0, 5.0)
        self.assertTrue(self.x != other)
        self.assertFalse(self.x == other)

    def test_ordering_is_by_nominal_value(self):
        T = self.testing_t
        # wide has the larger median but the smaller nominal value
        wide = T(0.0, 1.0, 10.0)
        narrow = T(1.5, 2.0, 2.5)
        self.assertGreater(wide.median(), narrow.median())
        self.assertTrue(wide < narrow)
        self.assertTrue(narrow > wide)
        self.assertTrue(wide <= narrow)
        self.assertTrue(narrow >= wide)
        self.assertFalse(narrow < wide)
        self.assertTrue(wide <= wide)
        self.assertTrue(wide >= wide)

        self.assertTrue(self.defaulted < wide)
        self.assertFalse(wide < self.defaulted)
        self.assertFalse(self.defaulted < self.defaulted)

    def test_ties_in_nominal_value_are_broken_by_the_bounds(self):
        T = self.testing_t
        narrow = T(-1.0, 0.0, 1.0)
        wide_below = T(-2.0, 0.0, 1.0)
        wide_above = T(-1.0, 0.0, 2.0)
        self.assertTrue(wide_below < narrow)
        self.assertFalse(narrow < wide_below)
        self.assertTrue(narrow < wide_above)
        self.assertFalse(wide_above < narrow)

        # ...but the nominal value still decides first
        self.assertTrue(narrow < T(-5.0, 0.5, 1.0))

        self.assertEqual(len({narrow, wide_below, wide_above, narrow}), 3)

    def test_equivalent_under_lt_exactly_when_eq(self):
        T = self.testing_t
        values = [
            self.defaulted,
            self.x,
            self.y,
            T(-1.0, 0.0, 1.0),
            T(-2.0, 0.0, 1.0),
            T(-1.0, 0.0, 2.0),
            T(0.0),
            T(1.0, 3.0, 5.0),
        ]
        for a in values:
            for b in values:
                with self.subTest(a=str(a), b=str(b)):
                    equivalent = not (a < b) and not (b < a)
                    self.assertEqual(equivalent, a == b)
                    self.assertEqual(a <= b, a < b or a == b)
                    self.assertEqual(a >= b, b < a or a == b)

    # -- Hashing and printing -------------------------------------------------

    def test_hashing(self):
        x = self.x
        self.assertEqual(hash(self.defaulted), 0)
        self.assertEqual(hash(x), hash(copy.copy(x)))
        self.assertNotEqual(hash(x), hash(self.testing_t(1.0, 3.0, 5.0)))
        self.assertEqual(len({x, self.y, x}), 2)

    def test_printing(self):
        self.assertEqual(str(self.x), "[1, 2, 5]")
        self.assertEqual(repr(self.x), "[1, 2, 5]")
        self.assertEqual(str(self.defaulted), "∅")


class TestTPFloat(TriplexTests, unittest.TestCase):
    testing_t = sigma.TPFloat
    interval_t = sigma.IFloat
    other_t = sigma.TPDouble
    info = helpers.FLOAT


class TestTPDouble(TriplexTests, unittest.TestCase):
    testing_t = sigma.TPDouble
    interval_t = sigma.IDouble
    other_t = sigma.TPFloat
    info = helpers.DOUBLE
