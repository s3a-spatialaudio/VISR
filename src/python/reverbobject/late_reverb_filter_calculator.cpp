/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/LoudspeakerArray.h>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libreverbobject/late_reverb_filter_calculator.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{

namespace python
{
namespace reverbobject
{

void exportLateReverbFilterCalculator( pybind11::module& m )
{
  pybind11::class_<::visr::reverbobject::LateReverbFilterCalculator, AtomicComponent>( m, "LateReverbFilterCalculator" )
    .def( pybind11::init<SignalFlowContext const &, char const *, CompositeComponent *, std::size_t, float, std::size_t, std::size_t>(),
        pybind11::arg( "context" ), pybind11::arg( "name" ),
        pybind11::arg( "parent" ),
        pybind11::arg( "numberOfObjects" ),
        pybind11::arg( "lateReflectionLengthSeconds" ),
        pybind11::arg( "numLateReflectionSubBandLevels" ),
        pybind11::arg( "maxUpdatesPerPeriod" )
      )
    ;
}

} // namepace reverbobject
} // namespace python
} // namespace visr
