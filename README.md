# Sigma 

Sigma is a header-only C++ library for uncertainty propagation throughout
mathematical operations on floating point values, inspired by
[uncertainties](https://github.com/lmfit/uncertainties) for Python and
[Measurements.jl](https://github.com/JuliaPhysics/Measurements.jl) for Julia.

## Motivation
Simplify tracking of uncertainty through mathematical operations, such as those
in the computational simulation of chemical systems.

See [Background](./docs/background.md) for more details.

## Features
- Propagate uncertainty throughout calculations
- Track dependent variable correlation to independent variables
- Support for common mathematical operations.
- Limited compatibility with [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
  for linear algebra support.

See [here](./docs/examples.md) for examples.

## Installation
Sigma is header-only, so installation is not strictly required for usage. The
files simply need to be locatable by your project, as well the dependencies
required by Sigma itself (e.g. Eigen if support is configured). Dependency
gathering, testing, building of the documentation, and installation are all
handled with CMake and the [CMaize](https://github.com/CMakePP/CMaize) 
Framework.

```Bash
# -- Configuration Step --
# Should the tests be built? BUILD_TESTING=ON Default: OFF
# Should we build the documentation? BUILD_DOCS=ON Default: OFF
# Include Eigen compatibility headers? ENABLE_EIGEN_SUPPORT=ON Default: ON
cmake -Bbuild -H. \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_INSTALL_PREFIX=/path/to/install \
    -DBUILD_TESTING=ON \
    -DBUILD_DOCS=ON \
    -DENABLE_EIGEN_SUPPORT=ON

# -- Build Documentation --
cmake --build build --target sigma_cxx_api
# Documentation will be in ./build/html

# -- Build and Run Tests --
cmake --build build --parallel
cd build
ctest -VV

# -- Install Library --
cmake --build build --target install
```

## Acknowledgments
TODO: Add acknowledgments
