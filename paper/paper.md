---
title: 'Sigma: Uncertainty Propagation for C++'
tags:
  - C++
  - Uncertainty Propagation
authors:
  - name: Jonathan M. Waldrop
    orcid: 0000-0003-0442-193X
    corresponding: true
    affiliation: "1"
  - name: Ryan M. Richard
    orcid: 0000-0003-4235-5179
    affiliation: "1, 2"
affiliations:
  - name: Chemical and Biological Sciences, Ames National Laboratory, USA
    index: 1
    ror: 041m9xr71
  - name: Department of Chemistry, Iowa State University, USA
    index: 2
    ror: 04rswrd78
date: 1 October 2024
bibliography: paper.bib

---

# Summary

Sigma is a header-only C++-17 library for uncertainty propagation, 
inspired by `uncertainties` [@uncertainties] for Python and
`Measurements.jl` [@Measurements.jl-2016] for Julia. The library tracks the 
functional correlation between dependent and independent variables, ensuring
that the uncertainty of the independent variables is properly considered in the
calculation of the dependent variables' uncertainties. It is intended as a near 
drop-in replacement for the standard floating point types (aside from 
uncertainty specification), and aims to be easily interoperable with the 
existing standard types.

# Statement of need

In scientific analysis, values are often paired with the degree of uncertainty
in the accuracy of that value. This uncertainty (or error) could be derived from
a number of sources, including the level of accuracy provided by a measuring
instrument, the statistical nature of the value being measured, or
approximations made in the determination of the value. Often, this uncertainty
is represented as the standard deviation of the value. When using these values
as function inputs, they convey an uncertainty on the new results. Propagating
the uncertainty by hand can be tedious, possibly prohibitively so in the case of
calculations that require machine computation to be feasible. As such, it has
been found prudent to automate the propagation of error as an extension of the
calculations themselves [@uncertainties; @Measurements.jl-2016]. To the best of
our knowledge, there is no currently maintained C++ library to facilitate this
kind of uncertainty propagation. As C++ is an important language in the
development of scientific software and high-performance computing, Sigma has
been developed in an attempt to fill this gap.

# Mathematics

Assume $F(A)$ is a function of $A$, where $A$ is a set whose elements are some 
or all of the elements of the sequence of $n$ variables 
$\left(a_{i}\right)_{i=1}^{n}$. These elements are defined as 
$a_{i} = \bar{a}_{i} \pm \sigma_{a_{i}}$, where $\bar{a}_{i}$ is the mean value
of the variable and $\sigma_{a_{i}}$ is called the uncertainty and is assumed to
represent an error measure closely related to the standard deviation of a random
variable. The linear uncertainty of $F(A)$ can be determined as 
$$
\sigma_{F} \approx
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
}.
$$
Note that for any element $a_{i}$ that is not a member of $A$,
$\frac{\partial F}{\partial a_{i}} = 0$ and those terms vanish in the
summations. The term $\sigma_{a_{i}a_{j}}$ is the covariance of $a_{i}$ and 
$a_{j}$, defined as
$$
\sigma_{a_{i}a_{j}} = 
E[\left(a_{i} - E[a_{i}]\right)\left(a_{j} - E[a_{j}]\right)],
$$
where $E[a_{i}]$ is the expectation value of $a_{i}$. The covariances can be 
eliminated from the above equation if the uncertainties of the variables are 
independent from one another, which is a requirement imposed here. As such, the
uncertainty of $F(A)$ when the members of $A$ are independent from one another
is simply
$$
\sigma_{F} \approx
\sqrt{
  \sum_{a_{i} \in A} \left(
    \left.
      \frac{\partial F}{\partial a_{i}}
    \right|_{a_{i}=\bar{a}_{i}} \sigma_{a_{i}}
  \right)^2
}.
$$

