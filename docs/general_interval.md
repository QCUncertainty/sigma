# General Intervals

## Motivation

The [interval](./interval.md) page describes `Interval`, whose bounds are always
members of the set. Mathematics also has open and half-open intervals, whose
bounds are limits the set approaches but does not contain. They matter at the
edges of a function's domain. \f$ \log \f$ is defined on \f$ (0, 1\rbrack \f$
but not on \f$ [0, 1] \f$, and \f$ 1/x \f$ is defined on \f$ (0, 1\rbrack \f$
but not on \f$ [0, 1] \f$. An interval type that cannot express an open bound
must refuse both, even though the true result exists and is easy to enclose.

Most computations never get near that edge, and for them openness is pure cost.
Closed intervals are self-sufficient: an operation on closed intervals that
succeeds always returns a closed interval, as shown below, so a calculation
that starts closed never produces an open bound.
Tracking openness anyway costs two flags per value. For `double`, those flags
pad the class from 24 to 32 bytes, which is significant for the large Eigen
arrays of intervals used in quantum chemistry. Every operation also carries the
bookkeeping that decides the openness of its result.

Sigma therefore splits the two. `Interval` represents closed intervals only,
and `GeneralInterval` adds open bounds on top of it, so a calculation pays for
openness only if it asks for it.

## Naming

Interval analysis takes intervals to be closed. Moore's original arithmetic
works with closed intervals, and the IEEE 1788-2015 standard defines an
interval as a closed, connected subset of the reals. `Interval` keeps the name
for the closed case, which is the standard meaning in the field.

In mathematics more broadly, "interval" covers all four forms below, so the
type that represents all of them is the **general** interval. The obvious
alternative, `OpenInterval`, is misleading: it suggests a type that represents
only \f$ (a, b) \f$, whereas a `GeneralInterval` is just as often closed or
half-open.

## Definition

A general interval is an interval in the sense of the
[interval](./interval.md) page, together with a record of whether each bound is
a member of the set. That gives four forms

\f{equation}{ \normalsize
\begin{aligned}
[a, b] &= \left\{ x : a \leq x \leq b \right\} &
\lbrack a, b) &= \left\{ x : a \leq x < b \right\} \\
(a, b\rbrack &= \left\{ x : a < x \leq b \right\} &
(a, b) &= \left\{ x : a < x < b \right\}
\end{aligned}
\label{eq:gi-def}
\f}

and the openness of each bound is tracked independently of the other. Openness
changes which values are members, and nothing else. The width, radius, and
median are those of the closed interval with the same bounds, so
\f$ [a, b] \f$ and \f$ \lbrack a, b) \f$ have the same width even though only
one of them contains \f$ b \f$. The closed interval with the same bounds is the
**closure** of a general interval, and it is the smallest `Interval` that
contains it.

An infinite bound is never a member of the set, since no real number is
infinite. IEEE 1788 follows the same convention. In `GeneralInterval` an
infinite bound is always reported as open. In `Interval`, which has no
openness to report, an infinite bound is read as "unbounded in that
direction".

## Closed in, closed out

The split is sound because closed intervals are self-sufficient. A function
that is continuous on a closed, bounded interval attains its extremes there, so
its range is again a closed, bounded interval. That covers the four arithmetic
operations, with division restricted to divisors that do not contain zero, and
every elementary function on a closed argument within its domain. The
operations that *would* produce an open or unbounded result from a closed
argument are exactly the ones whose argument touches the edge of the domain:
\f$ \log([0, 1]) \f$ and division by \f$ [0, 1] \f$. `Interval` throws for
those, as it must, since the argument contains a value the function is not
defined at. So every
`Interval` result is a closed interval, and open bounds enter a calculation only
when the caller constructs one.

The one exception is overflow. A bound that is finite in exact arithmetic can
round to infinity, as in \f$ \exp([0, 1000]) \f$. The infinite-bound
convention above covers this case: the result means "unbounded above", which is
a valid enclosure.

