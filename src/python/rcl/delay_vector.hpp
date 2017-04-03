/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_RCL_DELAY_VECOR_HPP_INCLUDED
#define VISR_PYTHON_RCL_DELAY_VECOR_HPP_INCLUDED

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#endif

namespace visr
{
namespace python
{
namespace rcl
{

#ifdef USE_PYBIND11
void exportDelayVector( pybind11::module & m );
#else
void exportAdd();
#endif

} // namepace rcl
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_RCL_DELAY_VECOR_HPP_INCLUDED
