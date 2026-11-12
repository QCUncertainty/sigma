# Triplex Arithmetic

> **Status:** design only. `sigma::Triplex` is not implemented yet; this page
> records the design it will be implemented against.

>> **Disclaimer:** Claude added the references and background. Please report
>> any inaccuracies you find.

## Motivation

An [interval](./interval.md) is fully described by its two bounds, and every
other quantity Sigma reports for it is derived from them. In particular the
median, \f$ (a + b)/2 \f$, is the only central value an interval can offer, so
using an interval implicitly assumes that the best estimate of the enclosed
quantity sits exactly halfway between the bounds and that the uncertainty is
symmetric about it.

That assumption fails in two ways, and both are common. The first is that the
uncertainty of a quantity is often genuinely asymmetric. Let \f$ x \f$ have the
best estimate \f$ 0 \f$ and be known to lie in \f$ [-1, 1] \f$. The interval
extension of \f$ \exp \f$ is exact, and gives \f$ [e^{-1}, e] \f$, whose median
is \f$ \cosh(1) \approx 1.543 \f$. The best estimate of \f$ \exp(x) \f$ is
\f$ e^0 = 1 \f$, which is nowhere near the middle of the result.

The second is that the median of a result is not the result of computing with
the medians. Let \f$ X = [-1, 3] \f$, with median \f$ 1 \f$, and form
\f$ X \cdot X \f$. The interval product is \f$ [-3, 9] \f$, whose median is
\f$ 3 \f$, while the computation applied to the best estimate gives
\f$ 1 \cdot 1 = 1 \f$. The bounds widened because of the dependency problem
described on the [interval](./interval.md) page, and the median moved with
them. In an iterative calculation this is worse than a loose bound: a
procedure that feeds the median of one step into the next is no longer
iterating the original equations, so the central value drifts away from the
answer the same procedure gives in ordinary floating point. In the worst case
it no longer converges at all.

Both problems go away if the central value is stored instead of derived. This
page describes a representation that keeps the lower bound, the upper bound,
and a central value as three independent pieces of state.

## Prior art and naming

The representation is the **triplex number**, introduced by Nickel and
coworkers at Karlsruhe in the 1960s and built into the programming language
Triplex-ALGOL 60 [1, 2]. A triplex number is an interval together with a
*Mittelwert* (mean value). Nickel gives two motivations [1], which are the two
problems above seen from the floating-point side. First, a result computed in
ordinary floating point and the same result computed with intervals cannot be
compared, and the floating-point value need not even lie in the interval.
Second, because rounding errors largely cancel, the leading digits of a
floating-point result can be correct even when the corresponding interval
cannot determine its sign. An interval alone throws that information away.
Sigma adopts the name **triplex**.

A more recent representation, the **asymmetric interval number** (AIN) of
Sałabun [3, 4], stores the same three numbers but interprets the central value
differently. In an AIN the central value is an *expectation*. Each AIN is
assigned an auxiliary probability density, uniform on each side of the central
value, which is chosen so that its mean is the central value. The central value
of \f$ g(X) \f$ is then the expectation of \f$ g \f$ under that density, which
requires an integral for every operation. The integral has a closed form for
some functions and must be evaluated by quadrature for others, such as
\f$ X^Y \f$ [4]. Sigma does not adopt the AIN rule, for three reasons. It
replaces a cheap scalar evaluation with an integral for every operation. It
imposes a density that was not part of the input and is not what the true
distribution becomes after a sequence of operations. And it gives up
comparability with ordinary floating point, which is the property that makes
the central value useful in an iterative calculation.

## Definition

A triplex number is an ordered triple

\f{equation}{ \normalsize
X = \langle a, m, b \rangle
\qquad
a \leq m \leq b
\label{eq:tp-def}
\f}

where \f$ [a, b] \f$ is an interval in the sense of the
[interval](./interval.md) page and \f$ m \f$ is the **mean**, a distinguished
member of that interval. The interval carries the guarantee: every possible
value of the quantity lies in \f$ [a, b] \f$. The mean carries the best
estimate: the value the computation would produce if every input were exactly
its mean. Nothing relates \f$ m \f$ to the bounds beyond the ordering of Eq.
\f$\eqref{eq:tp-def}\f$. In general

