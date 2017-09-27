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
void exportHoaSource( pybind11::module & m );
void exportPointSource( pybind11::module & m );
void exportPointSourceWithReverb( pybind11::module & m );
void exportPlaneWave( pybind11::module & m );

} // namespace python
} // namespace objectmodel
} // namespace visr

PYBIND11_MODULE( objectmodel, m )
{
  
  using namespace visr::objectmodel::python;

  exportObject( m );
  exportObjectType( m );
  exportObjectVector( m );

  // Concrete object types after base classes.
  exportHoaSource( m );
  exportPointSource( m );
  exportPointSourceWithReverb( m );
  exportPlaneWave( m );
}
