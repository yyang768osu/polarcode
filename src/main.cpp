#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "polar_list_decoder.hpp"

int add(int i, int j) {
    return i + j;
}

namespace py = pybind11;

PYBIND11_MODULE(polarcode, m) {
    m.def("add", &add);
    m.def("subtract", [](int i, int j) { return i - j; });
    py::class_<PolarListDecoder>(m, "PolarListDecoder")
        .def(py::init<int, int>())
        .def_property_readonly("n", &PolarListDecoder::get_n)
        .def("decode", &PolarListDecoder::decode);
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