\f{equation}{ \normalsize
m \neq \frac{a + b}{2}
\label{eq:tp-not-median}
\f}

and Eq. \f$\eqref{eq:tp-not-median}\f$ is the whole point of the type.

The word "mean" follows Nickel's *Mittelwert* and Sigma's own `Uncertain`
class, and it should not be read as a statistical expectation. The mean of
\f$ f(X) \f$ is \f$ f(m) \f$, which is also what the linear error propagation
of the [Background](./background.md) page uses for its mean, and which differs
from the expectation of \f$ f \f$ whenever \f$ f \f$ is nonlinear.

Because the mean need not be central, a triplex number has two radii rather
than one, measuring the distance from the mean to each bound

\f{equation}{ \normalsize
r_{-}(X) = m - a
\qquad
r_{+}(X) = b - m
\label{eq:tp-radii}
\f}

and Nickel's equivalent second form [1] stores exactly these:
\f$ m - r_{-} \leq x \leq m + r_{+} \f$. The width
\f$ b - a = r_{-} + r_{+} \f$ and the median \f$ (a + b)/2 \f$ are defined as
for an interval. The median remains the midpoint of the set, and by Eq.
\f$\eqref{eq:tp-not-median}\f$ it is not the mean.

The bounds may be closed, as in `Interval`, or possibly open, as in
`GeneralInterval` (see the [general interval](./general_interval.md) page).
With open bounds, the mean must still be a member of the set, so it may not
coincide with an open bound. A degenerate triplex number
\f$ \langle v, v, v \rangle \f$ has two closed bounds and represents the
exactly known value \f$ v \f$.

## Arithmetic

Triplex arithmetic is two independent computations carried out side by side.
The bounds are computed exactly as for an interval, with the same outward
rounding, so the enclosure property of interval arithmetic holds unchanged.
The mean is computed in ordinary floating point from the operand means

\f{equation}{ \normalsize
\langle a, m_x, b \rangle \circ \langle c, m_y, d \rangle =
\left\langle
  \mathrm{lo}\left([a, b] \circ [c, d]\right),\;
  \mathrm{fl}\left(m_x \circ m_y\right),\;
  \mathrm{hi}\left([a, b] \circ [c, d]\right)
\right\rangle
\label{eq:tp-binary}
\f}

where \f$ \mathrm{lo} \f$ and \f$ \mathrm{hi} \f$ are the bounds of the
interval result and \f$ \mathrm{fl} \f$ denotes the round-to-nearest result.
A unary function \f$ f \f$ works the same way, with mean
\f$ \mathrm{fl}(f(m)) \f$.

Eq. \f$\eqref{eq:tp-binary}\f$ keeps the ordering of Eq.
\f$\eqref{eq:tp-def}\f$ for the four arithmetic operations without any extra
work. Since \f$ m_x \in [a, b] \f$ and \f$ m_y \in [c, d] \f$, the exact
\f$ m_x \circ m_y \f$ lies in the exact interval result, and the outward
rounded bounds are representable numbers on either side of it. Rounding to
nearest is monotonic, so it cannot carry the mean past a representable number
that brackets it. The same argument does not quite cover the elementary
functions, because the math library's \f$ f(m) \f$ may be off by a few units
in the last place. When this pushes the mean outside the computed bounds, or
onto an open bound, the implementation **widens the bound** to include the mean,
and closes it if it was open. It does not move the mean. Widening never
invalidates an enclosure, and leaving the mean alone keeps the property below
exact.

Applying Eq. \f$\eqref{eq:tp-binary}\f$ to every operation gives the central
guarantee of the type:

> The mean of a triplex computation is, bit for bit, the result of the same
> computation performed in ordinary floating point on the input means.

