# Interval Arithmetic

## Motivation

The [Background](./background.md) page describes linear error propagation, the
theory behind Sigma's `Uncertain` class. That approach represents a quantity by
a mean and a standard deviation, and propagates the standard deviation through a
first-order Taylor expansion of each operation. It answers a statistical
question: given the spread of the inputs, what is the likely spread of the
output?

Some problems ask a different question. Rather than a likely spread, we may need
a *guarantee* that the true result lies within some set. Rather than independent
inputs, we may have quantities whose errors are strongly correlated because they
were computed from common ancestors. For these problems Sigma provides three
set-valued representations, each of which encloses the true result rather than
estimating its distribution.

The three form a progression, and each page in this series adds one capability

| representation | tracks dependence | order | overestimation |
| -------------- | ----------------- | ----- | -------------- |
| `Interval` | no | zeroth | \f$ O(h) \f$ |
| `Affine` | yes | first | \f$ O(h^2) \f$ |
| `TaylorModel` | yes | \f$ n \f$ | \f$ O(h^{n+1}) \f$ |

where \f$ h \f$ is the width of the input domain and \f$ n \f$ is a truncation
order chosen by the user. This page covers the first row. The
[affine](./affine.md) and [Taylor model](./taylor.md) pages cover the other two,
and each assumes the vocabulary established here.

## Definition

An interval is the set of real numbers lying between two bounds

\f{equation}{ \normalsize
[a, b] = \left\{ x \in \mathbb{R} : a \leq x \leq b \right\}
\label{eq:iv-def}
\f}

where \f$ a \f$ is the lower bound and \f$ b \f$ is the upper bound. A quantity
whose exact value is unknown, but which is known to lie between \f$ a \f$ and
\f$ b \f$, is fully described by Eq. \f$\eqref{eq:iv-def}\f$. No distributional
assumption is made: every value in the set is considered possible, and no value
outside it is.

Sigma additionally tracks whether each bound is itself a member of the set, so
the half-open interval \f$ \lbrack a, b) \f$ and the open interval
\f$ (a, b) \f$ are also representable. The bounds are tracked independently, and
the openness of a bound does not affect the interval's width.

Two derived quantities appear throughout the library. The width is the distance
between the bounds, and the radius is half of it

\f{equation}{ \normalsize
w\left([a,b]\right) = b - a
\qquad
\mathrm{rad}\left([a,b]\right) = \frac{b - a}{2}
\label{eq:iv-width}
\f}

along with the median, \f$ (a + b)/2 \f$, which is the midpoint of the set. The
median and radius pair is an equivalent description of a closed interval, and it
is the form the [affine](./affine.md) representation generalizes.

## Arithmetic

The guiding principle of interval arithmetic is that an operation on intervals
must produce an interval containing every result obtainable from the operands.
For an operation \f$ \circ \f$ this requirement is

\f{equation}{ \normalsize
X \circ Y \supseteq
\left\{ x \circ y : x \in X, y \in Y \right\}
\label{eq:iv-enclosure}
\f}

For the elementary arithmetic operations the enclosing set can be computed
exactly from the bounds. Addition and subtraction are the simplest cases, since
each is monotonic in both arguments

\f{equation}{ \normalsize
[a, b] + [c, d] = [a + c,\; b + d]
\label{eq:iv-add}
\f}

\f{equation}{ \normalsize
[a, b] - [c, d] = [a - d,\; b - c]
\label{eq:iv-sub}
\f}

Note the crossing of the bounds in Eq. \f$\eqref{eq:iv-sub}\f$: the smallest
possible difference comes from the smallest minuend and the largest subtrahend.
Multiplication is not monotonic when the operands straddle zero, so the extremes
may occur at any of the four combinations of endpoints

\f{equation}{ \normalsize
[a, b] \cdot [c, d] =
\left[
  \min\left(ac, ad, bc, bd\right),\;
  \max\left(ac, ad, bc, bd\right)
\right]
\label{eq:iv-mul}
\f}

Division is defined as multiplication by the reciprocal of the divisor

\f{equation}{ \normalsize
\frac{1}{[c, d]} = \left[\frac{1}{d}, \frac{1}{c}\right]
\qquad
0 \notin [c, d]
\label{eq:iv-recip}
\f}

where the stated condition is essential. If the divisor contains zero the
reciprocal is unbounded and no finite interval encloses the result, so Sigma
throws rather than returning a misleading answer.

