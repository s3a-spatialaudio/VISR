/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/LoudspeakerArray.h>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <librsao/reverb_parameter_calculator.hpp> 

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace reverbobject
{

void exportReverbParameterCalculator( pybind11::module& m )
{
  pybind11::class_<::visr::rsao::ReverbParameterCalculator, AtomicComponent>( m, "ReverbParameterCalculator" )
    .def( pybind11::init<SignalFlowContext const &, char const *, CompositeComponent *>(),
        pybind11::arg( "context" ), pybind11::arg( "name" ),
        pybind11::arg( "parent" ) )
    .def( "setup", &visr::rsao::ReverbParameterCalculator::setup,
        pybind11::arg( "arrayConfig"),
        pybind11::arg( "numberOfObjects" ),
        pybind11::arg( "discreteReflectionsPerSource" ),
        pybind11::arg( "discreteReflectionBiquadSections" ),
        pybind11::arg( "lateReflectionLengthseconds" ),
        pybind11::arg( "lateReflectionSubbands" )
      )
    ;
}

} // namepace reverbobject
} // namespace python
} // namespace visr
