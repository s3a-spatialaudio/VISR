/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_SIGNALFLOWS_DELAY_VECTOR_HPP_INCLUDED
#define VISR_PYTHON_SIGNALFLOWS_DELAY_VECTOR_HPP_INCLUDED

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace signalflows
{

void exportDelayVector( pybind11::module& m );

} // namepace signalflows
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_SIGNALFLOWS_DELAY_VECTOR_HPP_INCLUDED
