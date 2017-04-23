/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "listener_position.hpp"
#include "loudspeaker_array.hpp" // kept here temporarily.
#include "matrix_parameter.hpp" 

#include <boost/python.hpp>

BOOST_PYTHON_MODULE( pml )
{
  // Call the initialisation routines of all modules.
  using namespace visr::python::pml;
  exportListenerPosition();
  exportLoudspeakerArray();
  exportMatrixParameters();

}
