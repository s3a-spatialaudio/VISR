/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/LoudspeakerArray.h>

#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libreverbobject/reverb_object_renderer.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{

namespace python
{
namespace reverbobject
{

void exportReverbObjectRenderer( pybind11::module& m )
{
  pybind11::class_<::visr::reverbobject::ReverbObjectRenderer, CompositeComponent>( m, "ReverbObjectRenderer" )
    .def( pybind11::init<SignalFlowContext const &, char const *, CompositeComponent *,
          std::string const &, panning::LoudspeakerArray const &, std::size_t>(),
        pybind11::arg( "context" ), pybind11::arg( "name" ),
        pybind11::arg( "parent" ),
        pybind11::arg( "reverbConfig" ),
        pybind11::arg( "loudspeakerConfiguration" ),
        pybind11::arg( "numberOfObjectSignals" ) )
    ;
}

} // namepace reverbobject
} // namespace python
} // namespace visr
