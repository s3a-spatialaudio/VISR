/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librcl/time_frequency_transform.hpp>

#include <libvisr/atomic_component.hpp>
#include <libvisr/composite_component.hpp>
#include <libvisr/signal_flow_context.hpp>

#include <libefl/basic_vector.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{
namespace python
{
namespace rcl
{

namespace py = pybind11;

void exportTimeFrequencyTransform( py::module & m )
{
  using visr::rcl::TimeFrequencyTransform;

  py::class_<TimeFrequencyTransform, visr::AtomicComponent>( m, "TimeFrequencyTransform" )
   .def( py::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*,
         std::size_t, std::size_t, std::size_t, std::size_t, char const *>(),
     py::arg("context"), py::arg("name"), py::arg("parent") = static_cast<visr::CompositeComponent*>(nullptr),
     py::arg( "numberOfChannels" ), py::arg( "dftLength" ), py::arg( "windowLength" ), py::arg( "hopSize" ),
     py::arg( "fftImplementation" ) = "default" )
   .def( py::init<visr::SignalFlowContext const&, char const *, visr::CompositeComponent*,
     std::size_t, std::size_t, efl::BasicVector<visr::rcl::TimeFrequencyTransform::SampleType> const &, std::size_t, char const *>(),
     py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ) = static_cast<visr::CompositeComponent*>(nullptr),
     py::arg( "numberOfChannels" ), py::arg( "dftLength" ), py::arg( "window" ), py::arg( "hopSize" ), py::arg( "fftImplementation" ) = "default" )

   .def( py::init( []( SignalFlowContext const & context,
          char const * name, CompositeComponent * parent, std::size_t numberOfChannels, std::size_t dftLength,
         std::vector<TimeFrequencyTransform::SampleType> const & window, std::size_t hopSize, char const * fftImplementation )
     {
       efl::BasicVector<TimeFrequencyTransform::SampleType> windowVec( window.size(), cVectorAlignmentSamples );
       if( not window.empty() )
       {
         std::copy(window.begin(), window.end(), windowVec.data() );
       }
       return new TimeFrequencyTransform( context, name, parent, numberOfChannels, dftLength,
                                          windowVec, hopSize,fftImplementation );
    }), py::arg( "context" ), py::arg( "name" ), py::arg( "parent" ) = static_cast<visr::CompositeComponent*>(nullptr),
      py::arg( "numberOfChannels" ), py::arg( "dftLength" ), py::arg( "window" ), py::arg( "hopSize" ),
      py::arg( "fftImplementation" ) = "default"
   )
  ;
}

} // namepace rcl
} // namespace python
} // namespace visr
