/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "atomic_component.hpp"
#include "audio_port.hpp"
#include "channel_list.hpp"
#include "communication_protocol.hpp"
#include "communication_protocol_factory.hpp"
#include "component.hpp"
#include "composite_component.hpp"
#include "parameter_base.hpp"
#include "parameter_config_base.hpp"
#include "parameter_port.hpp"
#include "signal_flow_context.hpp"

#include <pybind11/pybind11.h>

PYBIND11_PLUGIN( visr )
{
  pybind11::module m( "visr", "VISR core API module" );
  using namespace visr::python::visr;
  exportSignalFlowContext( m );
  exportParameterConfigBase( m );
  exportParameterBase( m );
  exportCommunicationProtocol( m );
  exportCommunicationProtocolFactory( m );
  exportChannelList( m );
  exportComponent( m );
  exportAudioPort( m );
  exportParameterPort(m);
  exportCompositeComponent( m );
  exportAtomicComponent( m );
  return m.ptr();
}
