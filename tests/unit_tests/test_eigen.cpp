#ifdef ENABLE_EIGEN_SUPPORT

#include "testing.hpp"
#include <Eigen/Dense>
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Eigen Matrix with Uncertain Elements", "", sigma::UFloat,
                   sigma::UDouble) {
    using testing_t = TestType;
    using umatrix_t = Eigen::Matrix<testing_t, Eigen::Dynamic, Eigen::Dynamic>;

    testing_t a{1.0, 0.1};
    testing_t b{2.0, 0.2};
    testing_t c{3.0, 0.3};
    testing_t d{4.0, 0.4};

    umatrix_t mat1(2, 2), mat2(2, 2);
    mat1(0, 0) = a;
    mat1(0, 1) = b;
    mat1(1, 0) = c;
    mat1(1, 1) = d;

    mat2(0, 0) = d;
    mat2(0, 1) = c;
    mat2(1, 0) = b;
    mat2(1, 1) = a;

    SECTION("Matrix Addition") {
        auto mat3 = mat1 + mat2;
        test_uncertain(mat3(0, 0), 5.0, 0.4123, 2);
        test_uncertain(mat3(0, 1), 5.0, 0.3606, 2);
        test_uncertain(mat3(1, 0), 5.0, 0.3606, 2);
        test_uncertain(mat3(1, 1), 5.0, 0.4123, 2);
    }

    SECTION("Matrix Subtraction") {
        auto mat3 = mat1 - mat2;
        test_uncertain(mat3(0, 0), -3.0, 0.4123, 2);
        test_uncertain(mat3(0, 1), -1.0, 0.3606, 2);
        test_uncertain(mat3(1, 0), 1.0, 0.3606, 2);
        test_uncertain(mat3(1, 1), 3.0, 0.4123, 2);
    }

    SECTION("Matrix Multiplication") {
        auto mat3 = mat1 * mat2;
        test_uncertain(mat3(0, 0), 8.0, 0.9798, 3);
        test_uncertain(mat3(0, 1), 5.0, 0.6164, 3);
        test_uncertain(mat3(1, 0), 20.0, 2.4658, 3);
        test_uncertain(mat3(1, 1), 13.0, 1.8868, 3);
    }
}

#endif // ENABLE_EIGEN_SUPPORT
