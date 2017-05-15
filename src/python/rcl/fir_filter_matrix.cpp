/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/fir_filter_matrix.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

#if 0
  void setup( std::size_t numberOfInputs,
              std::size_t numberOfOutputs,
              std::size_t filterLength,
              std::size_t maxFilters,
              std::size_t maxRoutings,
              efl::BasicMatrix<SampleType> const & filters = efl::BasicMatrix<SampleType>(),
              pml::FilterRoutingList const & routings = pml::FilterRoutingList(),
              bool controlInputs = false,
              char const * fftImplementation = "default" );
#endif

void exportFirFilterMatrix( pybind11::module & m )
{
  using visr::rcl::FirFilterMatrix;

  pybind11::class_<FirFilterMatrix, visr::AtomicComponent>( m, "FirFilterMatrix" )
    .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
   .def( "setup", &FirFilterMatrix::setup, pybind11::arg("numberOfInputs"),
      pybind11::arg("numberOfOutputs"),
      pybind11::arg("filterLength"),
      pybind11::arg("maxFilters"),
      pybind11::arg("maxRoutings"),
      pybind11::arg("filters")/* = efl::BasicMatrix<SampleType>()*/,
      pybind11::arg("routings")/* = pml::FilterRoutingList()*/,
      pybind11::arg( "controlInputs" ) = false,
      pybind11::arg( "fftImplementation" ) = "default" )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
