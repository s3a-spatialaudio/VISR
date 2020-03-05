/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpml/initialise_parameter_library.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace efl
{
namespace python
{
void exportBasicVectors( pybind11::module & m );
void exportBasicMatrices( pybind11::module & m );
void exportDenormalisedNumberHandling( pybind11::module & m );
}
}
}


PYBIND11_MODULE( efl, m )
{
  using namespace visr::efl::python;
  exportBasicVectors( m );
  exportBasicMatrices( m );
  exportDenormalisedNumberHandling( m );
}
