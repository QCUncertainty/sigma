"""Mirrors tests/unit_tests/interval/test_interval.cpp."""

import copy
import math
import unittest

import helpers

import sigma


class IntervalTests:
    """The body of the test; subclasses pick the instantiation."""

    testing_t = None
    info = None

    def setUp(self):
        self.empty = self.testing_t()
        self.one_two = self.testing_t(1.0, 2.0)

    def v(self, value):
        """The counterpart of C++'s ``value_t{value}``."""
        return self.info.to_value(value)

    # -- Constructors ---------------------------------------------------------

    def test_ctor_default(self):
        self.assertTrue(self.empty.empty())

    def test_ctor_value(self):
        one = self.testing_t(1.0)
        helpers.assert_interval(self, one, 1.0, 1.0)
        self.assertFalse(one.empty())

    def test_ctor_lower_and_upper(self):
        helpers.assert_interval(self, self.one_two, 1.0, 2.0)
        self.assertFalse(self.one_two.empty())

    def test_copy(self):
        # Python has no move semantics or assignment operator, so the C++
        # move and assignment sections have no counterpart here
        for copier in (copy.copy, copy.deepcopy):
            with self.subTest(copier=copier.__name__):
                copy_empty = copier(self.empty)
                self.assertIsNot(copy_empty, self.empty)
                self.assertEqual(copy_empty, self.empty)
                self.assertTrue(copy_empty.empty())

                copy_one_two = copier(self.one_two)
                self.assertIsNot(copy_one_two, self.one_two)
                helpers.assert_interval(self, copy_one_two, 1.0, 2.0)
                self.assertEqual(copy_one_two, self.one_two)

    # -- Accessors ------------------------------------------------------------

    def test_width(self):
        self.assertEqual(self.one_two.width(), 1.0)
        self.assertEqual(self.testing_t(1.0, 3.0).width(), 2.0)
        self.assertEqual(self.testing_t(1.0, 1.0).width(), 0.0)
        self.assertEqual(self.testing_t(-1.2, 0.0).width(), self.v(1.2))
        self.assertRaises(ValueError, self.empty.width)

    def test_median(self):
        self.assertEqual(self.one_two.median(), 1.5)
        self.assertRaises(ValueError, self.empty.median)

    def test_radius(self):
        self.assertEqual(self.one_two.radius(), 0.5)
        self.assertRaises(ValueError, self.empty.radius)

    def test_empty(self):
        self.assertTrue(self.empty.empty())
        self.assertFalse(self.one_two.empty())

    def test_lower(self):
        self.assertEqual(self.one_two.lower(), 1.0)
        self.assertRaises(ValueError, self.empty.lower)

    def test_upper(self):
        self.assertEqual(self.one_two.upper(), 2.0)
        self.assertRaises(ValueError, self.empty.upper)

    def _epsilons(self):
        nextafter = self.info.nextafter
        lo, hi = self.one_two.lower(), self.one_two.upper()
        return (
            nextafter(lo, 0.0),
            nextafter(lo, 3.0),
            nextafter(hi, 0.0),
            nextafter(hi, 3.0),
        )

    def test_contains_value(self):
        self.assertFalse(self.empty.contains(0.0))

        one_m_eps, one_p_eps, two_m_eps, two_p_eps = self._epsilons()

        one = self.testing_t(1.0)
        self.assertFalse(one.contains(one_m_eps))
        self.assertTrue(one.contains(1.0))
        self.assertFalse(one.contains(one_p_eps))

        self.assertFalse(self.one_two.contains(one_m_eps))
        self.assertTrue(self.one_two.contains(1.0))
        self.assertTrue(self.one_two.contains(one_p_eps))
        self.assertTrue(self.one_two.contains(two_m_eps))
        self.assertTrue(self.one_two.contains(2.0))
        self.assertFalse(self.one_two.contains(two_p_eps))

    def test_contains_interval(self):
        T = self.testing_t
        self.assertTrue(self.empty.contains(T()))
        self.assertFalse(self.empty.contains(self.one_two))

        one_m_eps, one_p_eps, two_m_eps, two_p_eps = self._epsilons()

        outside_left = T(one_m_eps, one_p_eps)
        on_left = T(1.0, one_p_eps)
        inside = T(one_p_eps, two_m_eps)
        on_right = T(two_m_eps, 2.0)
        outside_right = T(two_m_eps, two_p_eps)

        self.assertTrue(self.one_two.contains(T()))
        self.assertFalse(self.one_two.contains(outside_left))
        self.assertTrue(self.one_two.contains(self.one_two))
        self.assertTrue(self.one_two.contains(on_left))
        self.assertTrue(self.one_two.contains(inside))
        self.assertTrue(self.one_two.contains(on_right))
        self.assertFalse(self.one_two.contains(outside_right))

    def test_contains_with_an_infinite_bound(self):
        # An infinite bound marks an unbounded direction, not a member
        unbounded = self.testing_t(0.0, math.inf)
        self.assertTrue(unbounded.contains(1.0))
        self.assertFalse(unbounded.contains(math.inf))
        self.assertTrue(unbounded.contains(unbounded))
        self.assertTrue(unbounded.contains(self.one_two))
        self.assertFalse(self.one_two.contains(unbounded))

    def test_hash(self):
        hash(self.empty)
        self.assertEqual(hash(self.empty), hash(self.testing_t()))

    # -- Set operations -------------------------------------------------------

    def test_set_union(self):
        T = self.testing_t
        with self.subTest("Union with Empty"):
            self.assertEqual(self.empty.set_union(self.empty), self.empty)
            self.assertEqual(self.empty.set_union(self.one_two), self.one_two)
        with self.subTest("Expands left bound"):
            self.assertEqual(self.one_two.set_union(T(0.0, 1.5)), T(0.0, 2.0))
        with self.subTest("Expands right bound"):
            self.assertEqual(self.one_two.set_union(T(1.5, 3.0)), T(1.0, 3.0))
        with self.subTest("Shared bound"):
            # Bounds are members, so intervals that only touch still overlap
            self.assertEqual(self.one_two.set_union(T(2.0, 3.0)), T(1.0, 3.0))

        self.assertRaises(ValueError, self.one_two.set_union, T(4.0, 5.0))

    def test_set_intersection(self):
        T = self.testing_t
        with self.subTest("Intersection with Empty"):
            self.assertEqual(
                self.empty.set_intersection(self.empty), self.empty
            )
            self.assertEqual(
                self.empty.set_intersection(self.one_two), self.empty
            )
        with self.subTest("Expands left bound"):
            self.assertEqual(
                self.one_two.set_intersection(T(0.0, 1.5)), T(1.0, 1.5)
            )
        with self.subTest("Expands right bound"):
            self.assertEqual(
                self.one_two.set_intersection(T(1.5, 3.0)), T(1.5, 2.0)
            )
        with self.subTest("Shared bound"):
            self.assertEqual(
                self.one_two.set_intersection(T(2.0, 3.0)), T(2.0, 2.0)
            )

        self.assertEqual(self.one_two.set_intersection(T(4.0, 5.0)), T())

    # -- Arithmetic -----------------------------------------------------------

    def test_negation(self):
        self.assertEqual(-self.empty, self.empty)
        self.assertEqual(-self.one_two, self.testing_t(-2.0, -1.0))

    def test_iadd(self):
        # Spot check here and rely on + for exhaustive testing. The in-place
        # operators must hand back the very object they modified.
        one_two = self.one_two
        one_two += one_two
        self.assertIs(one_two, self.one_two)
        self.assertEqual(one_two, self.testing_t(2.0, 4.0))

        empty = self.empty
        empty += self.v(2.0)
        self.assertIs(empty, self.empty)
        self.assertEqual(empty, self.testing_t(2.0, 2.0))

    def test_add(self):
        T = self.testing_t
        with self.subTest("With Interval, Empty"):
            self.assertEqual(self.empty + self.one_two, self.one_two)
            self.assertEqual(self.one_two + self.empty, self.one_two)
        with self.subTest("With Interval, Non-empty"):
            self.assertEqual(self.one_two + T(3.0, 4.0), T(4.0, 6.0))
        with self.subTest("With Scalar"):
            two = self.v(2.0)
            self.assertEqual(self.empty + two, T(two, two))
            self.assertEqual(self.one_two + two, T(3.0, 4.0))

    def test_isub(self):
        copy_one_two = copy.copy(self.one_two)
        one_two = self.one_two
        one_two -= copy_one_two
        self.assertIs(one_two, self.one_two)
        self.assertEqual(one_two, self.testing_t(-1.0, 1.0))

        empty = self.empty
        empty -= self.v(2.0)
        self.assertIs(empty, self.empty)
        self.assertEqual(empty, self.testing_t(-2.0, -2.0))

    def test_sub(self):
        T = self.testing_t
        with self.subTest("With Interval, Empty"):
            self.assertEqual(self.empty - self.one_two, -self.one_two)
            self.assertEqual(self.one_two - self.empty, self.one_two)
        with self.subTest("With Interval, Non-empty"):
            self.assertEqual(self.one_two - T(3.0, 4.0), T(-3.0, -1.0))
        with self.subTest("With Scalar"):
            two = self.v(2.0)
            two_two = T(two, two)
            one_zero = T(-1.0, 0.0)
            self.assertEqual(self.empty - two, -two_two)
            self.assertEqual(two - self.empty, two_two)
            self.assertEqual(self.one_two - two, one_zero)
            self.assertEqual(two - self.one_two, -one_zero)

    def test_imul(self):
        one_two = self.one_two
        one_two *= one_two
        self.assertIs(one_two, self.one_two)
        self.assertEqual(one_two, self.testing_t(1.0, 4.0))

        empty = self.empty
        empty *= self.v(2.0)
        self.assertIs(empty, self.empty)
        self.assertEqual(empty, self.testing_t())

    def test_mul(self):
        T = self.testing_t
        with self.subTest("With Interval, Empty"):
            self.assertEqual(self.empty * self.one_two, self.empty)
            self.assertEqual(self.one_two * self.empty, self.empty)
        cases = [
            ("result == [ll, hh]", self.one_two, T(3.0, 4.0), T(3.0, 8.0)),
            ("result == [lh, ll]", T(-3.0, 2.0), T(-4.0, 3.0), T(-9.0, 12.0)),
            ("result == [lh, hl]", T(-4.0, -3.0), self.one_two, T(-8.0, -3.0)),
            ("result == [lh, hh]", T(-3.0, 4.0), self.one_two, T(-6.0, 8.0)),
            ("result == [hl, ll]", T(-4.0, 3.0), T(-3.0, 2.0), T(-9.0, 12.0)),
            ("result == [hl, lh]", self.one_two, T(-4.0, -3.0), T(-8.0, -3.0)),
            ("result == [hl, hh]", self.one_two, T(-3.0, 4.0), T(-6.0, 8.0)),
            ("result == [hh, ll]", T(-4.0, -3.0), T(-2.0, -1.0), T(3.0, 8.0)),
        ]
        for name, lhs, rhs, result in cases:
            with self.subTest(name):
                self.assertEqual(lhs * rhs, result)
        with self.subTest("value"):
            two = self.v(2.0)
            self.assertEqual(self.one_two * two, T(2.0, 4.0))
            self.assertEqual(two * self.one_two, T(2.0, 4.0))

    def test_itruediv(self):
        copy_one_two = copy.copy(self.one_two)
        one_two = self.one_two
        one_two /= copy_one_two
        self.assertIs(one_two, self.one_two)
        self.assertEqual(one_two, self.testing_t(self.v(1.0 / 2.0), 2.0))

        empty = self.empty
        empty /= self.v(2.0)
        self.assertIs(empty, self.empty)
        self.assertEqual(empty, self.testing_t())

    def test_truediv(self):
        T = self.testing_t
        with self.subTest("With Interval, Empty"):
            self.assertEqual(self.empty / self.one_two, self.empty)
            self.assertEqual(self.one_two / self.empty, self.empty)
        with self.subTest("With Interval, Non-empty"):
            self.assertEqual(self.one_two / self.one_two, T(0.5, 2.0))
            with self.assertRaises(ValueError):
                self.one_two / self.v(0.0)
            with self.assertRaises(ValueError):
                self.one_two / T(-1.0, 1.0)
            # A bound is a member, so a divisor bounded by zero contains it
            with self.assertRaises(ValueError):
                self.one_two / T(0.0, 1.0)
            with self.assertRaises(ValueError):
                self.one_two / T(-1.0, 0.0)
        with self.subTest("With Value"):
            two = self.v(2.0)
            self.assertEqual(self.empty / two, self.empty)
            self.assertEqual(two / self.empty, self.empty)
            self.assertEqual(self.one_two / two, T(0.5, 1.0))
            self.assertEqual(two / self.one_two, T(1.0, 2.0))
            with self.assertRaises(ValueError):
                self.one_two / self.v(0.0)
            with self.assertRaises(ValueError):
                two / T(-1.0, 1.0)

    def test_outward_rounding(self):
        # A product or a quotient of representable values is usually not
        # itself representable, so the bounds have to round outward
        v = self.v
        T = self.testing_t
        two = T(v(2))
        three = T(v(3))

        quotient = two / three
        self.assertGreater(quotient.width(), 0.0)
        self.assertTrue(quotient.contains(v(v(2) / v(3))))

        product = T(v(0.1)) * T(v(0.3))
        self.assertGreater(product.width(), 0.0)
        self.assertTrue(product.contains(v(v(0.1) * v(0.3))))

        total = T(v(0.1)) + T(v(0.2))
        self.assertGreater(total.width(), 0.0)
        self.assertTrue(total.contains(v(v(0.1) + v(0.2))))

        # An exact result stays exact -- the bounds are directed, not padded
        self.assertEqual((two * three).width(), 0.0)
        self.assertEqual(two * three, T(v(6)))

    # -- Printing -------------------------------------------------------------

    def test_print_interval_form(self):
        # std::to_string formats with "%f"
        lo, hi = self.one_two.lower(), self.one_two.upper()
        self.assertEqual(
            self.one_two.print_interval_form(), f"[{lo:f}, {hi:f}]"
        )
        self.assertEqual(self.empty.print_interval_form(), "[∅]")

    def test_str(self):
        # A default std::ostream formats like "%g"
        value = self.testing_t(1.0, 2.0)
        corr = f"{value.median():g}+/-{value.radius():g}"
        self.assertEqual(str(value), corr)
        self.assertEqual(repr(value), corr)
        self.assertEqual(str(self.empty), "∅")

    # -- Comparisons ----------------------------------------------------------

    def test_eq(self):
        self.assertTrue(self.one_two == self.testing_t(1.0, 2.0))
        self.assertFalse(self.one_two == self.empty)
        self.assertTrue(self.empty == self.testing_t())
        self.assertFalse(self.empty == self.one_two)

    def test_ne(self):
        self.assertTrue(self.one_two != self.testing_t(1.0, 3.0))
        self.assertFalse(self.one_two != self.one_two)

    def test_relational_operators(self):
        # Ordering by median, not a rigorous enclosure comparison
        T = self.testing_t
        two_three = T(2.0, 3.0)
        self.assertTrue(self.one_two < two_three)
        self.assertFalse(two_three < self.one_two)
        self.assertTrue(two_three > self.one_two)
        self.assertTrue(self.one_two <= T(1.0, 2.0))
        self.assertTrue(self.one_two >= T(1.0, 2.0))

        # The empty interval sorts before every non-empty one, and is
        # equivalent only to itself
        self.assertTrue(self.empty < self.one_two)
        self.assertFalse(self.one_two < self.empty)
        self.assertFalse(self.empty < T())

    def test_ties_in_median_are_broken_by_the_bounds(self):
        T = self.testing_t
        narrow = T(0.0, 2.0)
        wide = T(-1.0, 3.0)
        self.assertEqual(narrow.median(), wide.median())
        self.assertTrue(wide < narrow)
        self.assertFalse(narrow < wide)

        # ...but the median still decides first
        self.assertTrue(narrow < T(-10.0, 13.0))

        self.assertEqual(len({narrow, wide, narrow}), 2)

    def test_equivalent_under_lt_exactly_when_eq(self):
        T = self.testing_t
        values = [
            self.empty,
            self.one_two,
            T(0.0, 2.0),
            T(-1.0, 3.0),
            T(1.0),
            T(1.0, 1.5),
            T(-5.0, 7.0),
        ]
        for a in values:
            for b in values:
                with self.subTest(a=str(a), b=str(b)):
                    self.assertEqual(not (a < b) and not (b < a), a == b)
                    self.assertEqual(a <= b, a < b or a == b)
                    self.assertEqual(a >= b, b < a or a == b)

    # -- widen ----------------------------------------------------------------

    def test_widen(self):
        T = self.testing_t
        with self.subTest("Point inside is a no-op"):
            self.assertEqual(sigma.widen(self.one_two, 1.5), self.one_two)
            self.assertEqual(sigma.widen(self.one_two, 1.0), self.one_two)
            self.assertEqual(sigma.widen(self.one_two, 2.0), self.one_two)
        with self.subTest("Point below moves the lower bound"):
            self.assertEqual(sigma.widen(self.one_two, 0.0), T(0.0, 2.0))
        with self.subTest("Point above moves the upper bound"):
            self.assertEqual(sigma.widen(self.one_two, 3.0), T(1.0, 3.0))
        with self.subTest("Empty becomes the point"):
            self.assertEqual(sigma.widen(self.empty, 3.0), T(3.0))
        with self.subTest("Point need not be a value_t"):
            self.assertEqual(sigma.widen(self.one_two, 0), T(0.0, 2.0))
        with self.subTest("Infinite point is never contained"):
            r = sigma.widen(self.one_two, math.inf)
            self.assertTrue(math.isinf(r.upper()))
            self.assertFalse(r.contains(math.inf))


class TestIFloat(IntervalTests, unittest.TestCase):
    testing_t = sigma.IFloat
    info = helpers.FLOAT


class TestIDouble(IntervalTests, unittest.TestCase):
    testing_t = sigma.IDouble
    info = helpers.DOUBLE
