#ifdef ENABLE_EIGEN_SUPPORT

#include "testing.hpp"
#include <Eigen/Dense>
#include <sigma/sigma.hpp>

using testing::test_uncertain;

TEMPLATE_TEST_CASE("Eigen Matrix with Uncertain Elements", "", sigma::UFloat,
                   sigma::UDouble) {
    using testing_t = TestType;
    using value_t   = typename testing_t::value_t;
    using umatrix_t = Eigen::Matrix<testing_t, Eigen::Dynamic, Eigen::Dynamic>;

    auto u = [](value_t mean) -> testing_t {
        return testing_t{mean, mean * (value_t)0.1};
    };

    SECTION("Arithmetic Operations") {
        testing_t a = u(1.0);
        testing_t b = u(2.0);
        testing_t c = u(3.0);
        testing_t d = u(4.0);

        umatrix_t mat1(2, 2), mat2(2, 2);
        mat1 << a, b, c, d;
        mat2 << d, c, b, a;

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

    SECTION("Linear Algebra") {
        umatrix_t A(3, 3);
        umatrix_t b(3, 1);
        A << u(1), u(2), u(3), u(4), u(5), u(6), u(7), u(8), u(10);
        b << u(3), u(3), u(4);

        umatrix_t x;
        auto check_solution = [&x]() {
            test_uncertain(x(0, 0), -2.0, 2.5016, 12);
            test_uncertain(x(1, 0), 1.0, 5.7594, 12);
            test_uncertain(x(2, 0), 1.0, 3.0627, 12);
        };

        SECTION("Partial LU Decomposition") {
            x = A.partialPivLu().solve(b);
            check_solution();
        }

        SECTION("Full LU Decomposition") {
            x = A.fullPivLu().solve(b);
            check_solution();
        }

        SECTION("Householder QR Decomposition") {
            x = A.householderQr().solve(b);
            check_solution();
        }

        SECTION("Column-Pivoting Householder QR Decomposition") {
            x = A.colPivHouseholderQr().solve(b);
            check_solution();
        }

        SECTION("Full-Pivoting Householder QR Decomposition") {
            x = A.fullPivHouseholderQr().solve(b);
            check_solution();
        }
    }
}

#endif // ENABLE_EIGEN_SUPPORT
