#ifdef ENABLE_EIGEN_SUPPORT
#include "testing.hpp"
#include <Eigen/Dense>
#include <sigma/sigma.hpp>

using testing::test_interval;

TEMPLATE_TEST_CASE("Eigen Matrix with Interval Elements", "", sigma::IFloat,
                   sigma::IDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;
    using imatrix_t = Eigen::Matrix<testing_t, Eigen::Dynamic, Eigen::Dynamic>;

    testing_t a(value_t{1}, value_t{3});
    testing_t b(value_t{2}, value_t{4});
    testing_t c(value_t{3}, value_t{5});
    testing_t d(value_t{4}, value_t{6});

    imatrix_t mat1(2, 2), mat2(2, 2);
    mat1 << a, b, c, d;
    mat2 << d, c, b, a;

    SECTION("Arithmetic Operations") {
        SECTION("Matrix Addition") {
            auto mat3 = mat1 + mat2;
            // [1,3]+[4,6]=[5,9], [2,4]+[3,5]=[5,9]
            // [3,5]+[2,4]=[5,9], [4,6]+[1,3]=[5,9]
            test_interval(mat3(0, 0), 5.0, 9.0);
            test_interval(mat3(0, 1), 5.0, 9.0);
            test_interval(mat3(1, 0), 5.0, 9.0);
            test_interval(mat3(1, 1), 5.0, 9.0);
        }
        SECTION("Matrix Subtraction") {
            auto mat3 = mat1 - mat2;
            // [1,3]-[4,6]=[-5,-1], [2,4]-[3,5]=[-3,1]
            // [3,5]-[2,4]=[-1,3],  [4,6]-[1,3]=[1,5]
            test_interval(mat3(0, 0), -5.0, -1.0);
            test_interval(mat3(0, 1), -3.0, 1.0);
            test_interval(mat3(1, 0), -1.0, 3.0);
            test_interval(mat3(1, 1), 1.0, 5.0);
        }
        SECTION("Matrix Multiplication") {
            auto mat3 = mat1 * mat2;
            // (0,0): a*d + b*b = [1,3]*[4,6]+[2,4]*[2,4] = [4,18]+[4,16] =
            // [8,34] (0,1): a*c + b*a = [1,3]*[3,5]+[2,4]*[1,3] = [3,15]+[2,12]
            // = [5,27] (1,0): c*d + d*b = [3,5]*[4,6]+[4,6]*[2,4] =
            // [12,30]+[8,24] = [20,54] (1,1): c*c + d*a =
            // [3,5]*[3,5]+[4,6]*[1,3] = [9,25]+[4,18] = [13,43]
            test_interval(mat3(0, 0), 8.0, 34.0);
            test_interval(mat3(0, 1), 5.0, 27.0);
            test_interval(mat3(1, 0), 20.0, 54.0);
            test_interval(mat3(1, 1), 13.0, 43.0);
        }
    }
}

#endif // ENABLE_EIGEN_SUPPORT
