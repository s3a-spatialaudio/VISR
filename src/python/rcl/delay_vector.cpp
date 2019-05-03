/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/delay_vector.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libpml/vector_parameter.hpp>

#include <python/libpythonbindinghelpers/vector_from_ndarray.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/numpy.h>

#include <algorithm>
#include <ciso646>
#include <vector>

namespace visr
{
namespace python
{
namespace rcl
{

namespace py = pybind11;

namespace // unnamed
{
/**
 * Helper function to create a BasicVector from a std::vector.
 * This enables passing Python lists or vectors to VISR bindings.
 * @todo Consider making this a library function.
 */
template< typename ElementType >
void fillBasicVector( efl::BasicVector<ElementType> & ret, std::vector<ElementType> const & v )
{
  ret.resize( v.size() );
  if( not v.empty() )
  {
    std::copy( v.begin(), v.end(), ret.data() );
  }
}

} // unnamed namespace

void exportDelayVector( py::module & m )
{ 
  using visr::rcl::DelayVector;

  py::class_<DelayVector, visr::AtomicComponent> dVec( m, "DelayVector" );

  // Note: We rely that rbbl::MultichannelDelayLine::MethodDelayPolicy (the type behind the alias DelayVector::MethodDelayPolicy)
  // is already bound in python/rcl/delay_matrix.cpp (this call has to happen before exportDelayVector)
  // TODO: Consider creating a Python module for librbbl, bind rbbl::MultichannelDelayLine::MethodDelayPolicy there and add
  // it to the import dependencies of the rcl module

  pybind11::enum_<DelayVector::ControlPortConfig>( dVec, "ControlPortConfig" )
    .value( "No", DelayVector::ControlPortConfig::None ) // Cannot use 'None' because that is recognised as keyword in Python.
    .value( "Delay", DelayVector::ControlPortConfig::Delay )
    .value( "Gain", DelayVector::ControlPortConfig::Gain )
    .value( "All", DelayVector::ControlPortConfig::All )
    .def( pybind11::self | py::self )
    .def( pybind11::self & py::self )
    ;

  dVec.def( py::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*>(),
      py::arg("context"), py::arg("name"), py::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
    .def( "setup", static_cast<void(DelayVector::*)( std::size_t, std::size_t, SampleType, 
      const char *, DelayVector::MethodDelayPolicy, DelayVector::ControlPortConfig, SampleType, SampleType)>(&DelayVector::setup), py::arg("numberOfChannels"),
      py::arg( "interpolationSteps" ) = 1024, py::arg( "maxDelay" ) = 3.0f,
      py::arg( "interpolationType" ) /*= DelayVector::InterpolationType::CubicLagrange*/,
      py::arg( "methodDelayPolicy") = DelayVector::MethodDelayPolicy::Add,
      py::arg( "controlInputs") = DelayVector::ControlPortConfig::None,
      py::arg( "initialDelay" ) = 0.0f,
      py::arg( "initialGain" ) = 1.0f )
    .def( py::init(
      []( visr::SignalFlowContext const& context, char const * name , visr::CompositeComponent* parent,
          std::size_t numberOfChannels, std::size_t interpolationSteps, SampleType maxDelay, const char * interpolationMethod,
          DelayVector::MethodDelayPolicy methodDelayPolicy, DelayVector::ControlPortConfig controlInputs, SampleType initialDelay, SampleType initialGain)
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
      py::arg( "controlInputs" ) = DelayVector::ControlPortConfig::None,
      py::arg( "initialDelay" ) = 0.0f,
      py::arg( "initialGain" ) = 1.0f )
    .def( py::init(
      []( visr::SignalFlowContext const& context, char const * name, visr::CompositeComponent* parent,
        std::size_t numberOfChannels, std::size_t interpolationSteps, SampleType maxDelay, const char * interpolationMethod,
        DelayVector::MethodDelayPolicy methodDelayPolicy, DelayVector::ControlPortConfig controlInputs, efl::BasicVector< SampleType > const & initialDelaysSeconds,
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
      py::arg( "controlInputs" ) = DelayVector::ControlPortConfig::None,
      py::arg( "initialDelay" ) = pml::VectorParameter<SampleType>{}, // Use the pml type, because BasicVector does not provide a copy ctor
      py::arg( "initialGain" ) = pml::VectorParameter<SampleType>{}   // Use the pml type, because BasicVector does not provide a copy ctor
     )
     .def( py::init(
        []( visr::SignalFlowContext const& context, char const * name, visr::CompositeComponent* parent,
            std::size_t numberOfChannels, std::size_t interpolationSteps, SampleType maxDelay, const char * interpolationMethod,
            DelayVector::MethodDelayPolicy methodDelayPolicy, DelayVector::ControlPortConfig controlInputs, py::array_t< SampleType > const & initialDelaysSeconds,
            py::array_t< SampleType > const & initialGainsLinear )
        {
          std::size_t const alignment = 0;
          efl::BasicVector<SampleType> delays{ bindinghelpers::vectorFromNdArray<SampleType>(initialDelaysSeconds, alignment ) };
          efl::BasicVector<SampleType> gains{ bindinghelpers::vectorFromNdArray<SampleType>(initialGainsLinear, alignment ) };

          DelayVector * inst = new DelayVector( context, name, parent );
          inst->setup( numberOfChannels, interpolationSteps, maxDelay, interpolationMethod,
          methodDelayPolicy, controlInputs, delays, gains );
          return inst;
        } ),
          py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ),
        py::arg( "numberOfChannels" ),
        py::arg( "interpolationSteps" ) = 1024, py::arg( "maxDelay" ) = 3.0f,
        py::arg( "interpolationType" ) = "lagrangeOrder3",
        py::arg( "methodDelayPolicy" ) = DelayVector::MethodDelayPolicy::Add,
        py::arg( "controlInputs" ) = DelayVector::ControlPortConfig::None,
        py::arg( "initialDelay" ),
        py::arg( "initialGain" ),
        "Constructor taking Python lists or NumPy arrays as initial gain and delay values."
        )
     .def( py::init(
        []( visr::SignalFlowContext const& context, char const * name, visr::CompositeComponent* parent,
            std::size_t numberOfChannels, std::size_t interpolationSteps, SampleType maxDelay, const char * interpolationMethod,
            DelayVector::MethodDelayPolicy methodDelayPolicy, DelayVector::ControlPortConfig controlInputs, std::vector< SampleType > const & initialDelaysSeconds,
            std::vector< SampleType > const & initialGainsLinear )
        {
          efl::BasicVector<SampleType> delays;
          efl::BasicVector<SampleType> gains;
          fillBasicVector<SampleType>( delays, initialDelaysSeconds );
          fillBasicVector<SampleType>( gains, initialGainsLinear );

          DelayVector * inst = new DelayVector( context, name, parent );
          inst->setup( numberOfChannels, interpolationSteps, maxDelay, interpolationMethod,
          methodDelayPolicy, controlInputs, delays, gains );
          return inst;
        } ),
          py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ),
        py::arg( "numberOfChannels" ),
        py::arg( "interpolationSteps" ) = 1024, py::arg( "maxDelay" ) = 3.0f,
        py::arg( "interpolationType" ) = "lagrangeOrder3",
        py::arg( "methodDelayPolicy" ) = DelayVector::MethodDelayPolicy::Add,
        py::arg( "controlInputs" ) = DelayVector::ControlPortConfig::None,
        py::arg( "initialDelay" ),
        py::arg( "initialGain" ),
        "Constructor taking Python lists or NumPy arrays as initial gain and delay values."
        )
    ;
}

} // namepace rcl
} // namespace python
} // namespace visr
