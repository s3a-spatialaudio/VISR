/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_PML_VECTOR_PARAMETER_HPP_INCLUDED
#define VISR_PYTHON_PML_VECTOR_PARAMETER_HPP_INCLUDED

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace pml
{

/**
 * Create Python bindings for pml::MatrixParameter<float> and pml::MatrixParameter<double>
 */
void exportVectorParameters( pybind11::module & m );

} // namepace pml
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_PML_VECTOR_PARAMETER_HPP_INCLUDED
