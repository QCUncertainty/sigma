#pragma once

/** @file eigen_compat.hpp
 *  @brief Components for compatibility with Eigen
 */

#ifdef ENABLE_EIGEN_SUPPORT
#include <Eigen/Dense>

// Foward Declarations
namespace sigma {
template<typename T>
class Interval;
} // namespace sigma

/** @def EIGEN_NUMTRAITS(float_type)
 *  @brief Factorization for Eigen::NumTraits Specialization for Interval
 */
#define EIGEN_NUMTRAITS(float_type)                                         \
    /** @brief Numeric traits for Interval<float_type> */                   \
    template<>                                                              \
    struct NumTraits<sigma::Interval<float_type>> : NumTraits<float_type> { \
        /** The interval type */                                            \
        using Interval = sigma::Interval<float_type>;                       \
        /** The corresponding real type */                                  \
        using Real = Interval;                                              \
        /** The corresponding non-integer type */                           \
        using NonInteger = Interval;                                        \
        /** The corresponding literal type */                               \
        using Literal = Interval;                                           \
        /** The corresponding nested type */                                \
        using Nested = Interval;                                            \
        enum {                                                              \
            IsComplex             = 0,                                      \
            IsInteger             = 0,                                      \
            IsSigned              = 1,                                      \
            RequireInitialization = 1,                                      \
            ReadCost              = 1,                                      \
            AddCost               = 3,                                      \
            MulCost               = 3                                       \
        };                                                                  \
    }

/** @namespace Eigen
 *  @brief The namespace of the Eigen library
 *
 *  Used here to overload the numeric traits struct for Interval values
 */
namespace Eigen {

EIGEN_NUMTRAITS(float);
EIGEN_NUMTRAITS(double);

} // namespace Eigen

#undef EIGEN_NUMTRAITS
#endif // ENABLE_EIGEN_SUPPORT
