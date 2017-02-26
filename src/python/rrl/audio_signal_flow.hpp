/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHON_RRL_AUDIO_SIGNAL_FLOW_INCLUDED
#define VISR_PYTHON_RRL_AUDIO_SIGNAL_FLOW_INCLUDED

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#endif


namespace visr
{
namespace python
{
namespace rrl
{

#ifdef USE_PYBIND11

void exportAudioSignalFlow( pybind11::module & m );

#else

void exportAudioSignalFlow();

#endif

} // namepace rrl
} // namespace python
} // namespace visr

#endif // #ifndef VISR_PYTHON_RRL_AUDIO_SIGNAL_FLOW_INCLUDED
