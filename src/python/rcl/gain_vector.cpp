/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/gain_vector.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

void exportGainVector( pybind11::module & m )
{
  using visr::rcl::GainVector;

  pybind11::class_<GainVector, visr::AtomicComponent>( m, "GainVector" )
   .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
    .def( "setup", static_cast<void(GainVector::*)( std::size_t, std::size_t, bool, SampleType)>(&GainVector::setup), 
      pybind11::arg("numberOfChannels"),
      pybind11::arg( "interpolationSteps" ) = 1024,
      pybind11::arg("controlInputs") = false,
      pybind11::arg( "initialGain" ) = 1.0f )
    .def( "setup", static_cast<void(GainVector::*)(std::size_t, std::size_t, bool, efl::BasicVector<SampleType> const &)>(&GainVector::setup),
      pybind11::arg( "numberOfChannels" ),
      pybind11::arg( "interpolationSteps" ) = 1024,
      pybind11::arg( "controlInputs" ) = false,
      pybind11::arg( "initialGainsLinear" ) )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
