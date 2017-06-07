/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/delay_vector.hpp>

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

void exportDelayVector( pybind11::module & m )
{
  using visr::rcl::DelayVector;

  pybind11::class_<DelayVector, visr::AtomicComponent> dVec( m, "DelayVector" );

  pybind11::enum_<DelayVector::MethodDelayPolicy>( dVec, "MethodDelayPolicy" )
    .value( "Add", DelayVector::MethodDelayPolicy::Add)
    .value( "Limit", DelayVector::MethodDelayPolicy::Limit)
    .value( "Reject", DelayVector::MethodDelayPolicy::Reject )
    ;


  dVec.def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
    .def( "setup", static_cast<void(DelayVector::*)( std::size_t, std::size_t, SampleType, 
      const char *, DelayVector::MethodDelayPolicy, bool, SampleType, SampleType)>(&DelayVector::setup), pybind11::arg("numberOfChannels"),
      pybind11::arg( "interpolationSteps" ) = 1024, pybind11::arg( "maxDelay" ) = 3.0f,
      pybind11::arg( "interpolationType" ) /*= DelayVector::InterpolationType::CubicLagrange*/,
      pybind11::arg( "methodDelayPolicy") = DelayVector::MethodDelayPolicy::Add,
      pybind11::arg( "controlInputs") = false,
      pybind11::arg( "initialDelay" ) = 0.0f,
      pybind11::arg( "initialGain" ) = 1.0f )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
