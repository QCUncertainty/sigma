#ifdef ENABLE_EIGEN_SUPPORT
#include "testing.hpp"
#include <Eigen/Dense>
#include <sigma/sigma.hpp>

using testing::test_triplex;

TEMPLATE_TEST_CASE("Eigen Matrix with Triplex Elements", "", TRIPLEX_TYPES) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;
    using tmatrix_t = Eigen::Matrix<testing_t, Eigen::Dynamic, Eigen::Dynamic>;

    testing_t a(value_t{1}, value_t{2}, value_t{3});
    testing_t b(value_t{2}, value_t{2}, value_t{4});
    testing_t c(value_t{3}, value_t{5}, value_t{5});
    testing_t d(value_t{4}, value_t{5}, value_t{6});

    tmatrix_t mat1(2, 2), mat2(2, 2);
    mat1 << a, b, c, d;
    mat2 << d, c, b, a;

    SECTION("Matrix Addition") {
        tmatrix_t mat3 = mat1 + mat2;
        // a+d = <5, 7, 9>, b+c = <5, 7, 9>
        test_triplex(mat3(0, 0), 5.0, 7.0, 9.0);
        test_triplex(mat3(0, 1), 5.0, 7.0, 9.0);
        test_triplex(mat3(1, 0), 5.0, 7.0, 9.0);
        test_triplex(mat3(1, 1), 5.0, 7.0, 9.0);
    }
    SECTION("Matrix Subtraction") {
        tmatrix_t mat3 = mat1 - mat2;
        // a-d = <-5, -3, -1>, b-c = <-3, -3, 1>
        // c-b = <-1, 3, 3>,   d-a = <1, 3, 5>
        test_triplex(mat3(0, 0), -5.0, -3.0, -1.0);
        test_triplex(mat3(0, 1), -3.0, -3.0, 1.0);
        test_triplex(mat3(1, 0), -1.0, 3.0, 3.0);
        test_triplex(mat3(1, 1), 1.0, 3.0, 5.0);
    }
    SECTION("Matrix Multiplication") {
        tmatrix_t mat3 = mat1 * mat2;
        // The bounds are those of the interval test; the nominal values are the
        // product of the matrices of nominal values, [[2, 2], [5, 5]] * [[5,
        // 5], [2, 2]] = [[14, 14], [35, 35]].
        test_triplex(mat3(0, 0), 8.0, 14.0, 34.0);
        test_triplex(mat3(0, 1), 5.0, 14.0, 27.0);
        test_triplex(mat3(1, 0), 20.0, 35.0, 54.0);
        test_triplex(mat3(1, 1), 13.0, 35.0, 43.0);
    }
}

#endif // ENABLE_EIGEN_SUPPORT
