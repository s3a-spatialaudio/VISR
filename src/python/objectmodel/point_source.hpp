/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED
#define VISR_PYTHON_VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED

#include <pybind11/pybind11.h>


namespace visr
{
namespace objectmodel
{
namespace python
{

void exportPointSource( pybind11::module& m );

} // namespace python
} // namespace objectmodel
} // namespace visr

#endif // #ifndef VISR_PYTHON_VISR_OBJECTMODEL_POINT_SOURCE_HPP_INCLUDED
