/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanningdsp/types.hpp>

#include <pybind11/pybind11.h>

// Forward declarations
namespace visr
{
namespace panningdsp
{
namespace python
{
  void exportPanningGainMatrix( pybind11::module & m );
  void exportPanningMatrixParameter(pybind11::module & m);
} // namespace python
} // namespace panningdsp
} // namespace visr

PYBIND11_MODULE(panningdsp, m)
{
  namespace py = pybind11;

  py::module::import( "visr" );
  py::module::import( "efl" );
  py::module::import( "pml" );

  m.doc() =
R"(VISR component library for panning-specific processing.
)";

  using namespace visr::panningdsp::python;

  m.attr( "timeStampInfinity" ) = py::int_( visr::panningdsp::cTimeStampInfinity );

  exportPanningGainMatrix( m );
  exportPanningMatrixParameter(m);
}
