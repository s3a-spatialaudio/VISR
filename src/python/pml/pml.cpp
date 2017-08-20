/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/initialise_parameter_library.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace pml
{
void exportDoubleBufferingProtocol( pybind11::module & m );
void exportMessageQueueProtocol( pybind11::module & m );
void exportSharedDataProtocol( pybind11::module & m );

void exportEmptyParameterConfig( pybind11::module & m );
void exportFilterRoutingParameter( pybind11::module & m );
void exportIndexedValueParameters( pybind11::module & m );
void exportListenerPosition( pybind11::module & m );
void exportMatrixParameters( pybind11::module & m );
void exportObjectVector( pybind11::module & m );
void exportScalarParameters( pybind11::module & m );
void exportSignalRoutingParameter( pybind11::module & m );
void exportStringParameter( pybind11::module & m );
void exportVectorParameters( pybind11::module & m );
}
}
}


PYBIND11_PLUGIN( pml )
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "objectmodel" );

  pybind11::module m( "pml", "VISR parameter message module" );
  using namespace visr::python::pml;

  // Export the communication protocols
  exportDoubleBufferingProtocol( m );
  exportMessageQueueProtocol( m );
  exportSharedDataProtocol( m );

  exportEmptyParameterConfig( m );
  exportFilterRoutingParameter( m );
  exportIndexedValueParameters( m );
  exportListenerPosition( m );
  exportMatrixParameters( m);
  exportObjectVector( m );
  exportScalarParameters( m );
  exportSignalRoutingParameter( m );
  exportStringParameter( m );
  exportVectorParameters( m );

  // Register parameter types and communication protocols
  visr::pml::initialiseParameterLibrary();

  return m.ptr();
}
