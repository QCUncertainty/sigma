# Affine Arithmetic

## Motivation

The [Interval](./interval.md) page closed with the dependency problem: because
an interval records only a set of values and not the identity of the quantity
that produced them, \f$ x - x \f$ evaluates to \f$ [-1, 1] \f$ rather than to
zero when \f$ x = [1, 2] \f$. The width of that result is pure artifact.

Affine arithmetic removes the artifact by changing what is stored. Instead of
two bounds, a quantity is represented as a center plus a list of named
contributions, where each name identifies an independent source of uncertainty.
When two quantities share a name, the representation knows they vary together,
and cancellation happens automatically. Sigma's implementation follows
[Rump and Kashiwagi](https://www.tuhh.de/ti3/paper/rump/RuKas14.pdf).

## The affine form

A quantity is written as a center value plus a weighted sum of error symbols

\f{equation}{ \normalsize
\hat{x} = x_0 + \sum_{i=1}^{n} x_i \epsilon_i
\qquad
\epsilon_i \in [-1, 1]
\label{eq:af-form}
\f}

where \f$ x_0 \f$ is the center, the \f$ x_i \f$ are radii, and the
\f$ \epsilon_i \f$ are error symbols. Each symbol is an opaque label standing
for one independent unknown that ranges over \f$ [-1, 1] \f$. The affine form
denotes the set of all values obtainable by substituting some admissible value
for every symbol, so its interval range follows from summing the magnitudes of
the radii

\f{equation}{ \normalsize
\mathrm{range}\left(\hat{x}\right) =
\left[
  x_0 - \sum_{i=1}^{n} \left|x_i\right|,\;
  x_0 + \sum_{i=1}^{n} \left|x_i\right|
\right]
\label{eq:af-range}
\f}

The quantity \f$ \sum_i |x_i| \f$ is the radius of the affine form, and Eq.
\f$\eqref{eq:af-range}\f$ recovers the interval description of the
[previous page](./interval.md) whenever it is needed. The reverse conversion
introduces one fresh symbol whose radius is the interval's radius, so any
interval can be promoted to an affine form.

## Why sharing symbols solves the dependency problem

The symbols are what carry correlation. Take \f$ x = [1, 2] \f$ and promote it
using Eq. \f$\eqref{eq:af-form}\f$, giving
\f$ \hat{x} = 1.5 + 0.5\,\epsilon_1 \f$. Now form \f$ y = x - x \f$

\f{equation}{ \normalsize
\hat{y} =
\left(1.5 + 0.5\,\epsilon_1\right) - \left(1.5 + 0.5\,\epsilon_1\right) = 0
\label{eq:af-cancel}
\f}

The centers cancel and, critically, the \f$ \epsilon_1 \f$ radii cancel too,
because both occurrences of \f$ x \f$ reference the *same* symbol. Eq.
\f$\eqref{eq:af-cancel}\f$ yields exactly zero, where the interval computation
gave \f$ [-1, 1] \f$. Nothing about the arithmetic is special; the improvement
comes entirely from the representation remembering where the uncertainty
originated.

This is also why symbols must be globally unique. Two quantities that are
genuinely independent must not accidentally share a label, or the arithmetic
will report a correlation that does not exist and may return a result that is
too narrow.

## Exact operations

Addition, subtraction, and scaling by a constant preserve the form of Eq.
\f$\eqref{eq:af-form}\f$ exactly. Adding two affine forms adds their centers and
adds the radii of matching symbols

\f{equation}{ \normalsize
\hat{x} + \hat{y} =
\left(x_0 + y_0\right) + \sum_{i=1}^{n} \left(x_i + y_i\right) \epsilon_i
\label{eq:af-add}
\f}

where a symbol absent from one operand simply contributes a radius of zero.
Multiplication by a scalar scales the center and every radius. These operations
introduce no approximation whatsoever, which is the central strength of affine
arithmetic: a computation built only from linear combinations is tracked with no
loss at all.

## Where linearization enters

The product of two affine forms is not affine. Expanding Eq.
\f$\eqref{eq:af-form}\f$ for two operands produces a term in
\f$ \epsilon_i \epsilon_j \f$, which is quadratic and has no representation in
the form. Sigma keeps the linear part exactly and bounds the quadratic remainder
with a single new symbol

\f{equation}{ \normalsize
\hat{x} \cdot \hat{y} =
x_0 y_0 +
\sum_{i=1}^{n} \left(x_i y_0 + y_i x_0\right) \epsilon_i +
\left(\sum_{i=1}^{n}\left|x_i\right|\right)
\left(\sum_{j=1}^{n}\left|y_j\right|\right)
\epsilon_{n+1}
\label{eq:af-mul}
\f}

where \f$ \epsilon_{n+1} \f$ is freshly minted and its radius is the product of
the two operand radii. The final term of Eq. \f$\eqref{eq:af-mul}\f$ is an
overestimate — the true quadratic contribution is generally smaller — and it is
where accuracy begins to be lost.

The same pattern covers every nonlinear operation. Given a function \f$ f \f$,
find a linear approximation to it over the current range together with a bound
on the approximation error, then apply

\f{equation}{ \normalsize
f\left(\hat{x}\right) \approx
\alpha x_0 + \zeta +
\sum_{i=1}^{n} \alpha x_i \epsilon_i +
\delta \epsilon_{n+1}
\label{eq:af-transform}
\f}

where \f$ \alpha \f$ is a scale factor, \f$ \zeta \f$ a shift, and
\f$ \delta \f$ the radius of one new symbol absorbing everything the linear
model cannot express. Eq. \f$\eqref{eq:af-transform}\f$ is implemented as
`apply_affine_transform`, and every nonlinear operation in the library funnels
through it. Reducing all nonlinearity to a single hook keeps the class small,
but it also means **all** higher-order information is collapsed into one scalar
\f$ \delta \f$, which is the structural limit of the approach.

## Constructing the transform

The parameters of Eq. \f$\eqref{eq:af-transform}\f$ come from a Chebyshev-style
construction. Over the operand's current range \f$ [a, b] \f$, take the chord of
\f$ f \f$ between the endpoints; its slope is the scale factor

\f{equation}{ \normalsize
\alpha = \frac{f(b) - f(a)}{b - a}
\label{eq:af-chord}
\f}

For a function of constant convexity the maximum deviation from that chord
occurs where the tangent has the same slope. The chord and that parallel tangent
bracket the function, so placing the shift halfway between them and setting
\f$ \delta \f$ to half their separation gives the smallest error bound
achievable by a linear model. Sigma applies this construction directly for `exp`
and `log`, uses the closed-form equivalent for `sqrt`, and derives
`multiplicative_inverse` the same way from the extremes of \f$ 1/x \f$ over the
range.

Division and powers are then composed rather than derived independently.
Division multiplies by the multiplicative inverse, and `pow` is evaluated as

\f{equation}{ \normalsize
\hat{x}^{\,y} = \exp\left(y \log \hat{x}\right)
\label{eq:af-pow}
\f}

with special handling for the cases Eq. \f$\eqref{eq:af-pow}\f$ cannot express:
a zero exponent, an operand range containing zero, and a strictly negative
operand range, where the result is obtained from \f$ |\hat{x}| \f$ and the sign
restored for odd integer exponents. Non-integer exponents of a negative operand
are rejected.

## What is gained, and what remains

Affine arithmetic is strictly better than interval arithmetic on dependent
computations, and the improvement is not marginal. Linear operations are exact,
and for nonlinear operations the excess width scales as \f$ O(h^2) \f$ in the
domain width rather than \f$ O(h) \f$ — halving the domain now cuts
overestimation by a factor of four. Two limitations remain.

The first is that the representation is **first order by construction**. Eq.
\f$\eqref{eq:af-form}\f$ admits no \f$ \epsilon_i \epsilon_j \f$ term, so every
quadratic and higher contribution must be flattened into the lumped
\f$ \delta \f$ of Eq. \f$\eqref{eq:af-transform}\f$. For strongly nonlinear
expressions, or for long chains of multiplications, that lump dominates the
result.

The second is **symbol growth**. Each nonlinear operation mints a new symbol and
none are ever retired, so a long computation accumulates terms indefinitely. The
radii of most of them are negligible, but they are still stored and still
iterated over.

## Thresholding

`sigma::ThresholdedAffine` addresses symbol growth by pruning. After every
mutating operation it computes each term's contribution relative to the total
radius and deletes those falling below a threshold, which defaults to one part
in a thousand

\f{equation}{ \normalsize
\frac{\left|x_i\right|}{\sum_{j} \left|x_j\right|} < t
\quad \Longrightarrow \quad
\mathrm{drop} \; \epsilon_i
\label{eq:af-threshold}
\f}

Terms whose radii are below the machine epsilon of the value type are dropped as
well.

It is important to be precise about what Eq. \f$\eqref{eq:af-threshold}\f$
costs. **Dropped terms are discarded, not bounded.** No lumped radius is
retained to account for them, so the pruned form is narrower than the true form
by the total radius of everything removed. The result is a tighter but no longer
guaranteed enclosure, and pruning is therefore not rigor-preserving on its own.
This is a deliberate trade in `ThresholdedAffine` — the threshold defaults low
enough that the discrepancy is usually negligible — but it is a trade, and the
[Taylor model](./taylor.md) page shows how to get the same term-count control
while keeping the enclosure valid, by moving discarded contributions into an
explicit remainder instead of deleting them.

## Class design

`sigma::Affine` stores exactly the two pieces of Eq. \f$\eqref{eq:af-form}\f$.
The center is a `std::optional<value_t>`, where an empty optional represents the
empty set, so `empty` is a test on the optional and the accessors that need a
value funnel through a private `assert_not_empty_` helper as in `Interval`. The
radii live in an `error_terms_t`, an `std::unordered_map` from symbol to radius;
only nonzero contributions are stored, and lookups during Eq.
\f$\eqref{eq:af-add}\f$ are therefore hash lookups on the symbol.

Symbols are minted by the static `make_error_term`, which returns successive
values from a function-local `std::atomic` counter. The symbol type is an opaque
integer alias — callers are not meant to construct or interpret one, only to
pass it along — and the atomic counter is what guarantees the global uniqueness
the correctness of Eq. \f$\eqref{eq:af-cancel}\f$ depends on.

`ThresholdedAffine` is built by **composition rather than inheritance**: it
holds an `Affine` and a threshold, forwards the accessors, and re-applies Eq.
\f$\eqref{eq:af-threshold}\f$ after each mutating operation. Its nonlinear
operations unwrap to the underlying form, delegate to the corresponding `Affine`
function, and re-wrap the result with the same threshold. Constructors take the
threshold through a nested `Threshold` tag type, which exists to keep
`ThresholdedAffine(center, threshold)` from colliding with
`ThresholdedAffine(lower, upper)`.

Convenience typedefs `AFloat` and `ADouble` cover `Affine`, and `TAFloat` and
`TADouble` cover `ThresholdedAffine`. See [sigma::Affine](@ref sigma::Affine)
and [sigma::ThresholdedAffine](@ref sigma::ThresholdedAffine) for the full API.

One caveat applies to both classes: neither tracks the uncertainty introduced by
floating-point arithmetic on the centers and radii themselves. The radii bound
the modeled uncertainty, not the rounding error committed while propagating it.

## Next

Both remaining limitations — first-order truncation and unbounded symbol growth
without a rigorous pruning story — are addressed by generalizing Eq.
\f$\eqref{eq:af-form}\f$ from a linear form to a polynomial of arbitrary order,
and pairing it with an explicit remainder interval. That is the subject of the
[Taylor model](./taylor.md) page.
