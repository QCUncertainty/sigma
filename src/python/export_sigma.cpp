#include "export_sigma.hpp"

PYBIND11_MODULE(sigma, m) {
    m.doc() = "Python bindings for the sigma uncertainty library";
    sigma::export_interval(m);
    sigma::export_triplex(m);
}
