#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "spo2_algorithm.h"

namespace py = pybind11;

PYBIND11_MODULE(SPO2Algorithm, m) {
  m.def(
    "get_oxygen_level",
    [](py::array_t<float, py::array::c_style | py::array::forcecast> ppg_ir,
       py::array_t<float, py::array::c_style | py::array::forcecast> ppg_red)
    {
        float oxygen_level = 0.0;
        get_oxygen_level(
            const_cast<float*>(ppg_ir.data()),
            const_cast<float*>(ppg_red.data()),
            static_cast<int>(ppg_ir.size()),
            &oxygen_level
        );
        return oxygen_level;
    },
    py::arg("ppg_ir"),
    py::arg("ppg_red")
  );
}