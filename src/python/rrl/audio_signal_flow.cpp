/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librrl/audio_signal_flow.hpp> 

#include <boost/python.hpp>
#include <boost/python/args.hpp>

#include <boost/noncopyable.hpp>

using namespace boost::python;

using visr::rrl::AudioSignalFlow;

BOOST_PYTHON_MODULE( rrl )
{

class_<AudioSignalFlow, boost::noncopyable>( "AudioSignalFlow", init<visr::ril::Component&>() )
  .add_property( "initialised", &AudioSignalFlow::initialised )
  .add_property( "numberOfAudioCapturePorts", &AudioSignalFlow::numberOfAudioCapturePorts )
  .add_property( "numberOfAudioPlaybackPorts", &AudioSignalFlow::numberOfAudioPlaybackPorts )
  ;
}