## Openness under arithmetic

A bound of a result is a member of the result exactly when the operand bounds
that produced it are members of their operands. For addition and subtraction,
each result bound comes from one bound of each operand, so its openness is the
logical OR of theirs

\f{equation}{ \normalsize
\mathrm{open}\left(a + c\right) = \mathrm{open}(a) \lor \mathrm{open}(c)
\qquad
\mathrm{open}\left(a - d\right) = \mathrm{open}(a) \lor \mathrm{open}(d)
\label{eq:gi-add}
\f}

where \f$ \mathrm{open}(a) \f$ is true when the bound \f$ a \f$ is not a member
of its interval. For nonzero result bounds, multiplication and division use
the four corners (the products or quotients of one bound of each operand),
and apply the same rule to the corners attaining the extreme. A bound is
closed if any of the tying corners uses two included bounds. For example,
the lower bound of \f$ [0, 1] \cdot \lbrack 0, 1) \f$ is closed because the
corner \f$ 0 \cdot 0 \f$ uses two included bounds.

Zero requires an additional attainment check: a zero extreme of a product is
closed whenever either operand contains zero and the other is nonempty,
even if neither bound of the other operand is included. Similarly, a zero
extreme of a quotient is closed whenever the numerator contains zero and the
admissible divisor is nonempty. Thus \f$ [0,0] \cdot (0,1) = [0,0] \f$.

