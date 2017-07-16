/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_VISR_COMPOSITE_COMPONENT_INCLUDED
#define VISR_PYTHON_VISR_COMPOSITE_COMPONENT_INCLUDED

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace visr
{

void exportCompositeComponent( pybind11::module& m );

} // namepace visr
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_VISR_COMPOSITE_COMPONENT_INCLUDED
