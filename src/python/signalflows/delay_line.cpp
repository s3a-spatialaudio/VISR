/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "core_renderer.hpp"

#include <libpanning/LoudspeakerArray.h>

#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libsignalflows/delay_line.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{

namespace python
{
namespace signalflows
{

void exportCoreRenderer( pybind11::module& m )
{
  pybind11::class_<signalflows::DelayVector, CompositeComponent>( m, "DelayVector" )
    .def( pybind11::init<SignalFlowContext &, char const *, CompositeComponent *,
      panning::LoudspeakerArray const &, std::size_t, std::size_t, std::size_t,
      visr::pml::MatrixParameter<visr::SampleType> const &, std::string const &>(),
        pybind11::arg("context"), pybind11::arg( "name" ),
        pybind11::arg( "parent" ) = static_cast<CompositeComponent*>(nullptr),
        pybind11::arg( "loudspeakerConfiguration" ), pybind11::arg( "numberOfInputs" ),
        pybind11::arg( "numberOfOutputs" ), pybind11::arg( "interpolationPeriod" ),
        pybind11::arg( "diffusionFilters" ), pybind11::arg("trackingConfiguration") )
    ;
}

} // namepace signalflows
} // namespace python
} // namespace visr
