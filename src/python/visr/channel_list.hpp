/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_VISR_CHANNEL_LIST_INCLUDED
#define VISR_PYTHON_VISR_CHANNEL_LIST_INCLUDED

#ifdef USE_PYBIND11
#include <pybind11.h>
#endif


namespace visr
{
namespace python
{
namespace visr
{

#ifdef USE_PYBIND11
void exportChannelList( pybind11::module& m );
#else
void exportChannelList();
#endif

} // namespace visr
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_VISR_CHANNEL_LIST_INCLUDED
