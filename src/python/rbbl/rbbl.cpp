/* Copyright Institute of Sound and Vibration Research - All rights reserved */


#include <pybind11/pybind11.h>

// Forward declarations
namespace visr
{
namespace rbbl
{
namespace python
{
  void exportBiquadCoefficients( pybind11::module & m );
  void exportMultichannelConvolversUniform( pybind11::module & m );
  void exportObjectChannelAllocator( pybind11::module & m );
}
}
}

PYBIND11_MODULE( rbbl, m )
{
  // MultichannelConvolverUniform uses pml parameter types MatrixParameter and FilterRoutingParameter
  pybind11::module::import( "pml" );

  using namespace visr::rbbl::python;
  exportBiquadCoefficients( m );
  exportMultichannelConvolversUniform( m );
  exportObjectChannelAllocator( m );
}
