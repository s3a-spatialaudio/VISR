/* Copyright Institute of Sound and Vibration Research - All rights reserved */


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
  pybind11::module::import( "visr" );
  pybind11::module::import( "efl" );
  pybind11::module::import( "pml" );

  m.doc() =
R"(VISR component library for panning-specific processing.
)";

  using namespace visr::panningdsp::python;
  exportPanningGainMatrix( m );
  exportPanningMatrixParameter(m);
}