## Elementary functions

A function that is monotonic over the interval attains its extremes at the
endpoints, so its interval extension requires only two scalar evaluations

\f{equation}{ \normalsize
f\left([a, b]\right) = \left[f(a),\; f(b)\right]
\qquad
f \; \mathrm{increasing}
\label{eq:iv-monotone}
\f}

This is why `sqrt`, `exp`, and `log` are inexpensive for intervals, and why they
need no approximation: for monotonic functions Eq. \f$\eqref{eq:iv-monotone}\f$
is not a bound on the range, it *is* the range. Sigma applies the corresponding
form for decreasing functions, and for `abs` it handles the non-monotonic case
by splitting at zero and taking the union of the pieces.

## Two fundamental limitations

Interval arithmetic is exact for a single operation, in the sense that Eq.
\f$\eqref{eq:iv-add}\f$ through Eq. \f$\eqref{eq:iv-monotone}\f$ produce the
tightest interval containing all possible results. Composing operations is where
accuracy is lost, and it is lost in two distinct ways.

The first is the **dependency problem**. The representation records only a set
of possible values; it does not record *which* quantity those values came from.
So when the same quantity appears twice in an expression, the two occurrences
are treated as independent. Let \f$ x = [1, 2] \f$ and consider
\f$ y = x - x \f$. The true answer is zero for every possible value of
\f$ x \f$, but Eq. \f$\eqref{eq:iv-sub}\f$ gives

\f{equation}{ \normalsize
[1, 2] - [1, 2] = [1 - 2,\; 2 - 1] = [-1, 1]
\label{eq:iv-dependency}
\f}

The result is not wrong — it does enclose zero — but it is needlessly wide, and
the width came from nowhere. Eq. \f$\eqref{eq:iv-dependency}\f$ is the canonical
demonstration that correlation information has been discarded, and recovering it
is precisely what the [affine](./affine.md) representation does.

The second limitation is the **rate at which overestimation grows**. Because an
interval carries no information about how the enclosed quantity varies, each
operation must assume the worst case independently. Over a domain of width
\f$ h \f$ the excess width of an interval result scales as \f$ O(h) \f$, which
is to say it shrinks only in proportion to the domain. Halving the domain halves
the overestimation. For a long computation this is often not enough, and it is
what motivates carrying higher-order information, as described on the
[Taylor model](./taylor.md) page.

## Class design

`sigma::Interval` wraps `boost::numeric::interval`, which supplies the
rounding-aware bound arithmetic of Eq. \f$\eqref{eq:iv-add}\f$ through Eq.
\f$\eqref{eq:iv-monotone}\f$, and adds the two pieces of state boost does not
track.

The first is emptiness. The wrapped boost interval is held in a `std::optional`,
and an empty `std::optional` represents the empty set. The empty interval arises
naturally from `set_intersection` of disjoint intervals, and it propagates
through subsequent operations. Accessors that have no meaningful value on the
empty set — `lower`, `upper`, `width`, `median` — funnel through a private
`assert_not_empty_` helper and throw `std::domain_error`.

The second is bound openness, stored as two independent booleans and queried
through the `left_open`, `left_closed`, `right_open`, and `right_closed`
accessors. Sigma keeps these separate from the numeric bounds so that
\f$ [a, b] \f$ and \f$ \lbrack a, b) \f$ are distinguishable even though their
widths agree.

The remaining interface divides into the derived quantities of Eq.
\f$\eqref{eq:iv-width}\f$ — `width`, `median`, and `radius` — the set operations
`set_union`, `set_intersection`, and the two `contains` overloads for testing a
value or another interval, and the arithmetic of Eq. \f$\eqref{eq:iv-add}\f$
through Eq. \f$\eqref{eq:iv-recip}\f$ exposed as the usual compound-assignment
and free operators. Because a `radius` of zero means the value is known exactly,
`Interval` also serves as the degenerate case of the representations that
follow. Convenience typedefs `IFloat` and `IDouble` cover the two common
instantiations, and [sigma::Interval](@ref sigma::Interval) documents the full
API.

## Next

The dependency problem of Eq. \f$\eqref{eq:iv-dependency}\f$ is the motivating
defect for [affine arithmetic](./affine.md), which fixes it by giving each
independent source of uncertainty a name and keeping those names attached to the
quantities derived from it.
