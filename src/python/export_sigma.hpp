#pragma once
#include <functional>
#include <pybind11/pybind11.h>
#include <sigma/interval/interval.hpp>
#include <sigma/triplex/triplex.hpp>
#include <sstream>
#include <string>

/** @file export_sigma.hpp
 *  @brief Declarations and helpers shared by the Python bindings
 */

namespace sigma {

/** @brief Exports IFloat, IDouble, and the functions acting on them
 *
 *  @param[in,out] m The Python module to add the classes and functions to.
 */
void export_interval(pybind11::module_& m);

/** @brief Exports TPFloat, TPDouble, and the functions acting on them
 *
 *  @param[in,out] m The Python module to add the classes and functions to.
 */
void export_triplex(pybind11::module_& m);

namespace detail_ {

/** @brief Adds the members every exported uncertain type shares
 *
 *  These are the comparison, hashing, printing, and copying special methods,
 *  plus the unary minus. The comparisons only accept another instance of the
 *  same type. Python's fallback for anything else gives `==` False and `!=`
 *  True, which is what the C++ comparisons between different types return.
 *
 *  @tparam T The C++ type wrapped by @p cls.
 *
 *  @param[in,out] cls The Python class to add the members to.
 */
template<typename T>
void export_common_members_(pybind11::class_<T>& cls) {
    namespace py = pybind11;
    // __hash__ goes before __eq__: pybind11 sets __hash__ to None when a class
    // defines __eq__ without one.
    cls.def("__hash__", [](const T& a) { return std::hash<T>{}(a); })
      .def(
        "__eq__", [](const T& a, const T& b) { return a == b; },
        py::is_operator())
      .def(
        "__ne__", [](const T& a, const T& b) { return a != b; },
        py::is_operator())
      .def(
        "__lt__", [](const T& a, const T& b) { return a < b; },
        py::is_operator())
      .def(
        "__gt__", [](const T& a, const T& b) { return a > b; },
        py::is_operator())
      .def(
        "__le__", [](const T& a, const T& b) { return a <= b; },
        py::is_operator())
      .def(
        "__ge__", [](const T& a, const T& b) { return a >= b; },
        py::is_operator())
      .def("__neg__", [](const T& a) { return -a; })
      .def("__str__",
           [](const T& a) {
               std::ostringstream ss;
               ss << a;
               return ss.str();
           })
      .def("__repr__",
           [](const T& a) {
               std::ostringstream ss;
               ss << a;
               return ss.str();
           })
      .def("__copy__", [](const T& a) { return T(a); })
      .def("__deepcopy__", [](const T& a, py::dict) { return T(a); });
}

/** @brief Adds the arithmetic operators of an uncertain type
 *
 *  Each operator accepts another instance of the same type or a plain value
 *  on either side. In-place operators return a reference to their left
 *  operand, so pybind11 hands back the existing Python object.
 *
 *  @tparam T The C++ type wrapped by @p cls.
 *  @tparam ScalarFxn The type of @p to_t.
 *
 *  @param[in,out] cls The Python class to add the operators to.
 *  @param[in] to_t Converts a plain value into the type a C++ operator takes.
 */
template<typename T, typename ScalarFxn>
void export_arithmetic_(pybind11::class_<T>& cls, ScalarFxn to_t) {
    namespace py  = pybind11;
    using value_t = typename T::value_t;
    cls
      .def(
        "__add__", [](const T& a, const T& b) { return a + b; },
        py::is_operator())
      .def(
        "__add__", [to_t](const T& a, value_t b) { return a + to_t(b); },
        py::is_operator())
      .def(
        "__radd__", [to_t](const T& a, value_t b) { return to_t(b) + a; },
        py::is_operator())
      .def(
        "__sub__", [](const T& a, const T& b) { return a - b; },
        py::is_operator())
      .def(
        "__sub__", [to_t](const T& a, value_t b) { return a - to_t(b); },
        py::is_operator())
      .def(
        "__rsub__", [to_t](const T& a, value_t b) { return to_t(b) - a; },
        py::is_operator())
      .def(
        "__mul__", [](const T& a, const T& b) { return a * b; },
        py::is_operator())
      .def(
        "__mul__", [to_t](const T& a, value_t b) { return a * to_t(b); },
        py::is_operator())
      .def(
        "__rmul__", [to_t](const T& a, value_t b) { return to_t(b) * a; },
        py::is_operator())
      .def(
        "__truediv__", [](const T& a, const T& b) { return a / b; },
        py::is_operator())
      .def(
        "__truediv__", [to_t](const T& a, value_t b) { return a / to_t(b); },
        py::is_operator())
      .def(
        "__rtruediv__", [to_t](const T& a, value_t b) { return to_t(b) / a; },
        py::is_operator())
      .def(
        "__iadd__", [](T& a, const T& b) -> T& { return a += b; },
        py::is_operator())
      .def(
        "__iadd__", [to_t](T& a, value_t b) -> T& { return a += to_t(b); },
        py::is_operator())
      .def(
        "__isub__", [](T& a, const T& b) -> T& { return a -= b; },
        py::is_operator())
      .def(
        "__isub__", [to_t](T& a, value_t b) -> T& { return a -= to_t(b); },
        py::is_operator())
      .def(
        "__imul__", [](T& a, const T& b) -> T& { return a *= b; },
        py::is_operator())
      .def(
        "__imul__", [to_t](T& a, value_t b) -> T& { return a *= to_t(b); },
        py::is_operator())
      .def(
        "__itruediv__", [](T& a, const T& b) -> T& { return a /= b; },
        py::is_operator())
      .def(
        "__itruediv__", [to_t](T& a, value_t b) -> T& { return a /= to_t(b); },
        py::is_operator());
}

/** @brief Adds the elementary functions of an uncertain type to a module
 *
 *  Python has one namespace per module, so each function is overloaded across
 *  every exported type. The integer overload of `pow` is registered before the
 *  floating-point one so that `pow(x, 2)` is repeated multiplication, as it is
 *  in C++.
 *
 *  @tparam T The C++ type the functions act on.
 *
 *  @param[in,out] m The Python module to add the functions to.
 */
template<typename T>
void export_functions_(pybind11::module_& m) {
    m.def("abs", [](const T& a) { return sigma::abs(a); });
    m.def("fabs", [](const T& a) { return sigma::fabs(a); });
    m.def("sqrt", [](const T& a) { return sigma::sqrt(a); });
    m.def("exp", [](const T& a) { return sigma::exp(a); });
    m.def("log", [](const T& a) { return sigma::log(a); });
    m.def("pow", [](const T& a, int e) { return sigma::pow(a, e); });
    m.def("pow", [](const T& a, double e) { return sigma::pow(a, e); });
    m.def("pow", [](const T& a, const T& e) { return sigma::pow(a, e); });
    m.def("sin", [](const T& a) { return sigma::sin(a); });
    m.def("cos", [](const T& a) { return sigma::cos(a); });
    m.def("tan", [](const T& a) { return sigma::tan(a); });
    m.def("asin", [](const T& a) { return sigma::asin(a); });
    m.def("acos", [](const T& a) { return sigma::acos(a); });
    m.def("atan", [](const T& a) { return sigma::atan(a); });
    m.def("sinh", [](const T& a) { return sigma::sinh(a); });
    m.def("cosh", [](const T& a) { return sigma::cosh(a); });
    m.def("tanh", [](const T& a) { return sigma::tanh(a); });
    m.def("asinh", [](const T& a) { return sigma::asinh(a); });
    m.def("acosh", [](const T& a) { return sigma::acosh(a); });
    m.def("atanh", [](const T& a) { return sigma::atanh(a); });
}

} // namespace detail_
} // namespace sigma
