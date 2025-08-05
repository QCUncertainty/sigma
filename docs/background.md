# Background

## Linear Error Propagation

Like the [uncertainties](https://github.com/lmfit/uncertainties) and
[Measurements.jl](https://github.com/JuliaPhysics/Measurements.jl) libraries,
Sigma is based on the
[theory of linear error propagation](https://en.wikipedia.org/wiki/Propagation_of_uncertainty).
Here we will provide a brief summary of that theory.

Assume a sequence of \f$ n \f$ variables \f$ \left(a_{i}\right)_{i=1}^{n} \f$
whose elements are defined as \f$ a_{i} = \bar{a}_{i} \pm \sigma_{a_{i}} \f$, where
\f$ \bar{a}_{i} \f$ is the mean value of \f$ a_{i} \f$
and \f$ \sigma_{a_{i}} \f$ is the standard deviation of \f$ a_{i} \f$.
\f$ F(A) \f$ is a function of \f$ A \f$, where
\f$ A = \{a_{i} : i \in \mathbb{N}, 1 \leq i \leq n\} \f$.
The uncertainty of \f$ F(A) \f$ can be determined as

\f[ \normalsize
\sigma_{F} =
\sqrt{
  \sum_{i=1}^{n} \left(
    \left(
      \left.
        \frac{\partial F}{\partial a_{i}}
      \right|_{a_{i}=\bar{a}_{i}} \sigma_{a_{i}}
    \right)^2 +
    2 \sum_{j=i+1}^{n} \left(
      \left(\frac{\partial F}{\partial a_{i}}\right)_{a_{i}=\bar{a}_{i}}
      \left(\frac{\partial F}{\partial a_{j}}\right)_{a_{j}=\bar{a}_{j}}
      \sigma_{a_{i}a_{j}}
    \right)
  \right)
}
\f]

where \f$ \sigma_{a_{i}a_{j}} \f$ is the covariance of \f$ a_{i} \f$
and \f$ a_{j} \f$. These covariances can be eliminated from the above equation
if the uncertainties of the variables are independent from one another, which
is a requirement imposed here. As such, the uncertainty of \f$ F(A) \f$ when
the members of \f$ A \f$ are independent from one another is

\f[ \normalsize
\sigma_{F} =
\sqrt{
  \sum_{i=1}^{n} \left(
    \left.
      \frac{\partial F}{\partial a_{i}}
    \right|_{a_{i}=\bar{a}_{i}} \sigma_{a_{i}}
  \right)^2
}
\f]

Next, we consider a set \f$ B = \{x, y\} \f$ where \f$ x = a_{i} + a_{j} \f$ and
\f$ y = a_{j} - a_{k} \f$, i.e. the elements of \f$ B \f$ are dependent
on a subset of the independent variables in set \f$ A \f$. Given the function
\f$ G(B) \f$, the uncertainty of \f$ G \f$ can be determined by application of
the chain rule to relate the independent variables to \f$ G \f$ through their
relationships with the dependent variables

\f[ \normalsize
\sigma_{G} =
\sqrt{
  \left(
    \left(
      \frac{\partial G}{\partial x}
      \frac{\partial x}{\partial a_{i}}
    \right)_{a_{i}=\bar{a}_{i}} \sigma_{a_{i}}
  \right)^2 +
  \left(
    \left(
      \frac{\partial G}{\partial x}
      \frac{\partial x}{\partial a_{j}}
      +
      \frac{\partial G}{\partial y}
      \frac{\partial y}{\partial a_{j}}
    \right)_{a_{j}=\bar{a}_{j}} \sigma_{a_{j}}
  \right)^2 +
  \left(
    \left(
      \frac{\partial G}{\partial y}
      \frac{\partial y}{\partial a_{k}}
    \right)_{a_{k}=\bar{a}_{k}} \sigma_{a_{k}}
  \right)^2
}
\f]
