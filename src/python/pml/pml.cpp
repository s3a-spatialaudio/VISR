/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "listener_position.hpp"
#include "loudspeaker_array.hpp" // kept here temporarily.
#include "matrix_parameter.hpp" 

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
  exportListenerPosition( m );
  exportLoudspeakerArray( m );
  exportMatrixParameters( m);
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
