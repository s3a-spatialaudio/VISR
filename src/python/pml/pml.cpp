/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libpml/initialise_parameter_library.hpp>

#include "double_buffering_protocol.hpp"
// #include "empty_parameter_config.hpp"
#include "listener_position.hpp"
#include "matrix_parameter.hpp"
#include "message_queue_protocol.hpp"
#include "object_vector.hpp"
#include "vector_parameter.hpp"

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace pml
{
void exportEmptyParameterConfig( pybind11::module & m );
void exportFilterRoutingParameter( pybind11::module & m );
void exportIndexedValueParameters( pybind11::module & m );
}
}
}


PYBIND11_PLUGIN( pml )
{
  pybind11::module::import( "visr" );

  pybind11::module m( "pml", "VISR parameter message module" );
  using namespace visr::python::pml;

  // Register parameter types and communication protocols
  visr::pml::initialiseParameterLibrary();

  // Export the communication protocols
  exportDoubleBufferingProtocol( m );
  exportMessageQueueProtocol( m );

  exportEmptyParameterConfig( m );
  exportFilterRoutingParameter( m );
  exportIndexedValueParameters( m );
  exportListenerPosition( m );
  exportMatrixParameters( m);
  exportObjectVector( m );
  exportVectorParameters( m );

  return m.ptr();
}
