# Sigma 

[Documentation](https://qcuncertainty.github.io/sigma/)

Sigma is a header-only C++ library for uncertainty propagation throughout
mathematical operations on floating point values, inspired by
[uncertainties](https://github.com/lmfit/uncertainties) for Python and
[Measurements.jl](https://github.com/JuliaPhysics/Measurements.jl) for Julia.

## Statement of Need
Scientific analysis, whether based on experiment or simulation, often uses
values that are paired with some representation of their accuracy or 
reliability. Oftentimes, this representation is a value's standard deviation or
a similar metric. The uncertainty in a value can be derived from a number of 
sources, including instrument accuracy, limitations from approximations, or 
the statistical nature of the value itself. New values resulting from operations
on uncertain inputs will have uncertainties derived from the input 
uncertainties, but the process of propagating those relationships can become
tedious and complicated. To the best of our knowledge, there is no currently 
maintained C++ library to facilitate this kind of uncertainty propagation. As 
C++ is an important language in the development of scientific software and 
high-performance computing, Sigma has been developed in an attempt to fill this
gap.

See [Background](./docs/background.md) for methodological details.

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

## Contributing

- [Contributor Guidelines](./docs/contributing.md)
- [Code of Conduct](./docs/code_of_conduct.md)

## Acknowledgments

This work was supported by the Ames National Laboratory's Laboratory Directed 
Research and Development (LDRD) program. The Ames Laboratory is operated for the
U.S. DOE by Iowa State University under contract # DE-AC02-07CH11358.
