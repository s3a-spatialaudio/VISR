/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/time_frequency_transform.hpp>

#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/signal_flow_context.hpp>

#include <libefl/basic_vector.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{
namespace python
{
namespace rcl
{

void exportTimeFrequencyTransform( pybind11::module & m )
{
  using visr::rcl::TimeFrequencyTransform;

  pybind11::class_<TimeFrequencyTransform, visr::AtomicComponent>( m, "TimeFrequencyTransform" )
   .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*,
         std::size_t, std::size_t, std::size_t, std::size_t, char const *>(),
     pybind11::arg("context"), pybind11::arg("name"), pybind11::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
     pybind11::arg( "numberOfChannels" ), pybind11::arg( "dftLength" ), pybind11::arg( "windowLength" ), pybind11::arg( "hopSize" ),
     pybind11::arg( "fftImplementation" ) = "default" )
   .def( pybind11::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*,
     std::size_t, std::size_t, efl::BasicVector<visr::rcl::TimeFrequencyTransform::SampleType> const &, std::size_t, char const *>(),
     pybind11::arg( "context" ), pybind11::arg( "name" ), pybind11::arg( "parent" ) = static_cast<visr::CompositeComponent*>(nullptr),
     pybind11::arg( "numberOfChannels" ), pybind11::arg( "dftLength" ), pybind11::arg( "window" ), pybind11::arg( "hopSize" ), pybind11::arg( "fftImplementation" ) = "default" )

   .def( "__init__",
      []( TimeFrequencyTransform &instance, SignalFlowContext const & context,
          char const * name, CompositeComponent * parent, std::size_t numberOfChannels, std::size_t dftLength,
         std::vector<TimeFrequencyTransform::SampleType> const & window, std::size_t hopSize, char const * fftImplementation )
     {
       efl::BasicVector<TimeFrequencyTransform::SampleType> windowVec( window.size(), cVectorAlignmentSamples );
       if( not window.empty() )
       {
         std::copy(window.begin(), window.end(), windowVec.data() );
       }
       new (&instance) TimeFrequencyTransform( context, name, parent, numberOfChannels, dftLength,
                                               windowVec, hopSize,fftImplementation );
    }, pybind11::arg( "context" ), pybind11::arg( "name" ), pybind11::arg( "parent" ) = static_cast<visr::CompositeComponent*>(nullptr),
      pybind11::arg( "numberOfChannels" ), pybind11::arg( "dftLength" ), pybind11::arg( "window" ), pybind11::arg( "hopSize" ),
      pybind11::arg( "fftImplementation" ) = "default"
   )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
