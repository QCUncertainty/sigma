#pragma once

/** @file eigen_compat.hpp
 *  @brief Components for compatibility with Eigen
 */

#ifdef ENABLE_EIGEN_SUPPORT
#include <Eigen/Dense>

// Foward Declarations
namespace sigma {
template<typename T>
class Taylor;

template<typename T>
class TaylorModel;

} // namespace sigma

/** @def EIGEN_NUMTRAITS(float_type)
 *  @brief Factorization for Eigen::NumTraits Specialization for Taylor
 */
#define EIGEN_NUMTRAITS(float_type)                                            \
    /** @brief Numeric traits for Taylor<float_type> */                        \
    template<>                                                                 \
    struct NumTraits<sigma::Taylor<float_type>> : NumTraits<float_type> {      \
        /** The Taylor type */                                                 \
        using Taylor = sigma::Taylor<float_type>;                              \
        /** The corresponding real type */                                     \
        using Real = Taylor;                                                   \
        /** The corresponding non-integer type */                              \
        using NonInteger = Taylor;                                             \
        /** The corresponding literal type */                                  \
        using Literal = Taylor;                                                \
        /** The corresponding nested type */                                   \
        using Nested = Taylor;                                                 \
        enum {                                                                 \
            IsComplex             = 0,                                         \
            IsInteger             = 0,                                         \
            IsSigned              = 1,                                         \
            RequireInitialization = 1,                                         \
            ReadCost              = 1,                                         \
            AddCost               = 3,                                         \
            MulCost               = 3                                          \
        };                                                                     \
    };                                                                         \
    /** @brief Numeric traits for TaylorModel<float_type> */                   \
    template<>                                                                 \
    struct NumTraits<sigma::TaylorModel<float_type>> : NumTraits<float_type> { \
        /** The TaylorModel type */                                            \
        using Taylor = sigma::TaylorModel<float_type>;                         \
        /** The corresponding real type */                                     \
        using Real = Taylor;                                                   \
        /** The corresponding non-integer type */                              \
        using NonInteger = Taylor;                                             \
        /** The corresponding literal type */                                  \
        using Literal = Taylor;                                                \
        /** The corresponding nested type */                                   \
        using Nested = Taylor;                                                 \
        enum {                                                                 \
            IsComplex             = 0,                                         \
            IsInteger             = 0,                                         \
            IsSigned              = 1,                                         \
            RequireInitialization = 1,                                         \
            ReadCost              = 1,                                         \
            AddCost               = 3,                                         \
            MulCost               = 3                                          \
        };                                                                     \
    }

/** @namespace Eigen
 *  @brief The namespace of the Eigen library
 *
 *  Used here to overload the numeric traits struct for Taylor and
 *  TaylorModel values
 */
namespace Eigen {

EIGEN_NUMTRAITS(float);
EIGEN_NUMTRAITS(double);

} // namespace Eigen

#undef EIGEN_NUMTRAITS
#endif // ENABLE_EIGEN_SUPPORT
