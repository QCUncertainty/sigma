# Taylor Models

## Motivation

The [Affine](./affine.md) page ended with two unresolved limitations. Affine
arithmetic is first order by construction, so every nonlinear operation
collapses all higher-order information into a single lumped error symbol; and
its term-pruning strategy controls symbol growth only by discarding
contributions outright, which gives up the guarantee that the result encloses
the true value.

Both follow from the same root cause: a linear form has nowhere to put quadratic
and higher information, and no place to record what was thrown away. A Taylor
model fixes both by generalizing the representation in two directions at once.
The linear form becomes a polynomial of arbitrary order \f$ n \f$, and an
explicit interval is carried alongside it to account for everything the
polynomial does not capture.

The approach is due to Berz and Makino, and this page follows their
[survey of validated inclusion methods](https://www.bmtdynamics.org/pub/papers/TMIJPAM03/TMIJPAM03.pdf).
It is the same differential algebra that underpins
[COSY INFINITY](https://www.bmtdynamics.org/cosy/) and, more recently,
[DACE](https://github.com/dacelib/dace).

The classes described here, `Taylor` and `TaylorModel`, are **not yet
implemented**. This page specifies the mathematics and the intended design so
the implementation can follow.

## The Taylor model

Let \f$ f \f$ be a function on a domain \f$ D \subset \mathbb{R}^v \f$ that is
\f$ n + 1 \f$ times continuously partially differentiable, let
\f$ x_0 \in D \f$, and let \f$ P \f$ be the order-\f$ n \f$ Taylor polynomial of
\f$ f \f$ about \f$ x_0 \f$. If the interval \f$ I \f$ satisfies

\f{equation}{ \normalsize
f(x) \in P\left(x - x_0\right) + I
\qquad
\forall x \in D
\label{eq:tm-inclusion}
\f}

then the pair \f$ (P, I) \f$ is an order-\f$ n \f$ Taylor model of \f$ f \f$
about \f$ x_0 \f$ on \f$ D \f$. Eq. \f$\eqref{eq:tm-inclusion}\f$ is called the
inclusion property, and it is the invariant that gives the representation its
meaning: the polynomial carries the shape of the function, the interval bounds
how far the truth can stray from that shape, and together they enclose \f$ f \f$
between two hypersurfaces over the whole domain. **Every operation defined below
exists to preserve Eq. \f$\eqref{eq:tm-inclusion}\f$.** An operation that
produces a tighter result at the cost of Eq. \f$\eqref{eq:tm-inclusion}\f$ has
not improved the answer, it has invalidated it.

Sigma normalizes each expansion variable to \f$ [-1, 1] \f$, exactly as the
[affine](./affine.md) representation does with its error symbols. This is the
standard change of variables \f$ \delta = (x - x_0) / \mathrm{rad}(D) \f$, and
it keeps the domain implicit, so a Taylor model needs no separate domain object.
For a single variable (\f$ v = 1 \f$) this convention makes \f$ P \f$ an
ordinary degree-\f$ n \f$ polynomial in \f$ \delta \f$, indexed by the usual
integer power \f$ k \f$

\f{equation}{ \normalsize
P(\delta) = \sum_{k=0}^{n} c_{k}\, \delta^{k}
\qquad
\delta \in [-1, 1]
\label{eq:tm-poly-1d}
\f}

with \f$ c_k \f$ the coefficient of \f$ \delta^k \f$.

For \f$ v > 1 \f$ variables, a single integer no longer says which variable a
power belongs to, so the exponent \f$ k \f$ generalizes to a **multi-index**
\f$ \beta = (\beta_1, \ldots, \beta_v) \f$, one non-negative integer per
variable. It plays the role of \f$ k \f$ in three ways: \f$ \delta^{\beta} \f$
abbreviates the monomial \f$ \delta_1^{\beta_1} \delta_2^{\beta_2} \cdots
\delta_v^{\beta_v} \f$; the total degree \f$ |\beta| = \beta_1 + \cdots +
\beta_v \f$ takes the place of \f$ k \f$ itself, so truncating at order
\f$ n \f$ means keeping every \f$ \beta \f$ with \f$ |\beta| \leq n \f$; and
each multi-index still has exactly one coefficient \f$ c_\beta \f$, so the
polynomial remains, as in the one-variable case, a finite sum of coefficient
times monomial. With this notation Eq. \f$\eqref{eq:tm-poly-1d}\f$ generalizes
directly

\f{equation}{ \normalsize
P(\delta) = \sum_{\left|\beta\right| \leq n} c_{\beta}\, \delta^{\beta}
\qquad
\delta \in [-1, 1]^v
\label{eq:tm-poly}
\f}

Comparing Eq. \f$\eqref{eq:tm-poly}\f$ with the affine form makes the
relationship to the earlier representations concrete

| representation | equivalent Taylor model |
| -------------- | ----------------------- |
| `Interval` | order 0, remainder equal to the whole bound |
| `Affine` | order 1, empty remainder |
| `TaylorModel` | order \f$ n \f$, remainder interval |

so the three pages of this series describe one construction at three truncation
orders rather than three unrelated ideas.

## Truncation and range bounding

Two operations recur throughout the arithmetic. The first is truncation: for a
polynomial \f$ Q \f$ we write \f$ Q_{\leq n} \f$ for the terms of total degree
at most \f$ n \f$ and \f$ Q_{>n} \f$ for the rest. The second is range bounding:
for a polynomial \f$ Q \f$ we write \f$ B(Q) \f$ for an interval enclosing the
values of \f$ Q \f$ over the domain

\f{equation}{ \normalsize
B(Q) \supseteq
\left\{ Q(\delta) : \delta \in [-1, 1]^v \right\}
\label{eq:tm-bound}
\f}

Any valid \f$ B \f$ makes the arithmetic below correct, but a sharper \f$ B \f$
makes it more useful. The requirement is that \f$ B \f$ be at least as sharp as
evaluating \f$ Q \f$ directly in interval arithmetic. Sigma's current
implementation is Berz and Makino's quadratic fast bounder (QFB): it bounds
each variable's own linear and quadratic contribution exactly, in closed
form, and falls back to evaluating every other term separately in
[`Interval`](./interval.md) arithmetic for the rest. The rest of this section
builds up to that implementation — the naive per-term baseline it falls back
on, an alternative (Horner form) that was considered and rejected, and QFB
itself.

### Naive baseline

The simplest valid \f$ B \f$ substitutes \f$[-1,1]\f$ for every expansion
variable, evaluates each monomial's power via [`Interval`](./interval.md)'s
`pow`, multiplies by its coefficient, and sums the resulting intervals term by
term. It is cheap and always valid, but it is not tight, because it is
susceptible to the classic interval-arithmetic *dependency problem*: if the
same variable appears in more than one term, each occurrence is bounded
independently, so the bound implicitly (and wrongly) allows different
occurrences of the same symbol to take different values at once. This naive
evaluation is still what Sigma's actual implementation, described below,
falls back to for whatever it cannot bound exactly.

A minimal example makes this concrete. Multiply two order-1 models with
independent expansion variables, \f$x = 2 + \delta_x\f$ (representing
\f$x \in [1,3]\f$) and \f$y = 3 + \delta_y\f$ (representing
\f$y \in [2,4]\f$); the exact product expands to

\f{equation}{ \normalsize
P(\delta_x, \delta_y) = 6 + 3\delta_x + 2\delta_y + \delta_x \delta_y
\label{eq:tm-bound-example}
\f}

The true range of Eq. \f$\eqref{eq:tm-bound-example}\f$ over the box
\f$[-1,1]^2\f$ is \f$[2, 12]\f$ (a bilinear function's extrema occur at the
box's corners; e.g. \f$\delta_x = \delta_y = -1 \Rightarrow 6-3-2+1=2\f$,
matching \f$x=1, y=2 \Rightarrow xy=2\f$). The naive baseline instead
computes \f$6 + [-3,3] + [-2,2] + [-1,1] = [0, 12]\f$: loose on the lower end,
because it sums the \f$-3\f$ attained by \f$3\delta_x\f$ at
\f$\delta_x = -1\f$ with the \f$-1\f$ attained by \f$\delta_x\delta_y\f$ at
\f$\delta_x = 1, \delta_y = -1\f$ — the *opposite* sign of \f$\delta_x\f$ —
as if both could hold at once. This is exactly what computing
`bound(x * y)` for `Taylor(1,3) * Taylor(2,4)` returns today.

### Horner form

A tighter option for a univariate polynomial is to evaluate it in nested
(Horner) form,

\f{equation}{ \normalsize
P(\delta) = c_0 + \delta\left(c_1 + \delta\left(c_2 + \cdots +
\delta\, c_n\right)\right)
\label{eq:tm-horner}
\f}

substituting a single \f$[-1,1]\f$ interval and reusing that same interval
object at every nesting level, rather than computing each power
independently. This reduces the dependency problem — reusing one interval
through repeated multiplication is tighter than bounding \f$c_k\delta^k\f$
separately for every \f$k\f$ — but it does not eliminate it: a polynomial
whose terms cancel, such as \f$\delta - \delta\f$, still evaluates to
\f$[-1,1] - [-1,1] = [-2,2]\f$ rather than the true value \f$0\f$, because
the two occurrences of \f$\delta\f$ are still bounded as if independent.

More importantly for Sigma, Horner form is naturally a *single-variable*
nesting: it has no canonical generalization to a sparse polynomial over
arbitrarily many expansion variables, the way \f$P\f$ in Eq.
\f$\eqref{eq:tm-poly}\f$ is. That mismatch is why Sigma looked to Berz and
Makino's dominated and fast bounders instead.

### Dominated and fast bounders

Berz and Makino's survey (linked above) describes a family of sharper
bounders, each splitting \f$P = L + R\f$ so that \f$L\f$, some low-order part
of \f$P\f$, is bounded exactly, leaving only the (typically smaller,
higher-order) remainder \f$R\f$ for the naive fallback above. The **linear
dominated bounder (LDB)** takes \f$L\f$ to be \f$P\f$'s constant and linear
(\f$|\beta| \leq 1\f$) part; because each \f$\delta_i\f$ appears exactly once
in \f$L\f$, its exact range over \f$[-1,1]^v\f$ has a closed form with *zero*
dependency-problem overestimation

\f{equation}{ \normalsize
B_{\mathrm{LDB}}(P) =
\left[c_0 - \sum_i \left|c_{e_i}\right|,\;\;
c_0 + \sum_i \left|c_{e_i}\right|\right] + B(R)
\label{eq:tm-ldb}
\f}

Sigma implements the next refinement in the family, the **quadratic fast
bounder (QFB)**, directly (`Taylor::bound()`), rather than LDB itself: it
extends \f$L\f$ one degree further, to each variable \f$\delta_i\f$'s
diagonal contribution \f$c_{e_i}\delta_i + c_{2e_i}\delta_i^2\f$ (the linear
and pure-quadratic terms in \f$\delta_i\f$ alone). That diagonal contribution
still has an exact, closed-form extremum over \f$\delta_i \in [-1,1]\f$, found
by completing the square: the parabola's vertex, clipped to \f$[-1,1]\f$, or
an endpoint if the vertex falls outside it. Summing these exact per-variable
extrema still incurs no cross-variable dependency-problem overestimation,
since each is evaluated on its own independent \f$\delta_i\f$; only
cross-quadratic and degree-\f$\geq 3\f$ terms are left in \f$R\f$, bounded by
the naive fallback described above — which is exactly why the `x*y` example
there still comes out to \f$[0,12]\f$ under QFB: neither \f$\delta_x\f$ nor
\f$\delta_y\f$ has a pure-quadratic term of its own, so QFB's diagonal split
captures nothing there and the whole product falls to the naive fallback,
unchanged. LDB itself was never implemented, since QFB is a strict
refinement of it and was adopted directly.

## Arithmetic

Addition and subtraction act independently on the two parts, since both are
linear

\f{equation}{ \normalsize
\left(P_1, I_1\right) + \left(P_2, I_2\right) =
\left(P_1 + P_2,\; I_1 + I_2\right)
\label{eq:tm-add}
\f}

Multiplication is where the design shows itself. The exact product of two
order-\f$ n \f$ polynomials has order \f$ 2n \f$, so it does not fit the
representation and must be split. The low-order part is kept as the new
polynomial, and everything else — the truncated tail, the cross terms between
each polynomial and the other's remainder, and the product of the remainders —
is bounded into the new remainder

\f{equation}{ \normalsize
\left(P_1, I_1\right) \cdot \left(P_2, I_2\right) =
\left(
  \left(P_1 P_2\right)_{\leq n},\;
  B\left(\left(P_1 P_2\right)_{>n}\right) +
  B\left(P_1\right) I_2 +
  B\left(P_2\right) I_1 +
  I_1 I_2
\right)
\label{eq:tm-mul}
\f}

Eq. \f$\eqref{eq:tm-mul}\f$ is worth reading against the corresponding affine
result. Affine multiplication also produces a quadratic term it cannot store,
and also bounds it with a single scalar. The difference is that a Taylor model
of order \f$ n \geq 2 \f$ *keeps* the quadratic term as an explicit coefficient
and only bounds what lies above order \f$ n \f$. Raising the order moves the
boundary between "tracked exactly" and "bounded", and the remainder shrinks
accordingly.

Note also what Eq. \f$\eqref{eq:tm-mul}\f$ does **not** do: it never discards
the tail. The truncated terms are bounded into \f$ I \f$, so Eq.
\f$\eqref{eq:tm-inclusion}\f$ survives the operation. This is precisely the
discipline that [`ThresholdedAffine` pruning](./affine.md) gives up.

Division is not a primitive operation. As in the affine case, it is composed
from multiplication and the multiplicative inverse, so only the inverse needs a
definition, and it is supplied by the intrinsic machinery below.

## Elementary functions

Recall \f$ f \f$ from the definition of the Taylor model above: it is the
function that \f$ (P, I) \f$ models, expanded about \f$ x_0 \f$. This section
builds a Taylor model of the composition \f$ g(f) \f$ for an elementary
function \f$ g \f$ such as \f$ \exp \f$, \f$ \log \f$, or \f$ \sqrt{\cdot} \f$;
\f$ g \f$ is called the *outer* function because it is applied on the outside
of \f$ f \f$, and \f$ f \f$ itself — the Taylor-model-valued argument — is the
*inner* one.

All elementary functions follow one recipe. Split the polynomial's constant
part, writing \f$ c_f \f$ for the value of \f$ f \f$ at the expansion point
\f$ x_0 \f$ — equivalently, the coefficient of the \f$ \beta = 0 \f$ term of
\f$ P \f$, since that is what a Taylor polynomial's constant term always is —
and \f$ \bar{P} = P - c_f \f$ for the rest of the polynomial, so that
\f$ (\bar{P}, I) \f$ is a Taylor model of \f$ \bar{f} = f - c_f \f$. Then
Taylor-expand the outer function \f$ g \f$ about \f$ c_f \f$

\f{equation}{ \normalsize
g(f) = \sum_{k=0}^{\infty}
\frac{1}{k!} \frac{d^{k} g}{d f^{k}}\left(c_f\right) \bar{f}^{\,k}
\label{eq:tm-outer-series}
\f}

Eq. \f$\eqref{eq:tm-outer-series}\f$ is evaluated using Taylor model
arithmetic, i.e. Eq. \f$\eqref{eq:tm-add}\f$ and Eq. \f$\eqref{eq:tm-mul}\f$,
with \f$ \bar{f} \f$ represented by the Taylor model \f$ (\bar{P}, I) \f$.

The reason this terminates cleanly is a small observation with large
consequences. Because \f$ \bar{P} \f$ has **no constant term**, every one of
its monomials has degree at least 1, so a product of \f$ k \f$ of them —
which is what \f$ \bar{P}^k \f$ expands into — has degree at least \f$ k \f$
as well: \f$ \bar{P}^k \f$ has no terms of degree below \f$ k \f$. Put another
way, raising \f$ \bar{P} \f$ to a higher power pushes its lowest surviving
degree higher, i.e. its terms climb up out of the truncation window as
\f$ k \f$ grows. So for \f$ k > n \f$ the polynomial part of \f$ \bar{P}^k \f$
vanishes entirely under truncation, and every term of the outer series beyond
order \f$ n \f$ contributes to the remainder only. The series therefore
splits exactly into a finite polynomial computation plus an interval.

Splitting Eq. \f$\eqref{eq:tm-outer-series}\f$ at \f$ k = n \f$ makes this
precise

\f{equation}{ \normalsize
g(f) =
\sum_{k=0}^{n} \frac{1}{k!} \frac{d^{k}g}{df^{k}}\left(c_f\right)
\bar{f}^{\,k}
+
R_n
\label{eq:tm-outer-split}
\f}

The first term is exactly the truncated polynomial part identified above, so it
is computed by repeated Taylor model addition and multiplication, i.e. Eq.
\f$\eqref{eq:tm-add}\f$ and Eq. \f$\eqref{eq:tm-mul}\f$. The second term,
\f$ R_n \f$, is the Lagrange form of the remainder; since its polynomial part
vanishes under truncation, it need not be tracked as a polynomial at all, only
bounded, which interval arithmetic does directly by letting \f$ \xi \f$ range
over the whole enclosure of \f$ f \f$

\f{equation}{ \normalsize
R_{n} \in
\frac{1}{\left(n+1\right)!}
\left(B\left(\bar{P}\right) + I\right)^{n+1}
\frac{d^{n+1}g}{df^{n+1}}\Big(
  c_f + \left[0, 1\right] \cdot \left(B\left(\bar{P}\right) + I\right)
\Big)
\label{eq:tm-outer-rem}
\f}

Taking the exponential as the worked example specializes Eq.
\f$\eqref{eq:tm-outer-split}\f$ and Eq. \f$\eqref{eq:tm-outer-rem}\f$: every
derivative of \f$ \exp \f$ is \f$ \exp \f$ itself, so
\f$ \frac{d^{k}\exp}{df^{k}}\left(c_f\right) = \exp\left(c_f\right) \f$ for
every \f$ k \f$, and the addition theorem together with the Taylor series of
\f$ \exp \f$ give

\f{equation}{ \normalsize
\exp\left(f\right) =
\exp\left(c_f\right)
\left\{
  1 + \bar{f} + \frac{1}{2!}\bar{f}^{\,2} + \cdots +
  \frac{1}{n!}\bar{f}^{\,n}
\right\} +
R_{n}
\label{eq:tm-exp}
\f}

where the braced expression is a polynomial in \f$ \bar{f} \f$, computed by
repeated Taylor model addition and multiplication, and \f$ R_n \f$ collects the
tail. The tail is bounded by the Lagrange form of the remainder, evaluated in
interval arithmetic over the range of the argument

\f{equation}{ \normalsize
R_{n} \in
\exp\left(c_f\right)
\frac{1}{\left(n+1\right)!}
\left(B\left(\bar{P}\right) + I\right)^{n+1}
\exp\left(\left[0, 1\right] \cdot \left(B\left(\bar{P}\right) + I\right)\right)
\label{eq:tm-exp-rem}
\f}

Eq. \f$\eqref{eq:tm-exp-rem}\f$ is ordinary [interval](./interval.md) arithmetic
on a quantity whose polynomial part is known to vanish, which is why the whole
tail can be collapsed into a single interval without loss of validity.

Every other elementary function is the same computation with a different outer
series and a different Lagrange bound. Several are reached indirectly through an
addition formula chosen so that the inner argument has no constant part, which
is what makes the vanishing-polynomial argument apply

| function | outer series | precondition |
| -------- | ------------ | ------------ |
| \f$ \exp \f$ | direct series, scaled by \f$ \exp(c_f) \f$ | none |
| \f$ \log \f$ | series in \f$ \bar{f}/c_f \f$ | \f$ B(P) + I \subset (0, \infty) \f$ |
| \f$ 1/f \f$ | alternating series in \f$ \bar{f}/c_f \f$ | \f$ 0 \notin B(P) + I \f$ |
| \f$ \sqrt{f} \f$, \f$ 1/\sqrt{f} \f$ | binomial series | \f$ B(P) + I \subset (0, \infty) \f$ |
| \f$ \sin \f$, \f$ \cos \f$ | addition theorem; remainder sign cycles with \f$ k \bmod 4 \f$ | none |
| \f$ \sinh \f$, \f$ \cosh \f$ | addition theorem | none |
| \f$ \arcsin \f$ | addition formula, then series in \f$ g = f\sqrt{1 - c_f^2} - c_f\sqrt{1 - f^2} \f$ | \f$ B(P) + I \subset (-1, 1) \f$ |
| \f$ \arccos \f$ | \f$ \pi/2 - \arcsin \f$ | \f$ B(P) + I \subset (-1, 1) \f$ |
| \f$ \arctan \f$ | addition formula, then series in \f$ g = \bar{f}/(1 + c_f f) \f$ | none |

The preconditions are genuine requirements, not conveniences. They are stated in
terms of \f$ B(P) + I \f$ — the full enclosure including the remainder — because
it is the enclosure, not the polynomial alone, that must avoid the singularity.
An implementation must check them and throw, in the same manner as the
corresponding `Affine` operations.

Antiderivation deserves a mention because it is nearly free and it is what makes
verified integration possible. Integrating with respect to variable \f$ i \f$
raises every degree by one, so the order-\f$ n \f$ terms would overflow the
truncation; they are bounded into the remainder instead

\f{equation}{ \normalsize
\partial_i^{-1}\left(P, I\right) =
\left(
  \int_0^{\delta_i} P_{\leq n-1}\, d\delta_i,\;
  \left(B\left(P_{>n-1}\right) + I\right) \cdot w_i
\right)
\label{eq:tm-antideriv}
\f}

where \f$ w_i \f$ is the width of the domain in the \f$ i \f$-th variable. Note
that Eq. \f$\eqref{eq:tm-antideriv}\f$ is stated for \f$(P, I)\f$, i.e. it
belongs to `TaylorModel` (not yet implemented), where the overflowing terms
have a remainder to be bounded into. `Taylor` alone has no such remainder, and
nothing else on this page depends on antiderivation — elementary functions
need only `compose_` plus Taylor model addition and multiplication (Eq.
\f$\eqref{eq:tm-add}\f$, Eq. \f$\eqref{eq:tm-mul}\f$) — so it was left out of
`Taylor`'s implementation and can be added later, alongside `TaylorModel`,
if verified integration is actually needed.

## Order scaling

The payoff is a theorem about how fast the remainder shrinks. Suppose the inputs
to a computation are Taylor models over a domain of width \f$ h \f$ whose
remainders are \f$ O(h^{n+1}) \f$. Then the remainders of their sums, their
products, and the results of every elementary function above are also
\f$ O(h^{n+1}) \f$

\f{equation}{ \normalsize
I_{f+g} = O\left(h^{n+1}\right)
\qquad
I_{f \cdot g} = O\left(h^{n+1}\right)
\qquad
I_{s(f)} = O\left(h^{n+1}\right)
\label{eq:tm-scaling}
\f}

Eq. \f$\eqref{eq:tm-scaling}\f$ is the \f$ (n+1) \f$-st order scaling property,
and it holds for whole computations, not merely single operations, because the
property is preserved by composition. It is the precise sense in which the
progression of these three pages is an improvement

| representation | overestimation |
| -------------- | -------------- |
| `Interval` | \f$ O(h) \f$ |
| `Affine` | \f$ O(h^2) \f$ |
| order-\f$ n \f$ `TaylorModel` | \f$ O(h^{n+1}) \f$ |

The three rows are one formula evaluated at \f$ n = 0 \f$, \f$ n = 1 \f$, and
general \f$ n \f$. Halving the domain reduces interval overestimation by a
factor of two and affine overestimation by four; for an order-5 model it falls
by a factor of 64.

## Design of the `Taylor` class

The design separates the polynomial algebra from the rigorous enclosure, giving
two classes that compose in the same way `Affine` and `ThresholdedAffine` do.
`Taylor` implements Eq. \f$\eqref{eq:tm-poly}\f$ — a truncated polynomial and
nothing else. It is cheap, it is useful on its own for sensitivity analysis and
automatic differentiation, and it makes no claim to enclose anything.
`TaylorModel` pairs a `Taylor` with a remainder interval and maintains Eq.
\f$\eqref{eq:tm-inclusion}\f$.

Three requirements shaped the design. The truncation order must be **per
object**, so that different quantities in the same program can carry different
orders; the class must work for **any floating-point type**, following the
`ValueType` template parameter used everywhere else in Sigma; and there must be
**no global state**. These are the reasons Sigma implements its own rather than
binding to an existing differential algebra library — DACE, the most mature
option, fixes one truncation order and variable count process-wide through a
global initialization call and is not templated on the scalar type.

### Internal state

```cpp
using size_type   = std::size_t;
using value_t     = ValueType;
using deviation_t = size_type;                       // opaque, like error_term_t
using monomial_t  = Monomial;                        // sparse exponent multi-index
using coeffs_t    = std::map<monomial_t, value_t>;
using interval_t  = Interval<value_t>;

std::optional<value_t> m_constant_;  // nullopt when empty, as in Affine
coeffs_t               m_coeffs_;    // non-constant terms only
size_type              m_order_;     // per-object truncation order
```

The constant term is stored apart from the rest because every elementary
function in the table above begins by splitting it off. Holding it in a
`std::optional` and letting an empty optional denote the empty set follows
`Affine` exactly, so `empty` and a private `assert_not_empty_` helper carry over
unchanged.

The multi-index type deserves its own small class. Expansion variables are
minted dynamically and a given model touches only a few of them, so a dense
exponent vector would be mostly zeros; `monomial_t` should instead wrap a sparse
map from variable to exponent and provide comparison, hashing, total degree, and
multi-index addition. Isolating it gives one place to test the index algebra,
which is otherwise easy to get subtly wrong.

The coefficient container is deliberately a `std::map` rather than the
`std::unordered_map` that `Affine` uses for its error terms. A Taylor model
carries far more terms than an affine form, and summing them in a deterministic
order — which an ordered container guarantees and a hash container does not —
keeps results reproducible across runs and platforms.

Expansion variables are minted exactly as affine error symbols are, by a static
`make_deviation` returning successive values from a function-local `std::atomic`
counter. Sharing a variable between two models is what expresses correlation, so
global uniqueness matters here for the same reason it does for the error symbols
of the [affine](./affine.md) representation.

### Interface

`Taylor` should mirror the `Affine` surface — the compound-assignment and free
arithmetic operators in both scalar and same-type overloads, unary negation,
equality, `range`, `contains`, `empty`, stream insertion, an `eigen_compat.hpp`,
and `TFloat`/`TDouble` typedefs. Beyond that it adds

| member | purpose |
| ------ | ------- |
| `order` | the truncation order of this object |
| `constant` | \f$ c_f \f$, the order-0 coefficient |
| `coefficients`, `n_terms` | access to Eq. \f$\eqref{eq:tm-poly}\f$ |
| `bound` | \f$ B(P) \f$ of Eq. \f$\eqref{eq:tm-bound}\f$ |
| `derivative` | ordinary term-by-term polynomial differentiation |
| `truncate` | drop terms above a given order |
| `make_deviation` | mint a fresh expansion variable |

One hook carries the weight of every nonlinear operation, and it is the direct
generalization of the affine `apply_affine_transform`. Where the affine version
takes two coefficients and one lumped error radius, its Taylor counterpart takes
the outer function's Taylor coefficients about \f$ c_f \f$

```cpp
// Affine:  alpha, zeta (a linear model) + delta (everything else)
// Taylor:  n+1 outer coefficients about c_f; the tail becomes the remainder
my_t compose_(const std::vector<value_t>& outer_coeffs) const;
```

Every entry in the elementary function table then reduces to building a
coefficient list and calling `compose_`, so the operations headers become tables
of series coefficients rather than bespoke derivations. Following the existing
layout, `operations/exponents.{hpp,ipp}` would cover the exponential, logarithm,
root, and power functions, with a new `operations/trigonometry.{hpp,ipp}` for
the trigonometric, hyperbolic, and inverse families. As in `Affine`, `pow`
should be composed from `log` and `exp` rather than given its own series.

A constructor taking a center and an order collides with the constructor taking
a lower and upper bound, since an integer literal converts to either parameter
type. `ThresholdedAffine` already solves this problem for its threshold, and the
same tag-type approach applies

```cpp
struct Order {
    size_type value;
    explicit Order(size_type v) : value(v) {}
};
static constexpr Order default_order() { return Order(2); }
```

with `Order` supplied as a trailing defaulted argument on each constructor.

## Achieving rigorous bounds

`TaylorModel` holds a `Taylor` and an `interval_t`, and guarantees Eq.
\f$\eqref{eq:tm-inclusion}\f$ for every \f$ \delta \in [-1,1]^v \f$.
Structurally it follows `ThresholdedAffine`: composition rather than
inheritance, forwarding accessors, an accessor exposing the underlying
polynomial, a constructor that re-wraps a polynomial and a remainder, mutating
operators that delegate and then repair the remainder, and free-function
elementary operations at the bottom of the same header rather than in an
`operations` subdirectory.

The arithmetic is Eq. \f$\eqref{eq:tm-add}\f$ and Eq. \f$\eqref{eq:tm-mul}\f$;
the elementary functions are `compose_` followed by the appropriate Lagrange
interval, as in Eq. \f$\eqref{eq:tm-exp-rem}\f$. Two members have no affine
analogue, and they are what close the gap left by
[`ThresholdedAffine`](./affine.md).

`sweep_to_order` reduces the polynomial to a lower order by bounding the removed
terms with \f$ B \f$ and adding that interval to the remainder. `sweep_small`
does the same for individual terms whose contribution falls below a threshold.
Both serve the purpose that [affine](./affine.md) thresholding serves —
controlling how many terms are carried — but because the removed contributions
are bounded into \f$ I \f$ rather than deleted, Eq.
\f$\eqref{eq:tm-inclusion}\f$ still holds afterward. The enclosure gets wider,
never invalid. That is the correct trade, and it is available only because the
representation has somewhere to put what it gives up.

Two points require care in any implementation.

**Mixed orders resolve downward.** Combining an order-\f$ n_1 \f$ model with an
order-\f$ n_2 \f$ model yields order \f$ \min(n_1, n_2) \f$. Higher-order
information about the lower-order operand does not exist and cannot be
manufactured, so the excess terms of the higher-order operand must be swept into
the remainder exactly as `sweep_to_order` does. Dropping them instead would
break Eq. \f$\eqref{eq:tm-inclusion}\f$ — the same failure mode as unbounded
pruning.

**Floating-point rounding is a separate concern.** Eq.
\f$\eqref{eq:tm-inclusion}\f$ speaks of exact real arithmetic. An implementation
that computes coefficients in ordinary floating-point arithmetic commits
rounding errors that the remainder does not account for, so the result is
rigorous with respect to the modeled uncertainty but not with respect to its own
computation. `Affine` has the same limitation and documents it. Closing the gap
requires either outward-rounded coefficient arithmetic or a rounding budget
accumulated into the remainder; the design should leave a hook for it rather
than imply a guarantee that is not yet delivered.

## Future work

Three areas are known to be open.

Rigor with respect to rounding, as just described, is the most significant.
Until it is addressed, `TaylorModel` should be documented as enclosing the
modeled function under exact arithmetic.

Sharper range bounding would still improve every operation, since \f$ B \f$
appears throughout Eq. \f$\eqref{eq:tm-mul}\f$, Eq. \f$\eqref{eq:tm-exp-rem}\f$,
and Eq. \f$\eqref{eq:tm-antideriv}\f$. "Truncation and range bounding" above
walks through the naive baseline, and Sigma's quadratic fast bounder (QFB),
which now bounds every polynomial's diagonal linear and quadratic terms
exactly. What's left is what QFB leaves to the naive fallback: cross terms
and terms of degree \f$\geq 3\f$. Extending the same exact-bounding idea to
cross terms is the natural next refinement.

Term count grows combinatorially. A model of order \f$ n \f$ in \f$ v \f$
variables admits

\f{equation}{ \normalsize
\binom{n + v}{n}
\label{eq:tm-terms}
\f}

monomials, so both order and variable count are expensive. Eq.
\f$\eqref{eq:tm-terms}\f$ is the reason `sweep_small` exists, and choosing a
policy for when to sweep is likely to need empirical tuning against real
computations rather than a fixed default.

When the implementation lands it should live in `include/sigma/taylor/` with
tests under `tests/unit_tests/taylor/`, mirroring the existing layout. No
build-system changes are required — the CMaize configuration globs headers and
test sources recursively — and the only existing file needing modification is
`include/sigma/sigma.hpp`, which must include the new header to export the
classes.
