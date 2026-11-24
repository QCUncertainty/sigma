"""Helpers shared by the sigma Python unit tests.

The C++ tests are templated on the value type, so they can write
``value_t{1.2}`` and get a float or a double as appropriate. Python only has
double precision floats, so this module supplies the float32 counterparts the
tests need: rounding a value to the nearest float32, ``nextafter``, and the
largest finite value.
"""

import math
import struct
import sys


def _to_float32(value):
    """Rounds a Python float to the nearest float32 (round to nearest)."""
    return struct.unpack("<f", struct.pack("<f", value))[0]


def _float32_bits(value):
    return struct.unpack("<I", struct.pack("<f", value))[0]


def _float32_from_bits(bits):
    return struct.unpack("<f", struct.pack("<I", bits))[0]


def _nextafter_float32(value, toward):
    """The float32 counterpart of ``math.nextafter``."""
    if math.isnan(value) or math.isnan(toward):
        return math.nan
    if value == toward:
        return toward
    if value == 0.0:
        # The smallest subnormal, with the sign of the direction moved in
        tiny = _float32_from_bits(1)
        return tiny if toward > 0.0 else -tiny
    bits = _float32_bits(value)
    # The magnitude grows when moving away from zero
    if (toward > value) == (value > 0.0):
        bits += 1
    else:
        bits -= 1
    return _float32_from_bits(bits)


class ValueTypeInfo:
    """What the tests need to know about a C++ value type.

    :param name: The C++ name of the type.
    :param to_value: Rounds a Python float to the nearest value of the type.
    :param nextafter: ``nextafter`` computed in the type.
    :param max_value: The largest finite value of the type.
    :param exact_libm: Whether a ``math`` function returns, bit for bit, what
        the C++ standard library returns for this type. True for double, as
        both call the same libm; false for float, which C++ computes with the
        float32 functions (e.g., ``expf``) that Python does not expose.
    """

    def __init__(self, name, to_value, nextafter, max_value, exact_libm):
        self.name = name
        self.to_value = to_value
        self.nextafter = nextafter
        self.max_value = max_value
        self.exact_libm = exact_libm


FLOAT = ValueTypeInfo(
    "float",
    _to_float32,
    _nextafter_float32,
    _float32_from_bits(0x7F7FFFFF),
    False,
)

DOUBLE = ValueTypeInfo(
    "double",
    lambda value: float(value),
    math.nextafter,
    sys.float_info.max,
    True,
)


def assert_interval(tc, x, lo, hi):
    """Checks an interval's bounds, median, and radius to within 1.0e-4.

    The counterpart of ``testing::test_interval``.
    """
    tc.assertAlmostEqual(x.lower(), lo, delta=1.0e-4)
    tc.assertAlmostEqual(x.upper(), hi, delta=1.0e-4)
    tc.assertAlmostEqual(x.median(), (lo + hi) / 2.0, delta=1.0e-4)
    tc.assertAlmostEqual(x.radius(), (hi - lo) / 2.0, delta=1.0e-4)


def assert_triplex(tc, x, lo, nominal, hi):
    """Checks a triplex's three values to within 1.0e-4.

    The counterpart of ``testing::test_triplex``.
    """
    tc.assertAlmostEqual(x.lower(), lo, delta=1.0e-4)
    tc.assertAlmostEqual(x.nominal(), nominal, delta=1.0e-4)
    tc.assertAlmostEqual(x.upper(), hi, delta=1.0e-4)


def assert_nominal(tc, info, actual, expected, rel_tol=2.0**-21):
    """Checks a nominal value against a reference computed with ``math``.

    For double the check is exact, as it is in C++. For float the reference
    is only an approximation of what the float32 libm returns, so the check
    is relative, by default to within a few float32 ulps.
    """
    if info.exact_libm:
        tc.assertEqual(actual, expected)
    else:
        tc.assertTrue(
            math.isclose(actual, expected, rel_tol=rel_tol, abs_tol=1.0e-30),
            f"{actual} != {expected} to within {rel_tol} ({info.name})",
        )


def assert_result(tc, info, r, bounds, nominal):
    """Checks a triplex result against its two independent computations.

    The counterpart of ``testing::test_result``: the bounds must be exactly
    those of the interval operation, and the nominal value those of the
    scalar operation (see :func:`assert_nominal` for float).
    """
    tc.assertEqual(r.interval(), bounds)
    assert_nominal(tc, info, r.nominal(), nominal)


def assert_invariant(tc, x):
    """Checks that a triplex satisfies lower() <= nominal() <= upper()."""
    tc.assertTrue(x.contains(x.nominal()))
