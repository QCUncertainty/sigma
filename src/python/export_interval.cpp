#include "export_sigma.hpp"

namespace sigma {
namespace {

/** @brief Exports Interval<T> as a Python class named @p name
 *
 *  @tparam T The floating-point type of the bounds.
 *
 *  @param[in,out] m The Python module to add the class to.
 *  @param[in] name The name of the Python class.
 */
template<typename T>
void export_interval_(pybind11::module_& m, const char* name) {
    namespace py     = pybind11;
    using interval_t = Interval<T>;
    using value_t    = typename interval_t::value_t;

    py::class_<interval_t> cls(m, name);
    cls.def(py::init<>())
      .def(py::init<value_t>())
      .def(py::init<value_t, value_t>())
      .def("width", &interval_t::width)
      .def("median", &interval_t::median)
      .def("radius", &interval_t::radius)
      .def("empty", &interval_t::empty)
      .def("lower", &interval_t::lower)
      .def("upper", &interval_t::upper)
      .def("contains", py::overload_cast<const interval_t&>(
                         &interval_t::contains, py::const_))
      .def("contains",
           py::overload_cast<value_t>(&interval_t::contains, py::const_))
      .def("set_union", &interval_t::set_union)
      .def("set_intersection", &interval_t::set_intersection)
      .def("print_interval_form", &interval_t::print_interval_form);

    detail_::export_common_members_(cls);
    detail_::export_arithmetic_(cls, [](value_t b) { return b; });
    detail_::export_functions_<interval_t>(m);
    m.def("widen",
          [](const interval_t& a, value_t point) { return widen(a, point); });
}

} // namespace

void export_interval(pybind11::module_& m) {
    export_interval_<float>(m, "IFloat");
    export_interval_<double>(m, "IDouble");
}

} // namespace sigma
