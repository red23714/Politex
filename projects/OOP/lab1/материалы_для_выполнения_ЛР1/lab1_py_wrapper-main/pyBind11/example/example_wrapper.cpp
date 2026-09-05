#include <pybind11/pybind11.h>
#include "example.h"

namespace py = pybind11;

PYBIND11_MODULE(example, m) {
    py::class_<Example>(m, "Example")
        .def(py::init<int>())  // Конструктор с параметром int
        .def("getValue", &Example::getValue)  // Метод getValue
        .def("setValue", &Example::setValue);  // Метод setValue
}