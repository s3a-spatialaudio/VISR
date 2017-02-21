/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librrl/audio_signal_flow.hpp> 

#include <libril/component.hpp>

#ifdef USE_PYBIND11
#include <pybind11.h>
#else
#include <boost/python.hpp>
#include <boost/python/args.hpp>
#include <boost/noncopyable.hpp>
#endif


namespace visr
{

using rrl::AudioSignalFlow;

namespace python
{
namespace rrl
{

#ifdef USE_PYBIND11

void exportAudioSignalFlow( pybind11::module & m )
{
  pybind11::class_<AudioSignalFlow>( m, "AudioSignalFlow" )
   .def( pybind11::init<visr::ril::Component&>() )
   .def_property_readonly( "initialised", &AudioSignalFlow::initialised )
   .def_property_readonly( "numberOfAudioCapturePorts", &AudioSignalFlow::numberOfAudioCapturePorts )
   .def_property_readonly( "numberOfAudioPlaybackPorts", &AudioSignalFlow::numberOfAudioPlaybackPorts )
   .def_property_readonly( "numberOfCaptureChannels", &AudioSignalFlow::numberOfCaptureChannels )
   .def_property_readonly( "numberOfPlaybackChannels", &AudioSignalFlow::numberOfPlaybackChannels )
    .def( "audioCapturePortName", &AudioSignalFlow::audioCapturePortName, pybind11::arg("index"), pybind11::return_value_policy::reference )
    .def( "audioPlaybackPortName", &AudioSignalFlow::audioPlaybackPortName, pybind11::arg( "index" ), pybind11::return_value_policy::reference )
  ;
}


#else

using namespace boost::python;


void exportAudioSignalFlow()
{
  class_<AudioSignalFlow, boost::noncopyable>( "AudioSignalFlow", init<visr::ril::Component&>() )
    .add_property( "initialised", &AudioSignalFlow::initialised )
    .add_property( "numberOfAudioCapturePorts", &AudioSignalFlow::numberOfAudioCapturePorts )
    .add_property( "numberOfAudioPlaybackPorts", &AudioSignalFlow::numberOfAudioPlaybackPorts )
    .add_property( "numberOfCaptureChannels", &AudioSignalFlow::numberOfCaptureChannels )
    .add_property( "numberOfPlaybackChannels", &AudioSignalFlow::numberOfPlaybackChannels )
    .def( "audioCapturePortName", &AudioSignalFlow::audioCapturePortName, (arg("index")), return_internal_reference<>() )
    .def( "audioPlaybackPortName", &AudioSignalFlow::audioPlaybackPortName, (arg( "index" )), return_internal_reference<>() )
    ;
}

#endif

} // namespace rrl
} // namespace python
} // namespace visr
