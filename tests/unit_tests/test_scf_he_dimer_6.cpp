#ifdef ENABLE_EIGEN_SUPPORT

#include "testing.hpp"
#include <Eigen/Dense>
#include <iomanip>
#include <sigma/sigma.hpp>
#include <unsupported/Eigen/CXX11/Tensor>

/* Model SCF for He dimer
 * Geom
 * He 0.0 0.0 0.0
 * He 0.0 0.0 5.0
 *
 */

using uncertain_t = sigma::UDouble;
using value_t     = typename uncertain_t::value_t;
using scalar_t    = Eigen::Tensor<uncertain_t, 0>;
using vector_t    = Eigen::Tensor<uncertain_t, 1>;
using matrix_t    = Eigen::Tensor<uncertain_t, 2>;
using tensor3_t   = Eigen::Tensor<uncertain_t, 3>;
using tensor4_t   = Eigen::Tensor<uncertain_t, 4>;
using index_t     = Eigen::IndexPair<int>;

constexpr auto rmajor = Eigen::RowMajor;
constexpr auto edynam = Eigen::Dynamic;
using eig_mat_t       = Eigen::Matrix<uncertain_t, edynam, edynam, rmajor>;
using map_t           = Eigen::Map<const eig_mat_t>;

static double error_v = 1e-6;

// These are all pre-generated integral values from Libint2
namespace he_dimer_6 {
matrix_t get_s() {
    matrix_t s(2, 2);
    s.setConstant(uncertain_t(0.0, error_v));
    s(0, 0) = s(0, 0) + 1.0000000000000000;
    s(0, 1) = s(0, 1) + 0.0046786143364014;
    s(1, 0) = s(1, 0) + 0.0046786143364014;
    s(1, 1) = s(1, 1) + 1.0000000000000000;
    return s;
}

matrix_t get_t() {
    matrix_t t(2, 2);
    t.setConstant(uncertain_t(0.0, error_v));
    t(0, 0) = t(0, 0) + 1.4117631711227245;
    t(0, 1) = t(0, 1) + -0.0047543932390151;
    t(1, 0) = t(1, 0) + -0.0047543932390151;
    t(1, 1) = t(1, 1) + 1.4117631711227245;
    return t;
}

matrix_t get_v() {
    matrix_t v(2, 2);
    v.setConstant(uncertain_t(0.0, error_v));
    v(0, 0) = v(0, 0) + -3.7435116177497019;
    v(0, 1) = v(0, 1) + -0.0080255940237100;
    v(1, 0) = v(1, 0) + -0.0080255940237100;
    v(1, 1) = v(1, 1) + -3.7435116177497019;
    return v;
}

tensor4_t get_eri4() {
    tensor4_t eri4(2, 2, 2, 2);
    eri4.setConstant(uncertain_t(0.0, error_v));
    eri4(0, 0, 0, 0) = eri4(0, 0, 0, 0) + 1.0557129400212610;
    eri4(0, 0, 0, 1) = eri4(0, 0, 0, 1) + 0.0019213771068706;
    eri4(0, 0, 1, 0) = eri4(0, 0, 1, 0) + 0.0019213771068706;
    eri4(0, 0, 1, 1) = eri4(0, 0, 1, 1) + 0.1999993218713060;
    eri4(0, 1, 0, 0) = eri4(0, 1, 0, 0) + 0.0019213771068706;
    eri4(0, 1, 0, 1) = eri4(0, 1, 0, 1) + 0.0000000000000000;
    eri4(0, 1, 1, 0) = eri4(0, 1, 1, 0) + 0.0000000000000000;
    eri4(0, 1, 1, 1) = eri4(0, 1, 1, 1) + 0.0019213771068706;
    eri4(1, 0, 0, 0) = eri4(1, 0, 0, 0) + 0.0019213771068706;
    eri4(1, 0, 0, 1) = eri4(1, 0, 0, 1) + 0.0000000000000000;
    eri4(1, 0, 1, 0) = eri4(1, 0, 1, 0) + 0.0000000000000000;
    eri4(1, 0, 1, 1) = eri4(1, 0, 1, 1) + 0.0019213771068706;
    eri4(1, 1, 0, 0) = eri4(1, 1, 0, 0) + 0.1999993218713060;
    eri4(1, 1, 0, 1) = eri4(1, 1, 0, 1) + 0.0019213771068706;
    eri4(1, 1, 1, 0) = eri4(1, 1, 1, 0) + 0.0019213771068706;
    eri4(1, 1, 1, 1) = eri4(1, 1, 1, 1) + 1.0557129400212610;
    return eri4;
}
} // namespace he_dimer_6

