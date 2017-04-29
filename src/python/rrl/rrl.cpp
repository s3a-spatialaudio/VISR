/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"
#include "integrity_checking.hpp"

#include <pybind11/pybind11.h>

PYBIND11_PLUGIN( rrl )
{
  pybind11::module m( "rrl", "VISR renderer runtime module" );
  using namespace visr::python::rrl;
  exportAudioSignalFlow( m );
  exportIntegrityChecking( m );
  return m.ptr();
}