Openness also changes which divisors are allowed. A divisor merely *bounded* by
zero, such as \f$ (0, 1\rbrack \f$, does not contain it, so it is divided by
normally, and the reciprocal runs off to infinity in that direction:

\f{equation}{ \normalsize
\frac{1}{(0, 1\rbrack} = \lbrack 1, \infty)
\label{eq:gi-recip}
\f}

A divisor that contains zero throws, as it does for `Interval`.

## Openness of elementary functions

A monotonic function maps each bound to a bound of the result. An increasing
function keeps the openness of each bound, and a decreasing one swaps them,
just as it swaps the bounds. A non-monotonic function can attain an extreme in
the interior of its argument, and an interior extreme is always attained, so
the corresponding bound of the result is closed whatever the openness of the
argument. This is why the bounds of a trigonometric result are reported
closed: they may come from an interior extremum rather than from a bound of
the argument. The same reasoning covers the interior minimum of \f$ \cosh \f$
at the origin and of an even power at zero. `abs` splits the argument at zero
and takes the union of the pieces, keeping their openness.

Openness decides every boundary case of a domain, since an open bound is not a
value the interval contains. So \f$ \log([0, 1]) \f$ throws, while
\f$ \log((0, 1\rbrack) \f$ is unbounded below, and
\f$ \mathrm{atanh}([-1, 1]) \f$ throws, while
\f$ \mathrm{atanh}((-1, 1)) \f$ is the whole line. The same rule governs
`pow`, whose two impossible cases are a negative base under a fractional
exponent and a zero base under a negative one. Everywhere else the domain rules
are those of `Interval`: a function whose argument contains a value it is not
defined at throws `std::domain_error` rather than trimming the argument back
into its domain.

`tan` is the one function whose domain excludes points *interior* to the real
line rather than bounding it, being undefined at every odd multiple of
\f$ \pi/2 \f$. Openness cannot rescue the boundary case there, because those
points are irrational and so not representable as a bound: an argument that
comes within rounding distance of a pole cannot be shown to exclude it, and is
refused along with the arguments that genuinely contain one. A
`GeneralInterval` therefore gains nothing over an `Interval` for `tan`.

## Openness in set operations

Openness matters to the set operations wherever two bounds coincide. The
intersection \f$ [0, 1] \cap (1, 2\rbrack \f$ is empty, whereas
\f$ [0, 1] \cap [1, 2] \f$ is the single point \f$ [1, 1] \f$. The union
\f$ \lbrack 0, 1) \cup [1, 2] \f$ is the single interval \f$ [0, 2] \f$,
whereas \f$ \lbrack 0, 1) \cup (1, 2\rbrack \f$ is not an interval at all,
because it is missing \f$ 1 \f$. In the same way, \f$ \lbrack 0, 1) \f$ does not
contain \f$ 1 \f$, and it does not contain \f$ [0, 1] \f$ either.

## Class design

`sigma::GeneralInterval<ValueType>` is built by **composition** on `Interval`,
as `ThresholdedAffine` is built on `Affine`. It holds an `Interval<value_t>`,
which supplies the bounds, emptiness, and outward rounding, plus two booleans
for the openness of the bounds. The booleans are meaningless for the empty
interval, which is open and closed at once in the topological sense and is
equal only to itself.

Wrapping alone is not enough. Openness changes which arguments are in the
domain, so `GeneralInterval` cannot compute its result by calling `Interval` on
its closure: `log((0, 1])` would become `log([0, 1])`, which throws. Each
elementary function is therefore two layers. The lower layer is the round trip
through boost in `detail_`: convert the closure to a boost interval, evaluate
the boost function under Sigma's rounding policy, and convert back. The upper
layer is the domain check. Both classes share the lower layer, so the rounding
code, the hard part, exists once. `Interval` puts its closed domain check on
top of it. `GeneralInterval` puts an openness-aware domain check on top of it,
and then applies the openness rules above to the result. The arithmetic
operators work the same way, delegating the bounds to the closure's operators
except for a divisor bounded by an open zero, whose closure cannot be divided
by and which is handled as multiplication by its reciprocal.

The interface is `Interval`'s, plus openness:

- **Construction.** The default constructor makes the empty interval, and a
  single value makes a degenerate closed one, as for `Interval`.
  `GeneralInterval(lower, upper, left_open, right_open)` takes the bounds and
  their openness. An `Interval` converts **implicitly** to a `GeneralInterval`
  with both bounds closed, which is lossless, so arithmetic that mixes the two
  types promotes to `GeneralInterval`. The reverse conversion loses
  information and is spelled out with `closure()`.
- **Accessors.** Everything `Interval` provides, plus `left_open`,
  `left_closed`, `right_open`, and `right_closed`, and `closure()`, which
  returns the closure as an `Interval`.
- **Arithmetic and elementary functions.** The same operators and the same
  free functions as `Interval`, found through argument-dependent lookup, with
  the domain and openness rules above. The binary arithmetic operators are
  hidden friends rather than free templates, because template argument
  deduction ignores conversions and so would not accept an `Interval` or a
  scalar operand.
- **Set operations.** `set_union`, `set_intersection`, and the two `contains`
  overloads, with the openness rules above.
- **Comparison and hashing.** `==` compares the bounds and their openness,
  so \f$ [a, b] \neq \lbrack a, b) \f$. The ordering operators order by median,
  as `Interval`'s do, breaking ties by the bounds and then their openness so
  that two intervals are equivalent under the ordering exactly when `==` says
  they are equal. The `std::hash` specialization includes the openness.
- **Output.** `print_interval_form` prints a parenthesis in place of the
  bracket for an open bound. Stream insertion prints the median and radius, as
  it does for `Interval`.

`Interval` correspondingly has no openness. Its constructor is
`Interval(lower, upper)`, `print_interval_form` always prints brackets, and its
set operations and `contains` overloads compare bounds only.

`GeneralInterval` lives alongside `Interval` in
`include/sigma/interval/general_interval.hpp`, with its elementary functions in
`include/sigma/interval/general_operations/` and its conversions in
`detail_/general_convert.hpp`, since the two classes share the round trip
through boost. It specializes `Eigen::NumTraits` the same way `Interval` does,
and the convenience typedefs are `GIFloat` and `GIDouble`.

Representations built on intervals choose which one they need. `Affine` and
`Taylor` use intervals only to bound ranges, never produce an open bound, and
use `Interval`. [Triplex](./triplex.md) numbers are templated on
their interval type, so the same class provides both a closed and a general
triplex.