This is Nickel's comparability property, and it is what fixes the drift
described at the top of this page. Returning to that example,
\f$ \langle -1, 1, 3 \rangle \cdot \langle -1, 1, 3 \rangle =
\langle -3, 1, 9 \rangle \f$. The bounds still suffer the dependency problem,
but the mean is exactly the floating-point answer. Likewise
\f$ X - X \f$ has the needlessly wide bounds of the interval case, but its mean
is exactly zero. The dependency problem cannot affect the mean, because a
single scalar evaluation has no second copy of \f$ X \f$ to decorrelate from.

The elementary functions inherit their domains from the underlying interval
type without change. The mean lies in the argument interval, so if the
interval function accepts the argument, then \f$ f(m) \f$ is defined.
Division by an interval containing zero throws, as it does for intervals, even
when the divisor's mean
is nonzero.

## What is gained, and what remains

Triplex arithmetic adds information to the interval but does not tighten it.
The bounds are exactly the bounds `Interval` would produce, so the dependency
problem and the \f$ O(h) \f$ overestimation discussed on the
[interval](./interval.md) page apply unchanged. What changes is that the
interval no longer has to supply a best estimate. The mean is exact in the
sense above, the asymmetry of the uncertainty is visible through the two radii
of Eq. \f$\eqref{eq:tp-radii}\f$, and a calculation that iterates on the mean
iterates the same equations it would in ordinary floating point.

Two things the mean is *not* should be stated plainly. It is not rigorous: it
carries the rounding error of ordinary floating point, and only the bounds are
guaranteed. And, as noted in the definition, it is not the statistical mean
of the result.

Triplex arithmetic is orthogonal to the dependency tracking of
[affine arithmetic](./affine.md) and [Taylor models](./taylor.md). Those
representations tighten the bounds, while a triplex number keeps an unbiased
central value alongside whatever bounds it is given. Combining the two is
possible, but it is outside the scope of this design.

## Class design

`sigma::Triplex<IntervalType>` is built by **composition** on an interval
type, just as `ThresholdedAffine` is built on `Affine`. `IntervalType` is
either `Interval<U>` or `GeneralInterval<U>`, and `value_t` is its `value_t`.
`Triplex` holds an `IntervalType` and a `value_t` mean, so a closed triplex
does not pay for open bounds. The interval supplies the bounds, any openness,
emptiness, outward rounding, and domain checking. `Triplex` adds only the mean
and the invariant of Eq. \f$\eqref{eq:tp-def}\f$. Each operation delegates the
bounds to the corresponding interval operation, computes the mean with the
corresponding `std` function, and then applies the widening rule described
above. That rule is the only place the two interval types differ, and it goes
through one small helper that widens an interval to include a point, which for
a `GeneralInterval` also closes the bound the point lands on.

Storing the bounds, rather than the mean and the two radii of Nickel's second
form, is deliberate. With the bounds stored, all of the rounding-aware code in
`Interval` and `detail_/policies.hpp` is reused as is. With the radii stored,
the outward rounding would have to be redone for the radii. Nickel prefers the
radius form because it spends fewer digits [1], but that advantage matters
for hand-written decimal triples, not for `double`.

One implementation hazard is specific to this type. The bounds are computed
under directed rounding, and the mean must be computed in round-to-nearest, or
it stops matching the floating-point result. `Interval`'s operations restore
the rounding mode before they return, so the mean is evaluated *after* the
interval operation and outside any rounding guard. A unit test checks the
guarantee of the previous section directly, by comparing the mean of a
nontrivial triplex expression for exact equality with the same expression
evaluated in plain `double`.

The interface mirrors `Interval` where the concepts coincide, and departs from
it where they don't:

- **Construction.** The default constructor makes the empty triplex, which
  is needed for Eigen and for standard containers. A single value constructs
  the degenerate \f$ \langle v, v, v \rangle \f$ and is implicit, as it is for
  `Interval`. `Triplex(lower, mean, upper)` takes its arguments in the order of
  Eq. \f$\eqref{eq:tp-def}\f$, with two trailing openness flags when the
  interval type is `GeneralInterval`, and `Triplex(interval, mean)` is
  explicit. Unlike `Interval`, which sorts its
  bounds, these do not reorder anything. A mean outside the set throws
  `std::domain_error`. There is deliberately **no** constructor from an
  `Interval` alone, because it would have to make up a mean, and the obvious
  choice, the median, is exactly the assumption this type exists to avoid.
