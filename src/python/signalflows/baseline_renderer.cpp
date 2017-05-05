/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "baseline_renderer.hpp"

#include <libpanning/LoudspeakerArray.h>


#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libsignalflows/baseline_renderer.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{

using signalflows::BaselineRenderer;

namespace python
{
namespace signalflows
{

void exportBaselineRenderer( pybind11::module& m )
{
  pybind11::class_<BaselineRenderer, CompositeComponent>( m, "BaselineRenderer" )
    .def( pybind11::init< visr::SignalFlowContext const&, char const*, visr::CompositeComponent *, visr::panning::LoudspeakerArray const &,
                          std::size_t, std::size_t, std::size_t, visr::pml::MatrixParameter<visr::SampleType> const &, std::string const &, std::size_t,
                          std::size_t, std::string const &, bool>(),
          pybind11::arg("context"),
          pybind11::arg("name"),
          pybind11::arg("parent"),
          pybind11::arg("loudspeakerConfig"),
          pybind11::arg("numberOfInputs"),
          pybind11::arg("numberOfOutputs"),
          pybind11::arg("interpolationPeriod"),
          pybind11::arg("diffusionFilters"),
          pybind11::arg("trackingConfiguration")="",
          pybind11::arg("sceneReceiverPort")=4242,
          pybind11::arg("numberEqSections")=0,
          pybind11::arg("reverbConfig")="",
          pybind11::arg("frequencyDependentPanning")=false
      );
}

} // namepace signalflows
} // namespace python
} // namespace visr
