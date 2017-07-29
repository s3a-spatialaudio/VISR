/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <pybind11/pybind11.h>

// Forward declarations
namespace visr
{
namespace rbbl
{
namespace python
{
  void exportMultichannelConvolversUniform( pybind11::module & m );
}
}
}

PYBIND11_PLUGIN(rbbl)
{
  // MultichannelConvolverUniform uses pml parameter types MatrixParameter and FilterRoutingParameter
  pybind11::module::import( "pml" );

  pybind11::module m( "rbbl", "VISR renderer building block library" );
  using namespace visr::rbbl::python;
  exportMultichannelConvolversUniform( m );
  return m.ptr();
}
