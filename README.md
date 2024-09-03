# Sigma 

Sigma is a header-only C++ library for uncertainty propagation throughout
mathematical operations on floating point values. Inspired by
[uncertainties](https://github.com/lmfit/uncertainties) for Python and
[Measurements.jl](https://github.com/JuliaPhysics/Measurements.jl) for Julia.

## Motivation
TODO: Add motivation

See [Background](./docs/background.md) for more details.

## Features
- Support for common mathematical operations.
- Limited compatible with [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
  for linear algebra support.

See [here](./docs/examples.md) for examples of these features.

## Installation
Sigma is header-only, so installation is not strictly required for usage. The
files simply need to be locatable by your project, as well the dependencies
required by Sigma itself (e.g. Eigen if support is configured). Dependency
gathering, testing, building of the documentation, and installation are all
handled with CMake and the [CMaize](https://github.com/CMakePP/CMaize) Framework.

```Bash
# -- Configuration Step --
# BUILD_TESTING=ON - Building the tests.
# BUILD_DOCS=ON - Building the documentation. Requires Doxygen.
# ENABLE_EIGEN_SUPPORT=ON - Eigen support included. Will build and install Eigen
#                           alongside Sigma if not found.
cmake -Bbuild -H. \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_INSTALL_PREFIX=/path/to/install \
    -DBUILD_TESTING=ON \
    -DBUILD_DOCS=ON \
    -DENABLE_EIGEN_SUPPORT=ON

# -- Build Documentation
cmake --build build --target sigma_cxx_api
# Documentation will be in ./build/html

# -- Build and Run Tests
cmake --build build --parallel
cd build
ctest -VV

# -- Install Library
cmake --build build --target install
```

## Contributing
- [Code of Conduct](./docs/contributing.md#code-of-conduct)
- [Contributor Guidelines](./docs/contributing.md#contributor-guidelines)
- [Contributor License Agreement](./docs/contributing.md#contributor-license-agreement)
- [Developer Documentation](./docs/contributing.md#developer-documentation)

## Acknowledgments
TODO: Add acknowledgments
