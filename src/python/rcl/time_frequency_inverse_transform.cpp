/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/time_frequency_inverse_transform.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libefl/basic_vector.hpp>

#include <pybind11/pybind11.h>

namespace visr
{
namespace python
{
namespace rcl
{

void exportTimeFrequencyInverseTransform( pybind11::module & m )
{
  using visr::rcl::TimeFrequencyInverseTransform;

  pybind11::class_<TimeFrequencyInverseTransform, visr::AtomicComponent>( m, "TimeFrequencyInverseTransform" )
   .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*, std::size_t, std::size_t, std::size_t, char const*>(),
     pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
     pybind11::arg( "numberOfChannels" ), pybind11::arg( "dftLength" ), pybind11::arg( "hopSize" ), pybind11::arg( "fftImplementation" ) = "default" )
  ; 
}

} // namepace rcl
} // namespace python
} // namespace visr
