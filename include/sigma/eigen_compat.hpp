#pragma once

/** @file eigen_compat.hpp
 *  @brief Components for compatibility with Eigen
 */

#ifdef ENABLE_EIGEN_SUPPORT
#include <Eigen/Dense>

// Foward Declarations
namespace sigma {
template<typename T>
class Uncertain;
template<typename T>
class Interval;
template<typename T>
class Affine;
template<typename T>
class PartitionedAffine;
} // namespace sigma

/** @def EIGEN_NUMTRAITS(float_type)
 *  @brief Factorization for Eigen::NumTraits Specialization for Uncertain
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

/** @def EIGEN_NUMTRAITS_INTERVAL(float_type)
 *  @brief Factorization for Eigen::NumTraits Specialization for Interval
 */
#define EIGEN_NUMTRAITS_INTERVAL(float_type)                                \
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

/** @def EIGEN_NUMTRAITS_AFFINE(float_type)
 *  @brief Factorization for Eigen::NumTraits Specialization for Affine
 */
#define EIGEN_NUMTRAITS_AFFINE(float_type)                                \
    /** @brief Numeric traits for Interval<float_type> */                 \
    template<>                                                            \
    struct NumTraits<sigma::Affine<float_type>> : NumTraits<float_type> { \
        /** The affine type */                                            \
        using Affine = sigma::Affine<float_type>;                         \
        /** The corresponding real type */                                \
        using Real = Affine;                                              \
        /** The corresponding non-integer type */                         \
        using NonInteger = Affine;                                        \
        /** The corresponding literal type */                             \
        using Literal = Affine;                                           \
        /** The corresponding nested type */                              \
        using Nested = Affine;                                            \
        enum {                                                            \
            IsComplex             = 0,                                    \
            IsInteger             = 0,                                    \
            IsSigned              = 1,                                    \
            RequireInitialization = 1,                                    \
            ReadCost              = 1,                                    \
            AddCost               = 3,                                    \
            MulCost               = 3                                     \
        };                                                                \
    }

/** @def EIGEN_NUMTRAITS_AFFINE(float_type)
 *  @brief Factorization for Eigen::NumTraits Specialization for Affine
 */
#define EIGEN_NUMTRAITS_PARTITIONED_AFFINE(float_type)       \
    /** @brief Numeric traits for Interval<float_type> */    \
    template<>                                               \
    struct NumTraits<sigma::PartitionedAffine<float_type>>   \
      : NumTraits<float_type> {                              \
        /** The affine type */                               \
        using Affine = sigma::PartitionedAffine<float_type>; \
        /** The corresponding real type */                   \
        using Real = Affine;                                 \
        /** The corresponding non-integer type */            \
        using NonInteger = Affine;                           \
        /** The corresponding literal type */                \
        using Literal = Affine;                              \
        /** The corresponding nested type */                 \
        using Nested = Affine;                               \
        enum {                                               \
            IsComplex             = 0,                       \
            IsInteger             = 0,                       \
            IsSigned              = 1,                       \
            RequireInitialization = 1,                       \
            ReadCost              = 1,                       \
            AddCost               = 3,                       \
            MulCost               = 3                        \
        };                                                   \
    }

/** @namespace Eigen
 *  @brief The namespace of the Eigen library
 *
 *  Used here to overload the numeric traits struct for Uncertain and Interval
 *  values
 */
namespace Eigen {

EIGEN_NUMTRAITS(float);
EIGEN_NUMTRAITS(double);

EIGEN_NUMTRAITS_INTERVAL(float);
EIGEN_NUMTRAITS_INTERVAL(double);

EIGEN_NUMTRAITS_AFFINE(float);
EIGEN_NUMTRAITS_AFFINE(double);

EIGEN_NUMTRAITS_PARTITIONED_AFFINE(float);
EIGEN_NUMTRAITS_PARTITIONED_AFFINE(double);

} // namespace Eigen

#undef EIGEN_NUMTRAITS
#undef EIGEN_NUMTRAITS_INTERVAL
#undef EIGEN_NUMTRAITS_AFFINE
#endif // ENABLE_EIGEN_SUPPORT
