/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librrl/audio_signal_flow.hpp> 

#include <boost/python.hpp>
#include <boost/python/args.hpp>

#include <boost/noncopyable.hpp>

using namespace boost::python;

namespace visr
{

using rrl::AudioSignalFlow;

namespace python
{
namespace rrl
{

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

} // namepace rrl
} // namespace python
} // namespace visr
