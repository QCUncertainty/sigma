# Examples

To use the Sigma library, simply `#include` the header `sigma/sigma.hpp`.
This imports the features of the `sigma` namespace, including the `Uncertain`
class and the operations that act on those objects. Sigma provides the typedefs
`UFloat` and `UDouble` for the classes `Uncertain<float>` and
`Uncertain<double>`. Below is a simple and complete example of using Sigma,
with more details in the examples that follow:

```cpp
#include <sigma/sigma.hpp>

int main(int argc, char *argv[]) {
    using ufloat = sigma::UFloat;   // same as sigma::Uncertain<float>
    using udouble = sigma::UDouble; // same as sigma::Uncertain<double>;

    udouble a{1.0, 0.1};
    udouble b{1.0, 0.2};

    b = b + 1.0;

    udouble c = a + b;
    std::cout << c << std::endl;    // Prints: 3+/-0.223607

    return 0;
}
```

## Construction of Uncertain Variables
To construct an uncertain variable of the `sigma::UDouble`, you must pass the
mean value of the variable and the value of the standard deviation.
```cpp
// An uncertain variable, equal to 10+/-0.2
sigma::UDouble x{10.0, 0.2};
```
Passing no values or only the mean results in values that are certain,
i.e. their standard deviation is 0.0.
```cpp
// Defaulted certain value equal to 0+/-0
sigma::UDouble y{};

// A certain variable, equal to 10+/-0
sigma::UDouble z{10.0};
```

## Element Access
The mean and standard deviation of an `Uncertain` instance can be accessed in a
read-only fashion with the `mean()` and `sd()` functions, respectively. These
elements cannot be directly manipulated, and are only updated by mathematical
operations.
```cpp
sigma::UDouble x{10.0, 0.2};
std::cout << x.mean() << std::endl; // Prints 10
std::cout << x.sd() << std::endl;   // Prints 0.2
```

## Equality and Comparison
Two `Uncertain` instances are considered equal if they have the same mean,
standard deviation, and dependencies. This means that two instances can have the
same mean and distribution and be nonequivalent because they are dependent on
different sources of error.
```cpp
sigma::UDouble a{1.0, 0.1};
sigma::UDouble b{1.0, 0.1};           // Different error source than a
sigma::UDouble c = a;                 // Same error source as a
bool different_error_source = (a==b); // False
bool same_error_source      = (a==c); // True
```
Comparisons between `Uncertain` instances are based on the mean values of the
variables, regardless of distribution.
```cpp
sigma::UDouble a{1.8, 0.1};
sigma::UDouble b{2.0, 1.0};
bool a_less_than_b = (a < b); // True
```
The `>=` and `<=` operators have a notable interaction with the equality
definition above:
```cpp
sigma::UDouble a{1.0, 0.1};
sigma::UDouble b{1.0, 0.1};
bool not_less_than_or_equal = (a <= b); // False
```
This boolean is `false` because the mean value of `a` is not less than that of
`b`, but the two values are not equal because they have different error sources.

## Mathematical Operations
The `Uncertain` class supports common arithmetic operations, as well as
equivalents for many of the functions found in the C++ standard header
`<cmath>`.
```cpp
sigma::UDouble a{1.0, 0.1};
sigma::UDouble b{2.0, 0.2};

auto c = a - b;                 // c = -1+/-0.223607
auto d = sigma::copysign(b, c); // d = -2+/-0.2
auto e = sigma::pow(a, 2);      // e = 1+/-0.2
```
For a complete list of functions, see [here](@ref sigma).

## Linear Algebra
Sigma has limited compatibility with the
[Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) library, which
provides support for a number of linear algebra operations.
```cpp
#include <Eigen/Dense>
using udouble_t = sigma::UDouble;
using umatrix_t = Eigen::Matrix<udouble_t, Eigen::Dynamic, Eigen::Dynamic>;

udouble_t a{1.0, 0.1};
udouble_t b{2.0, 0.2};
udouble_t c{3.0, 0.3};
udouble_t d{4.0, 0.4};
umatrix_t mat1(2, 2), mat2(2, 2);
mat1 << a, b, c, d;
mat2 << d, c, b, a;
auto mat3 = mat1 * mat2;
// mat3:
// 8+/-0.979796  5+/-0.616441
//20+/-2.46577  13+/-1.8868
```
Aside from basic arithmetic operations, the following decomposition methods have
been tested:
- LU (partial and full)
- Householder QR (full, column, and no pivoting)
- Cholesky (LLT and LDLT)
- Eigendecomposition (self-adjoint matrix only)

For details on %Eigen usage, see their
[documentation](https://eigen.tuxfamily.org/dox/).
