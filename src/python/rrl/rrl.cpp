/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include <boost/python.hpp>

BOOST_PYTHON_MODULE( rrl )
{
  using namespace visr::python::rrl;
  exportAudioSignalFlow();
}
