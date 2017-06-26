/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libaudiointerfaces/audio_interface.hpp> 

#include <librrl/audio_signal_flow.hpp> 

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <vector>

namespace visr
{
namespace python
{
namespace audiointerfaces
{

namespace // unnamed
{

  class AudioInterfaceWrapper: public visr::audiointerfaces::AudioInterface
  {
  public:
    using AudioInterface::AudioInterface;

    void start() override
    {
      PYBIND11_OVERLOAD_PURE( void, AudioInterface, start, );
    }

    void stop() override
    {
      PYBIND11_OVERLOAD_PURE( void, AudioInterface, start, );
    }
  };

  void registerFlowCallback( visr::audiointerfaces::AudioInterface & interface, rrl::AudioSignalFlow & flow )
  {
    // TODO: We could do lots of error checking here (provided that the AudioInterface interface allows sufficient information 
    // to be extracted.
    interface.registerCallback( &rrl::AudioSignalFlow::processFunction, &flow );
  }

  void unregisterFlowCallback( visr::audiointerfaces::AudioInterface & interface )
  {
    interface.unregisterCallback( &rrl::AudioSignalFlow::processFunction );
  }

} // unnamed namespace
  
void exportAudioInterface( pybind11::module & m )
{
  /**
   * Python binding of the abstract interface AudioInterface.
   * Note that we do not provide the full interface, but allow only callbacks to AudioSignalFlow object.
   */
  pybind11::class_<visr::audiointerfaces::AudioInterface, AudioInterfaceWrapper> ai( m, "AudioInterface" );

  pybind11::class_<visr::audiointerfaces::AudioInterface::Configuration>( ai, "Configuration" )
    .def( pybind11::init<std::size_t, std::size_t, visr::audiointerfaces::AudioInterface::SampleRateType, std::size_t>(),
      pybind11::arg("numberOfCaptureChannels"),
      pybind11::arg( "numberOfPlayback" ),
      pybind11::arg( "sampleRate" ) = 0,
      pybind11::arg( "period" ) = 0 )
    .def_property_readonly( "numberOfCaptureChannels", &visr::audiointerfaces::AudioInterface::Configuration::numCaptureChannels )
    .def_property_readonly( "numberOfPlaybackChannels", &visr::audiointerfaces::AudioInterface::Configuration::numPlaybackChannels )
    .def_property_readonly( "sampleRate", &visr::audiointerfaces::AudioInterface::Configuration::sampleRate )
    .def_property_readonly( "period", &visr::audiointerfaces::AudioInterface::Configuration::periodSize )
  ;

  ai.def( "registerCallback", &registerFlowCallback, pybind11::arg("flow"))
    .def( "unregisterCallback", &unregisterFlowCallback )
    .def( "start", &visr::audiointerfaces::AudioInterface::start )
    .def( "stop", &visr::audiointerfaces::AudioInterface::stop )
    ;
}

} // namespace audiointerfaces
} // namespace python
} // namespace visr
