/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/delay_vector.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#ifdef USE_PYBIND11
#include <pybind11/pybind11.h>
#else
#include <boost/python.hpp>
#include <boost/python/args.hpp>

#include <boost/noncopyable.hpp>
#endif

namespace visr
{
namespace python
{
namespace rcl
{

#ifdef USE_PYBIND11
void exportDelayVector( pybind11::module & m )
{
  using visr::rcl::DelayVector;

  pybind11::class_<DelayVector, visr::AtomicComponent> dVec( m, "DelayVector" );

  pybind11::enum_<DelayVector::InterpolationType>( dVec, "InterpolationType" )
    .value( "NearestSample", DelayVector::InterpolationType::NearestSample)
    .value( "Linear", DelayVector::InterpolationType::Linear)
    .value( "CubicLagrange", DelayVector::InterpolationType::CubicLagrange )
    ;

  dVec.def( pybind11::init<visr::SignalFlowContext&, char const *, visr::CompositeComponent*>(),
      pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr) )
    .def( "setup", static_cast<void(DelayVector::*)( std::size_t, std::size_t, SampleType, 
                                                     DelayVector::InterpolationType, bool, SampleType, SampleType)>(&DelayVector::setup), pybind11::arg("numberOfChannels"),
      pybind11::arg( "interpolationSteps" ) = 1024, pybind11::arg( "maxDelay" ) = 3.0f,
      pybind11::arg( "interpolationType" ) = DelayVector::InterpolationType::CubicLagrange,
          pybind11::arg("controlInputs") = false,
          pybind11::arg( "initialDelay" ) = 0.0f,
      pybind11::arg( "initialGain" ) = 1.0f )
    // TO
    .def( "process", &DelayVector::process );
}

#endif

} // namepace rcl
} // namespace python
} // namespace visr