- **Accessors.** `lower`, `upper`, `mean`, `width`, `median`, the radii
  `lower_radius` and `upper_radius` of Eq. \f$\eqref{eq:tp-radii}\f$, `empty`,
  and `interval()`, which returns the bounds as an `IntervalType` and discards
  the mean. A general triplex adds the four openness queries. There is no
  symmetric `radius`. For a
  triplex it would mean half the width, and that number is easily mistaken for
  a distance from the mean. Code that wants it can write `width() / 2`.
  Accessors that need a value throw `std::domain_error` on the empty triplex,
  through a private `assert_not_empty_` helper as in `Interval`.
- **Arithmetic.** The compound-assignment and free operators of Eq.
  \f$\eqref{eq:tp-binary}\f$, between two triplex numbers and between a triplex
  and a `value_t`. There is no mixed arithmetic with `Interval`, since an
  interval has no mean to combine.
- **Elementary functions.** The same set as `Interval`, as free functions in
  `sigma` found by argument-dependent lookup: `abs`, `sqrt`, `exp`, `log`,
  `pow`, the trigonometric and inverse trigonometric functions, and the
  hyperbolic and inverse hyperbolic functions.
- **Set queries.** The two `contains` overloads, which test against the
  bounds. `set_union` and `set_intersection` are **omitted**: the result is a
  well-defined set, but there is no defensible mean for it.
- **Comparison and hashing.** `==` compares all three values, and the
  openness of the bounds when there is any. The ordering operators order by
  mean, which plays the role
  the median plays in `Interval`'s ordering, and like that ordering they give a
  total order for sorting rather than an enclosure relation. A `std::hash`
  specialization follows `Interval`'s.
- **Output.** Stream insertion prints `[a, m, b]`, with a parenthesis in place
  of the bracket for an open bound, consistent with the interval types.

The files follow the existing layout: `include/sigma/triplex/triplex.hpp`, the
operations split into `basic`, `exponents`, `trigonometry`, and `hyperbolic`
headers under `include/sigma/triplex/operations/`, an `eigen_compat.hpp` that
specializes `Eigen::NumTraits` as `Interval` does, an include in
`sigma/sigma.hpp`, and tests under `tests/unit_tests/triplex/`. The
convenience typedefs `TPFloat` and `TPDouble` name the closed instantiations,
`Triplex<Interval<float>>` and `Triplex<Interval<double>>`, since `TFloat` and
`TDouble` already name `Taylor` instantiations. The general instantiations are
written out in full until there is a need for typedefs.

## Future work

The design leaves out several things that would be straightforward to add
later:

- A factory taking the mean and the two radii, Nickel's second form.
- An AIN-style policy that propagates the mean as an expectation, for users who
  want a statistical rather than a nominal central value. Under the design
  above this would be a policy choice for the mean alone, since the bounds are
  the same either way.
- A triplex analogue of `Affine`, which would pair a nominal mean with bounds
  that track dependence.

## References



1. K. Nickel, "Bericht über neue Karlsruher Ergebnisse bei der Fehlererfassung
   von numerischen Prozessen," *Aplikace matematiky* **13**, 168–173 (1968).
   <http://dml.cz/dmlcz/103150>
2. K. Nickel, N. Apostolatos, U. Kulisch, R. Krawczyk, B. Lortz, and H. W.
   Wippermann, "The Algorithmic Language Triplex-ALGOL 60," *Numerische
   Mathematik* **11**, 175–180 (1968).
3. W. Sałabun, "Asymmetric interval numbers: A new approach to modeling
   uncertainty," *Fuzzy Sets and Systems* **499**, 109169 (2025).
4. A. Shekhovtsov, A. Świder, S. Śniegowski, and W. Sałabun, "Exponentiation
   Operators for Asymmetric Interval Numbers and Their Properties," ICCS 2026,
   doi:10.1007/978-3-032-29915-4_27.
