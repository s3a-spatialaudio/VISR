/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"
#include "integrity_checking.hpp"

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#endif

#ifdef USE_PYBIND11

PYBIND11_PLUGIN( rrl )
{
  pybind11::module m( "rrl", "VISR renderer runtime module" );
  using namespace visr::python::rrl;
  exportAudioSignalFlow( m );
  exportIntegrityChecking( m );
  return m.ptr();
}
#else
BOOST_PYTHON_MODULE( rrl )
{
  using namespace visr::python::rrl;
  exportAudioSignalFlow();
  exportCheckIntegrity();
}
#endif
