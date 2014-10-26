/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include "audio_input.hpp"
#include "audio_output.hpp"
#include "audio_component.hpp"
#include "communication_area.hpp"

#include <utility> // for std::pair and sttd::make_pair

namespace visr
{
namespace ril
{

AudioSignalFlow::AudioSignalFlow()
: mInitialised( false )
{
}

AudioSignalFlow::~AudioSignalFlow()
{
}

void AudioSignalFlow::setPeriod( std::size_t periodLength )
{
  if( initialised() )
  {
    throw std::logic_error( "AudioSignalFlow::setPeriod() must be called only during initialisation." );
  }
  mPeriod = periodLength;
}

// todo: make this method protected?
void AudioSignalFlow::initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                                    std::size_t alignmentElements /* = 0 */ )
{
  mCommArea.reset( new CommunicationArea<SampleType>( numberOfSignals, signalLength, alignmentElements ) );
}

/*static*/ void 
AudioSignalFlow::processFunction( void* userData,
                                  AudioInterface::ExternalSampleType const * const * captureSamples,
                                  AudioInterface::ExternalSampleType * const * playbackSamples,
                                  AudioInterface::CallbackResult& callbackResult )
{
  AudioSignalFlow* flowObj = reinterpret_cast<AudioSignalFlow*>( userData );
  flowObj->processInternal( captureSamples, playbackSamples, callbackResult );
}

void 
AudioSignalFlow::processInternal( SampleType const * const * captureSamples,
                                  SampleType * const * playbackSamples,
                                  AudioInterface::CallbackResult& callbackResult )
{
  // fill the capture part of the communication area.

  // call the process() method of the derived class to perform the specific processing.
  process();

  // collect the generated output from the playback part of the communication area and 
  // fill the playbackSamples accordingly.
}

void 
AudioSignalFlow::registerComponent( AudioComponent * component, char const * componentName )
{
  std::string const myName( componentName );
  std::pair<std::string, AudioComponent*> myPair( myName, component );
  std::pair<ComponentTable::iterator, bool> res = mComponents.insert( myPair );
  if( !res.second ) 
  {
    throw std::invalid_argument( "A component with the given name already exists." );
  }
}

AudioPort & 
AudioSignalFlow::findPort( std::string const & componentName,
                           std::string const & portName )
{
  ComponentTable::iterator findIt = mComponents.find( componentName );
  if( findIt == mComponents.end() )
  {
    throw std::invalid_argument( "No components with the given name exists." );
  }
  AudioComponent * component = findIt->second;
  AudioInput * audioIn = component->getPort<AudioInput>(portName.c_str( ));
  if( audioIn )
  {
    return *audioIn;
  }
  AudioOutput * audioOut = component->getPort<AudioOutput>( portName.c_str( ) );
  if( audioOut ) {
    return *audioOut;
  }
  throw std::invalid_argument( "Port with that name does not exist." );
}

} // namespace ril
} // namespace visr