Next, we consider a set $B = \{x, y\}$ where $x = x(a_{i}, a_{j})$ and
$y = y(a_{j})$, i.e. the elements of $B$ are functions of some number of 
independent variables. As the values of $x$ and $y$ are dependent on the values
of $a_{i}$ and $a_{j}$, they are said to be *functionally correlated* to the
independent variables [@Measurements.jl-2016] and their uncertainties are easily
calculated from the previous equation. Given the function $G(B)$, 
the value of $\sigma_{G}$ cannot be calculated from the previous equation as it
does not account for the functional correlation of the elements of $B$. The
uncertainty of $G$ can be properly determined by application of the chain rule 
to relate the independent variables to $G$ through their relationships with the
dependent variables
$$
\sigma_{G} \approx 
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
  \right)^2
}.
$$

# Usage

Sigma is header-only, so it only needs to be findable by the dependent project 
to be used. The library is buildable with CMake [@CMake] and utilizes the 
CMaize [@CMaize] extension to handle configuration, dependency 
management, and building the tests and/or documentation. To use the library in a
project, simply add `#include <sigma/sigma.hpp>` in an appropriate location 
within the project's source.

The primary component of Sigma is the `Uncertain<T>` class, templated on
the floating point type used to represent the mean and uncertainty of the
variable. A simple construction of an uncertain floating point value can be
accomplished by passing the mean and a value for the uncertainty (such as a 
standard deviation):
```cpp
using numeric_t = double;
numeric_t a_mean{100.0};
numeric_t a_sd{1.0};
sigma::Uncertain<numeric_t> a{a_mean, a_sd};
std::cout << a << std::endl;    // Prints: 100+/-1
```
The same can be accomplished in a less verbose way as 
`sigma::Uncertain a{100.0, 1.0}`. Sigma also provides the typedefs `UFloat`
and `UDouble` (uncertain `float` and `double`, respectively) for convenience.

Basic arithmetic with certain or uncertain values is accomplished trivially,
```cpp
sigma::Uncertain a{1.0, 0.1};
sigma::Uncertain b{2.0, 0.2};
auto c = a + 2.0; // 3.0+/-0.1
auto d = a * 2.0; // 2.0+/-0.2
auto e = a + b;   // 3.0+/-0.2236
auto f = a * b;   // 2.0+/-0.2828
```
The resulting variables here are functionally correlated to `a` and/or `b`,
meaning the operation `e - c` would return an instance with the value 
$0\pm0.2$ as the contributions from `a` would exactly negate each other.

Sigma also implements many of the most common math functions found in the C++
standard library, such as those for trigonometry and rounding:
```cpp
sigma::Uncertain radians{0.785398, 0.1};
sigma::Uncertain degrees{45.0, 0.1};
sigma::Uncertain decimal{1.2, 0.1};
auto to_degrees = sigma::degrees(radians); // 45.0000+/-5.7296
auto in_radians = sigma::radians(degrees); //  0.7854+/-0.0017
auto tangent    = sigma::tan(radians);     //  1.0000+/-0.2000
auto truncated  = sigma::trunc(decimal);   //  1.0+/-0.0
```
Sigma also has a limited degree of compatibility with the Eigen library [@Eigen],
allowing for matrix operations and a number of linear solvers. Additional 
functionality is possible, though not currently ensured. Linear algebra usage is
partially limited by Sigma only being suitable for the representation of real 
numbers, but support for complex numbers is intended at a later time.

While inspired by `uncertainties` and `Measurements.jl`, Sigma is not a 
one-to-one translation of either package. Sigma implements a method of tracking
variable dependence similar to those found in the other packages, with each
instance tracking the independent variables that it depends on and the 
contributions of those variables to the instance's uncertainty. Function names 
in Sigma use the naming convention of the C++ Standard Library where applicable,
and may differ from the names used in the other packages. `uncertainties` 
specifically allows for the alteration of an independent variable's uncertainty 
and on-the-fly re-evaluation for dependent variables. Sigma does not replicate 
this feature, preferring a static evaluation of uncertainties that is more 
consistent with the expected behavior for C++'s floating types.

# Acknowledgements

This work was supported by the Ames National Laboratory's Laboratory Directed 
Research and Development (LDRD) program. The Ames Laboratory is operated for the
U.S. DOE by Iowa State University under contract # DE-AC02-07CH11358.

# References
