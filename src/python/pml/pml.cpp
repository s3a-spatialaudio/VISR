/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "matrix_parameter.hpp" 

#include <boost/python.hpp>

BOOST_INIT_MODULE( pml )
{
  // Call the initialisation routines of all modules.
  exportMatrixParameters();
}
