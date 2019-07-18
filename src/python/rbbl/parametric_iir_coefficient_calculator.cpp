/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/parametric_iir_coefficient_calculator.hpp>
#include <librbbl/parametric_iir_coefficient.hpp>
#include <librbbl/biquad_coefficient.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <array>
#include <string>
#include <sstream>
#include <vector>

namespace visr
{
namespace rbbl
{
namespace python
{

namespace py = pybind11;

void exportParametricIirCoefficientCalculator( pybind11::module & m )
{
  py::class_<ParametricIirCoefficientCalculator >(m, "ParametricIirCoefficientCalculator")
    .def_static("calculateIirCoefficients",
        static_cast<void(*)(ParametricIirCoefficient<float> const &, BiquadCoefficient<float> &, float)>(&ParametricIirCoefficientCalculator::calculateIirCoefficients<float>),
        py::arg("param"), py::arg("coeffs"), py::arg("samplingFrequency") )
    .def_static("calculateIirCoefficients",
        static_cast<BiquadCoefficient<float>(*)(ParametricIirCoefficient<float> const &, float)>(&ParametricIirCoefficientCalculator::calculateIirCoefficients<float>),
        py::arg("param"), py::arg("samplingFrequency") )
    .def_static("calculateIirCoefficients",
        static_cast<void(*)(ParametricIirCoefficientList<float> const &, BiquadCoefficientList<float> &, float)>(&ParametricIirCoefficientCalculator::calculateIirCoefficients<float>),
        py::arg("params"), py::arg("coeffs"), py::arg("samplingFrequency"))
    .def_static("calculateIirCoefficients",
        static_cast<void(*)(ParametricIirCoefficient<double> const &, BiquadCoefficient<double> &, double)>(&ParametricIirCoefficientCalculator::calculateIirCoefficients<double>),
        py::arg("param"), py::arg("coeffs"), py::arg("samplingFrequency"))
    .def_static("calculateIirCoefficients",
        static_cast<BiquadCoefficient<double>(*)(ParametricIirCoefficient<double> const &, double)>(&ParametricIirCoefficientCalculator::calculateIirCoefficients<double>),
        py::arg("param"), py::arg("samplingFrequency"))
    .def_static("calculateIirCoefficients",
    static_cast<void(*)(ParametricIirCoefficientList<double> const &, BiquadCoefficientList<double> &, double)>(&ParametricIirCoefficientCalculator::calculateIirCoefficients<double>),
    py::arg("params"), py::arg("coeffs"), py::arg("samplingFrequency"))
    ;
}

} // namespace python
} // namepace rbbl
} // namespace visr
