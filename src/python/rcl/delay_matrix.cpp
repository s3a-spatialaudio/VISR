/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/delay_matrix.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

namespace visr
{
namespace python
{
namespace rcl
{

void exportDelayMatrix( pybind11::module & m )
{
  using visr::rcl::DelayMatrix;

  pybind11::class_<DelayMatrix, visr::AtomicComponent> dVec( m, "DelayMatrix" );

  // Note: Consider moving to rbbl::MultichannelDelayLine (the type in DelayMatrix is only a typedef of that one)
  // Note: This binding as used by DelayVector as well (that binding code has to be called after this definition).
  pybind11::enum_<DelayMatrix::MethodDelayPolicy>( dVec, "MethodDelayPolicy" )
    .value( "Add", DelayMatrix::MethodDelayPolicy::Add)
    .value( "Limit", DelayMatrix::MethodDelayPolicy::Limit)
    .value( "Reject", DelayMatrix::MethodDelayPolicy::Reject )
    ;

  pybind11::enum_<DelayMatrix::ControlPortConfig>( dVec, "ControlPortConfig" )
    .value( "No", DelayMatrix::ControlPortConfig::None ) // Cannot use 'None' because that is recognised as keyword in Python.
    .value( "Delay", DelayMatrix::ControlPortConfig::Delay )
    .value( "Gain", DelayMatrix::ControlPortConfig::Gain )
    .value( "All", DelayMatrix::ControlPortConfig::All )
    .def( pybind11::self | pybind11::self )
    .def( pybind11::self & pybind11::self )
    ;

  dVec.def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
    .def( "setup", static_cast<void(DelayMatrix::*)( std::size_t, std::size_t, std::size_t, SampleType,
      const char *, DelayMatrix::MethodDelayPolicy, DelayMatrix::ControlPortConfig, SampleType, SampleType)>(&DelayMatrix::setup),
      pybind11::arg( "numberOfInputs" ),
      pybind11::arg( "numberOfOutputs"),
      pybind11::arg( "interpolationSteps" ) = 1024, pybind11::arg( "maxDelay" ) = 3.0f,
      pybind11::arg( "interpolationType" ) /*= DelayMatrix::InterpolationType::CubicLagrange*/,
      pybind11::arg( "methodDelayPolicy") = DelayMatrix::MethodDelayPolicy::Add,
      pybind11::arg( "controlInputs") = DelayMatrix::ControlPortConfig::None,
      pybind11::arg( "initialDelay" ) = 0.0f,
      pybind11::arg( "initialGain" ) = 1.0f )
    .def( "setup", static_cast<void(DelayMatrix::*)( std::size_t, std::size_t, std::size_t, SampleType,
        const char *, DelayMatrix::MethodDelayPolicy, DelayMatrix::ControlPortConfig, efl::BasicMatrix<SampleType> const &, efl::BasicMatrix<SampleType>  const &)>(&DelayMatrix::setup),
        pybind11::arg( "numberOfInputs" ),
        pybind11::arg( "numberOfOutputs"),
        pybind11::arg( "interpolationSteps" ) = 1024,
        pybind11::arg( "maxDelay" ) = 3.0f,
        pybind11::arg( "interpolationType" ) /*= DelayMatrix::InterpolationType::CubicLagrange*/,
        pybind11::arg( "methodDelayPolicy") = DelayMatrix::MethodDelayPolicy::Add,
        pybind11::arg( "controlInputs") = DelayMatrix::ControlPortConfig::None,
        pybind11::arg( "initialDelays" ) /*= efl::BasicMatrix<SampleType>()*/,
        pybind11::arg( "initialGains" ) /*= efl::BasicMatrix<SampleType>()*/ )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
