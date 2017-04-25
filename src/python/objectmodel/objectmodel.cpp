/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include "object.hpp"
#include "object_type.hpp"
#include "object_vector.hpp"
#include "point_source.hpp"
#include "plane_wave.hpp"

#include <pybind11/pybind11.h>


PYBIND11_PLUGIN(objectmodel)
{
  pybind11::module m( "objectmodel", "VISR object model library" );
  
  using namespace visr::objectmodel::python;

  exportObject( m );
  exportObjectType( m );
  exportObjectVector( m );
  exportPointSource( m );
  exportPlaneWave( m );

  return m.ptr();
}
