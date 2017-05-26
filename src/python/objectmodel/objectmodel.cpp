/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <pybind11/pybind11.h>

namespace visr
{
namespace objectmodel
{
namespace python
{

void exportObject( pybind11::module & m );
void exportObjectType( pybind11::module & m );
void exportObjectVector( pybind11::module & m );
void exportPointSource( pybind11::module & m );
void exportPointSourceWithReverb( pybind11::module & m );
void exportPlaneWave( pybind11::module & m );

} // namespace python
} // namespace objectmodel
} // namespace visr

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
