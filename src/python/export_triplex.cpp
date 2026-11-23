#include "export_sigma.hpp"

namespace sigma {
namespace {

/** @brief Exports Triplex<IntervalType> as a Python class named @p name
 *
 *  There is deliberately no constructor from an interval alone, since an
 *  interval does not determine a nominal value.
 *
 *  @tparam IntervalType The type of the triplex's bounds.
 *
 *  @param[in,out] m The Python module to add the class to.
 *  @param[in] name The name of the Python class.
 */
template<typename IntervalType>
void export_triplex_(pybind11::module_& m, const char* name) {
    namespace py     = pybind11;
    using triplex_t  = Triplex<IntervalType>;
    using interval_t = typename triplex_t::interval_t;
    using value_t    = typename triplex_t::value_t;

    py::class_<triplex_t> cls(m, name);
    cls.def(py::init<>())
      .def(py::init<value_t>())
      .def(py::init<value_t, value_t, value_t>())
      .def(py::init<interval_t, value_t>())
      .def("empty", &triplex_t::empty)
      .def("lower", &triplex_t::lower)
      .def("nominal", &triplex_t::nominal)
      .def("upper", &triplex_t::upper)
      .def("width", &triplex_t::width)
      .def("median", &triplex_t::median)
      .def("lower_radius", &triplex_t::lower_radius)
      .def("upper_radius", &triplex_t::upper_radius)
      .def("interval", [](const triplex_t& t) { return t.interval(); })
      .def("contains", py::overload_cast<const triplex_t&>(&triplex_t::contains,
                                                           py::const_))
      .def("contains",
           py::overload_cast<value_t>(&triplex_t::contains, py::const_));

    detail_::export_common_members_(cls);
    // A plain value converts implicitly to a triplex in C++, which is how
    // mixed triplex-scalar arithmetic works there
    detail_::export_arithmetic_(cls, [](value_t b) { return triplex_t(b); });
    detail_::export_functions_<triplex_t>(m);
}

} // namespace

void export_triplex(pybind11::module_& m) {
    export_triplex_<Interval<float>>(m, "TPFloat");
    export_triplex_<Interval<double>>(m, "TPDouble");
}

} // namespace sigma
