/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <libpml/initialise_parameter_library.hpp>

#include "double_buffering_protocol.hpp"
#include "empty_parameter_config.hpp"
#include "listener_position.hpp"
#include "loudspeaker_array.hpp" // kept here temporarily.
#include "matrix_parameter.hpp"
#include "vector_parameter.hpp"

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#endif

#ifdef USE_PYBIND11

PYBIND11_PLUGIN( pml )
{
  pybind11::module::import( "visr" );

  pybind11::module m( "pml", "VISR parameter message module" );
  using namespace visr::python::pml;

  // Register parameter types and communication protocols
  visr::pml::initialiseParameterLibrary();

  // Export the communication protocols
  exportDoubleBufferingProtocol( m );

  exportEmptyParameterConfig( m );
  exportListenerPosition( m );
  exportLoudspeakerArray( m );
  exportMatrixParameters( m);
  exportVectorParameters( m );
  return m.ptr();
}

#else

BOOST_PYTHON_MODULE( pml )
{
  // Call the initialisation routines of all modules.
  using namespace visr::python::pml;
  exportListenerPosition();
  exportLoudspeakerArray();
  exportMatrixParameters();
}

#endif
