#pragma once

#ifdef ENABLE_EIGEN_SUPPORT
#include <Eigen/Dense>

// Foward Declaration
namespace sigma {
template<typename T>
class Uncertain;
} // namespace sigma

// Factorization for Eigen::NumTraits Specialization
#define EIGEN_NUMTRAITS(float_type)                                          \
    template<>                                                               \
    struct NumTraits<sigma::Uncertain<float_type>> : NumTraits<float_type> { \
        typedef sigma::Uncertain<float_type> Real;                           \
        typedef sigma::Uncertain<float_type> NonInteger;                     \
        typedef sigma::Uncertain<float_type> Literal;                        \
        typedef sigma::Uncertain<float_type> Nested;                         \
        enum {                                                               \
            IsComplex             = 0,                                       \
            IsInteger             = 0,                                       \
            IsSigned              = 1,                                       \
            RequireInitialization = 1,                                       \
            ReadCost              = 1,                                       \
            AddCost               = 3,                                       \
            MulCost               = 3                                        \
        };                                                                   \
    }

namespace Eigen {

EIGEN_NUMTRAITS(float);
EIGEN_NUMTRAITS(double);

} // namespace Eigen

#undef EIGEN_NUMTRAITS
#endif // ENABLE_EIGEN_SUPPORT
