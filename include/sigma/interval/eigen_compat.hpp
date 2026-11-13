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
template<typename T>
class GeneralInterval;
} // namespace sigma

/** @def EIGEN_NUMTRAITS(interval_type, float_type)
 *  @brief Factorization for Eigen::NumTraits Specialization for Interval and
 *         GeneralInterval
 */
#define EIGEN_NUMTRAITS(interval_type, float_type)             \
    /** @brief Numeric traits for interval_type<float_type> */ \
    template<>                                                 \
    struct NumTraits<sigma::interval_type<float_type>>         \
      : NumTraits<float_type> {                                \
        /** The interval type */                               \
        using Interval = sigma::interval_type<float_type>;     \
        /** The corresponding real type */                     \
        using Real = Interval;                                 \
        /** The corresponding non-integer type */              \
        using NonInteger = Interval;                           \
        /** The corresponding literal type */                  \
        using Literal = Interval;                              \
        /** The corresponding nested type */                   \
        using Nested = Interval;                               \
        enum {                                                 \
            IsComplex             = 0,                         \
            IsInteger             = 0,                         \
            IsSigned              = 1,                         \
            RequireInitialization = 1,                         \
            ReadCost              = 1,                         \
            AddCost               = 3,                         \
            MulCost               = 3                          \
        };                                                     \
    }

/** @namespace Eigen
 *  @brief The namespace of the Eigen library
 *
 *  Used here to overload the numeric traits struct for Interval and
 *  GeneralInterval values
 */
namespace Eigen {

EIGEN_NUMTRAITS(Interval, float);
EIGEN_NUMTRAITS(Interval, double);
EIGEN_NUMTRAITS(GeneralInterval, float);
EIGEN_NUMTRAITS(GeneralInterval, double);

} // namespace Eigen

#undef EIGEN_NUMTRAITS
#endif // ENABLE_EIGEN_SUPPORT