TEST_CASE("Model SCF - He Dimer - 10e-6") {
    // Set Integral Values
    auto s          = he_dimer_6::get_s();
    auto t          = he_dimer_6::get_t();
    auto v          = he_dimer_6::get_v();
    auto eri4       = he_dimer_6::get_eri4();
    matrix_t H_core = t + v;

    // Nuclear repulsion energy
    value_t e_nuc = 0.8;

    // Containers for the density
    matrix_t p(2, 2);

    // SCF Parameters
    int max_iter = 4;

    // Form Initial Guess
    map_t s_map(s.data(), 2, 2);
    map_t h_core_map(H_core.data(), 2, 2);
    Eigen::GeneralizedSelfAdjointEigenSolver<eig_mat_t> ges(h_core_map, s_map);
    eig_mat_t density = ges.eigenvectors() * ges.eigenvectors().transpose();

    p(0, 0) = density(0, 0);
    p(0, 1) = density(0, 1);
    p(1, 0) = density(1, 0);
    p(1, 1) = density(1, 1);

    std::cout << "He Dimer - 10e-6" << std::endl;
    SECTION("Maintain error sources") {
        std::cout << "Maintain error sources" << std::endl;
        for(auto i = 0; i < max_iter; ++i) {
            std::cout << std::setprecision(16);
            std::cout << "Iteration " << i << std::endl;

            Eigen::array<index_t, 2> j_modes{index_t(2, 0), index_t(3, 1)};
            Eigen::array<index_t, 2> k_modes{index_t(1, 0), index_t(2, 1)};

            // reuse integrals
            matrix_t J = eri4.contract(p, j_modes);
            matrix_t K = eri4.contract(p, k_modes);
            matrix_t F = H_core + 2 * J - K;

            // Compute Energy
            matrix_t H = H_core + F;
            Eigen::array<index_t, 2> e_modes{index_t(0, 0), index_t(1, 1)};
            scalar_t e         = H.contract(p, e_modes);
            uncertain_t e_elec = e();
            std::cout << "Total Energy   = " << e_elec + e_nuc << std::endl;
            std::cout << "# Error Sources= " << e_elec.deps().size()
                      << std::endl;
            // for(const auto& [dep, deriv] : e_elec.deps()) {
            //     std::cout << dep.get() << " = " << deriv << std::endl;
            // }

            map_t F_map(F.data(), 2, 2);
            Eigen::GeneralizedSelfAdjointEigenSolver<eig_mat_t> es(F_map,
                                                                   s_map);
            eig_mat_t d = es.eigenvectors() * es.eigenvectors().transpose();

            p(0, 0) = d(0, 0);
            p(0, 1) = d(0, 1);
            p(1, 0) = d(1, 0);
            p(1, 1) = d(1, 1);
        }
    }

    SECTION("New error sources") {
        std::cout << "New error sources" << std::endl;
        for(auto i = 0; i < max_iter; ++i) {
            std::cout << std::setprecision(16);
            std::cout << "Iteration " << i << std::endl;

            Eigen::array<index_t, 2> j_modes{index_t(2, 0), index_t(3, 1)};
            Eigen::array<index_t, 2> k_modes{index_t(1, 0), index_t(2, 1)};

            // get new instances
            matrix_t J = he_dimer_6::get_eri4().contract(p, j_modes);
            matrix_t K = he_dimer_6::get_eri4().contract(p, k_modes);
            matrix_t F = he_dimer_6::get_t() + he_dimer_6::get_v() + 2 * J - K;

            // Compute Energy
            matrix_t H = he_dimer_6::get_t() + he_dimer_6::get_v() + F;
            Eigen::array<index_t, 2> e_modes{index_t(0, 0), index_t(1, 1)};
            scalar_t e         = H.contract(p, e_modes);
            uncertain_t e_elec = e();
            std::cout << "Total Energy   = " << e_elec + e_nuc << std::endl;
            std::cout << "# Error Sources= " << e_elec.deps().size()
                      << std::endl;
            // for(const auto& [dep, deriv] : e_elec.deps()) {
            //     std::cout << dep.get() << " = " << deriv << std::endl;
            // }

            map_t F_map(F.data(), 2, 2);
            auto s_again = he_dimer_6::get_s();
            map_t s_again_map(s_again.data(), 2, 2);
            Eigen::GeneralizedSelfAdjointEigenSolver<eig_mat_t> es(F_map,
                                                                   s_again_map);
            eig_mat_t d = es.eigenvectors() * es.eigenvectors().transpose();

            p(0, 0) = d(0, 0);
            p(0, 1) = d(0, 1);
            p(1, 0) = d(1, 0);
            p(1, 1) = d(1, 1);
        }
    }
}

#endif // ENABLE_EIGEN_SUPPORT
