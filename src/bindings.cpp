#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "spo2_algorithm.h"

namespace py = pybind11;

PYBIND11_MODULE(SPO2Algorithm, m) {
  m.def(
    "get_oxygen_level",
    [](py::array_t<double, py::array::c_style | py::array::forcecast> ppg_ir,
       py::array_t<double, py::array::c_style | py::array::forcecast> ppg_red,
       int sampling_rate,
       double callib_coef1,
       double callib_coef2,
       double callib_coef3)
    {
        double oxygen_level = 0.0;
        get_oxygen_level(
            const_cast<double*>(ppg_ir.data()),
            const_cast<double*>(ppg_red.data()),
            static_cast<int>(ppg_ir.size()),
            sampling_rate,
            callib_coef1,
            callib_coef2,
            callib_coef3,
            &oxygen_level
        );
        return oxygen_level;
    },
    py::arg("ppg_ir"),
    py::arg("ppg_red"),
    py::arg("sampling_rate"),
    py::arg("callib_coef1"),
    py::arg("callib_coef2"),
    py::arg("callib_coef3")
  );
}