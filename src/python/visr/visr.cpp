/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "atomic_component.hpp"
#include "channel_list.hpp"
#include "audio_port.hpp"
#include "component.hpp"
#include "composite_component.hpp"
#include "parameter_config_base.hpp"
#include "signal_flow_context.hpp"

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#endif

#ifdef USE_PYBIND11

PYBIND11_PLUGIN( visr )
{
  pybind11::module m( "visr", "VISR core API module" );
  using namespace visr::python::visr;
  exportSignalFlowContext( m );
  exportChannelList( m );
  exportAudioPort( m );
  exportComponent( m );
  exportCompositeComponent( m );
  exportAtomicComponent( m );
  exportParameterConfigBase( m );
  return m.ptr();
}

#else
BOOST_PYTHON_MODULE( visr )
{
  // Call the initialisation routines of all modules.
  // This is the standard way to creeate Python modules from bindings in multiple files.
  using namespace visr::python::visr;
  exportAtomicComponent();
  exportChannelList();
  exportAudioPort();
  exportSignalFlowContext();
  exportComponent();
  exportCompositeComponent();
  exportParameterConfigBase( );
}
#endif
