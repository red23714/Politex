#include <boost/python.hpp>
#include "example.h"

using namespace boost::python;

// Обёртка для класса Example
BOOST_PYTHON_MODULE(example) {
    class_<Example>("Example", init<size_t>())  // Класс и его конструктор
        .def("getValue", &Example::getValue)  // Метод getValue
        .def("setValue", &Example::setValue);  // Метод setValue
}