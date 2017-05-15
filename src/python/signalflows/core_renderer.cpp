/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/LoudspeakerArray.h>

#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libsignalflows/core_renderer.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{

namespace python
{
namespace signalflows
{

void exportCoreRenderer( pybind11::module& m )
{
  pybind11::class_<::visr::signalflows::CoreRenderer, CompositeComponent>( m, "CoreRenderer" )
    .def( pybind11::init<SignalFlowContext const &, char const *, CompositeComponent *,
      panning::LoudspeakerArray const &, std::size_t, std::size_t, std::size_t,
        efl::BasicMatrix<SampleType> const &, std::string const &, std::size_t, std::string const &, bool>(),
        pybind11::arg("context"), pybind11::arg( "name" ),
        pybind11::arg( "parent" ) = static_cast<CompositeComponent*>(nullptr),
        pybind11::arg( "loudspeakerConfiguration" ), pybind11::arg( "numberOfInputs" ),
        pybind11::arg( "numberOfOutputs" ), pybind11::arg( "interpolationPeriod" ),
        pybind11::arg( "diffusionFilters" ), pybind11::arg("trackingConfiguration")=std::string(),
        pybind11::arg("numberEqSections") = 0, pybind11::arg("reverbConfig")=std::string(),
        pybind11::arg("frequencyDependentPanning")=false )
    ;
}

} // namepace signalflows
} // namespace python
} // namespace visr
