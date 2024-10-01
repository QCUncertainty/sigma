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
        SECTION("LU Decomposition") {
            umatrix_t A(3, 3);
            umatrix_t b(3, 1);
            umatrix_t x;
            A << u(1), u(2), u(3), u(4), u(5), u(6), u(7), u(8), u(10);
            b << u(3), u(3), u(4);
            auto check_solution = [&x]() {
                test_uncertain(x(0, 0), -2.0, 2.5016, 12);
                test_uncertain(x(1, 0), 1.0, 5.7594, 12);
                test_uncertain(x(2, 0), 1.0, 3.0627, 12);
            };
            SECTION("Partial") {
                x = A.partialPivLu().solve(b);
                check_solution();
            }
            SECTION("Full") {
                x = A.fullPivLu().solve(b);
                check_solution();
            }
        }
        SECTION("Householder Decompistion") {
            umatrix_t A(3, 3);
            umatrix_t b(3, 1);
            umatrix_t x;
            A << u(1), u(2), u(3), u(4), u(5), u(6), u(7), u(8), u(10);
            b << u(3), u(3), u(4);
            auto check_solution = [&x]() {
                test_uncertain(x(0, 0), -2.0, 2.5016, 12);
                test_uncertain(x(1, 0), 1.0, 5.7594, 12);
                test_uncertain(x(2, 0), 1.0, 3.0627, 12);
            };
            SECTION("QR") {
                x = A.householderQr().solve(b);
                check_solution();
            }
            SECTION("Column-Pivoting QR") {
                x = A.colPivHouseholderQr().solve(b);
                check_solution();
            }
            SECTION("Full-Pivoting QR") {
                x = A.fullPivHouseholderQr().solve(b);
                check_solution();
            }
        }
        SECTION("Cholesky Decomposition") {
            umatrix_t A(2, 2);
            umatrix_t b(2, 2);
            umatrix_t x;
            A << u(2), u(-1), u(-1), u(3);
            b << u(1), u(2), u(3), u(1);
            auto check_solution = [&x]() {
                test_uncertain(x(0, 0), 1.2, 0.2160, 5);
                test_uncertain(x(0, 1), 1.4, 0.2261, 5);
                test_uncertain(x(1, 0), 1.4, 0.2261, 5);
                test_uncertain(x(1, 1), 0.8, 0.1440, 5);
            };
            SECTION("LLT") {
                x = A.llt().solve(b);
                check_solution();
            }
            SECTION("LDLT") {
                x = A.ldlt().solve(b);
                check_solution();
            }
        }
        SECTION("Eigendecomposition") {
            umatrix_t A(2, 2);
            A(0, 0) = u(1);
            A(0, 1) = u(2);
            A(1, 0) = A(0, 1); // Take care with symmetry
            A(1, 1) = u(3);
            Eigen::SelfAdjointEigenSolver<umatrix_t> solver(A);
            umatrix_t evalues  = solver.eigenvalues();
            umatrix_t evectors = solver.eigenvectors();
            test_uncertain(evalues(0, 0), -0.2361, 0.2100, 3);
            test_uncertain(evalues(1, 0), 4.2361, 0.2826, 3);
            test_uncertain(evectors(0, 0), -0.8507, 0.0197, 3);
            test_uncertain(evectors(0, 1), -0.5257, 0.0318, 3);
            test_uncertain(evectors(1, 0), 0.5257, 0.0318, 3);
            test_uncertain(evectors(1, 1), -0.8507, 0.0197, 3);
        }
    }
}

#endif // ENABLE_EIGEN_SUPPORT
