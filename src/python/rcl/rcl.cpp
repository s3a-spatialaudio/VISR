/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <pybind11/pybind11.h>

// Forward declarations
namespace visr
{
namespace python
{
namespace rcl
{
  void exportAdd( pybind11::module & m );
  void exportDelayVector( pybind11::module & m );
  void exportFirFilterMatrix( pybind11::module & m );
  void exportGainVector( pybind11::module & m );
  void exportHoaAllRadGainCalculator( pybind11::module & m );
  void exportPanningCalculator( pybind11::module & m );
}
}
}

PYBIND11_PLUGIN(rcl)
{
  pybind11::module::import( "visr" );
  pybind11::module::import( "panning" );
  pybind11::module::import( "pml" );

  pybind11::module m( "rcl", "VISR atomic component library" );
  using namespace visr::python::rcl;
  exportAdd( m );
  exportDelayVector( m );
  exportFirFilterMatrix( m );
  exportGainVector( m );
  exportHoaAllRadGainCalculator( m );
  exportPanningCalculator( m );
  return m.ptr();
}
