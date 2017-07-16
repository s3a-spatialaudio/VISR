/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_VISR_CHANNEL_LIST_INCLUDED
#define VISR_PYTHON_VISR_CHANNEL_LIST_INCLUDED

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace visr
{

void exportChannelList( pybind11::module& m );

} // namespace visr
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_VISR_CHANNEL_LIST_INCLUDED
