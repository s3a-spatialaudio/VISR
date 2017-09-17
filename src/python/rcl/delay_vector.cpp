/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/delay_vector.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libpml/matrix_parameter.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

namespace py = pybind11;

void exportDelayVector( py::module & m )
{
  using visr::rcl::DelayVector;

  py::class_<DelayVector, visr::AtomicComponent> dVec( m, "DelayVector" );

  // Note: We rely that rbbl::MultichannelDelayLine::MethodDelayPolicy (the type behind the alias DelayVector::MethodDelayPolicy)
  // is already bound in python/rcl/delay_matrix.cpp (this call has to happen before exportDelayVector)
  // TODO: Consider creating a Python module for librbbl, bind rbbl::MultichannelDelayLine::MethodDelayPolicy there and add
  // it to the import dependencies of the rcl module

  dVec.def( py::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*>(),
      py::arg("context"), py::arg("name"), py::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
    .def( "setup", static_cast<void(DelayVector::*)( std::size_t, std::size_t, SampleType, 
      const char *, DelayVector::MethodDelayPolicy, bool, SampleType, SampleType)>(&DelayVector::setup), py::arg("numberOfChannels"),
      py::arg( "interpolationSteps" ) = 1024, py::arg( "maxDelay" ) = 3.0f,
      py::arg( "interpolationType" ) /*= DelayVector::InterpolationType::CubicLagrange*/,
      py::arg( "methodDelayPolicy") = DelayVector::MethodDelayPolicy::Add,
      py::arg( "controlInputs") = false,
      py::arg( "initialDelay" ) = 0.0f,
      py::arg( "initialGain" ) = 1.0f )
    .def( py::init(
      []( visr::SignalFlowContext const& context, char const * name , visr::CompositeComponent* parent,
          std::size_t numberOfChannels, std::size_t interpolationSteps, SampleType maxDelay, const char * interpolationMethod,
          DelayVector::MethodDelayPolicy methodDelayPolicy, bool controlInputs, SampleType initialDelay, SampleType initialGain)
      {
        DelayVector * inst = new DelayVector( context, name, parent );
        inst->setup( numberOfChannels, interpolationSteps, maxDelay, interpolationMethod,
                    methodDelayPolicy, controlInputs, initialDelay, initialGain );
        return inst;
      }),
      py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ),
      py::arg( "numberOfChannels" ),
      py::arg( "interpolationSteps" ) = 1024, py::arg( "maxDelay" ) = 3.0f,
      py::arg( "interpolationType" ) /*= DelayVector::InterpolationType::CubicLagrange*/,
      py::arg( "methodDelayPolicy" ) = DelayVector::MethodDelayPolicy::Add,
      py::arg( "controlInputs" ) = false,
      py::arg( "initialDelay" ) = 0.0f,
      py::arg( "initialGain" ) = 1.0f )
    .def( py::init(
      []( visr::SignalFlowContext const& context, char const * name, visr::CompositeComponent* parent,
        std::size_t numberOfChannels, std::size_t interpolationSteps, SampleType maxDelay, const char * interpolationMethod,
        DelayVector::MethodDelayPolicy methodDelayPolicy, bool controlInputs, efl::BasicVector< SampleType > const & initialDelaysSeconds,
        efl::BasicVector< SampleType > const & initialGainsLinear )
      {
        DelayVector * inst = new DelayVector( context, name, parent );
        inst->setup( numberOfChannels, interpolationSteps, maxDelay, interpolationMethod,
          methodDelayPolicy, controlInputs, initialDelaysSeconds, initialGainsLinear );
        return inst;
      }),
      py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ),
      py::arg( "numberOfChannels" ),
      py::arg( "interpolationSteps" ) = 1024, py::arg( "maxDelay" ) = 3.0f,
      py::arg( "interpolationType" ) = "lagrangeOrder3",
      py::arg( "methodDelayPolicy" ) = DelayVector::MethodDelayPolicy::Add,
      py::arg( "controlInputs" ) = false,
      py::arg( "initialDelay" ) = pml::MatrixParameter<SampleType>(),
      py::arg( "initialGain" ) = pml::MatrixParameter<SampleType>() )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
