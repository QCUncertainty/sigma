#pragma once

/** @file eigen_compat.hpp
 *  @brief Components for compatibility with Eigen
 */

#ifdef ENABLE_EIGEN_SUPPORT
#include <Eigen/Dense>

// Foward Declaration
namespace sigma {
template<typename T>
class Uncertain;
} // namespace sigma

/** @def EIGEN_NUMTRAITS(float_type)
 *  @brief Factorization for Eigen::NumTraits Specialization
 */
#define EIGEN_NUMTRAITS(float_type)                                          \
    /** @brief Numeric traits for Uncertain<float_type> */                   \
    template<>                                                               \
    struct NumTraits<sigma::Uncertain<float_type>> : NumTraits<float_type> { \
        /** The uncertain type */                                            \
        using Uncertain = sigma::Uncertain<float_type>;                      \
        /** The corresponding real type */                                   \
        using Real = Uncertain;                                              \
        /** The corresponding non-integer type */                            \
        using NonInteger = Uncertain;                                        \
        /** The corresponding literal type */                                \
        using Literal = Uncertain;                                           \
        /** The corresponding nested type */                                 \
        using Nested = Uncertain;                                            \
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

/** @namespace Eigen
 *  @brief The namespace of the Eigen library
 *
 *  Used here to overload the numeric traits struct for Uncertain values
 */
namespace Eigen {

EIGEN_NUMTRAITS(float);
EIGEN_NUMTRAITS(double);

} // namespace Eigen

#undef EIGEN_NUMTRAITS
#endif // ENABLE_EIGEN_SUPPORT
