/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include "audio_input.hpp"
#include "audio_output.hpp"
#include "communication_area.hpp"
#include "component.hpp"

#include <libefl/vector_functions.hpp>

#include <utility> // for std::pair and std::make_pair

namespace visr
{
namespace ril
{

AudioSignalFlow::AudioSignalFlow( std::size_t period, SamplingFrequencyType samplingFrequency )
 : mInitialised( false )
 , mPeriod( period )
 , mSamplingFrequency( samplingFrequency )
{
}

AudioSignalFlow::~AudioSignalFlow()
{
}

// todo: make this method protected?
void AudioSignalFlow::initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                                    std::size_t alignmentElements /* = cVectorAlignmentSamples */ )
{
  mCommArea.reset( new CommunicationArea<SampleType>( numberOfSignals, signalLength, alignmentElements ) );
}

/*static*/ void 
AudioSignalFlow::processFunction( void* userData,
                                  SampleType const * const * captureSamples,
                                  SampleType * const * playbackSamples,
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
  // TODO: It needs to be checked beforehand that the widths of the input and output signal vectors match.

  // fill the capture part of the communication area.
  std::size_t const numCaptureChannels = numberOfCaptureChannels();
  for( std::size_t captureIdx( 0 ); captureIdx < numCaptureChannels; ++captureIdx )
  {
    SampleType * const destPtr = mCommArea->at( mCaptureIndices[captureIdx] );
    // Note: We cannot assume an alignment as we don't know the alignment of the passed captureSamples.
    // TODO: Add optional argument to the AudioCallback interface to signal the alignment of the input and output samples.
    efl::ErrorCode const res = efl::vectorCopy( captureSamples[captureIdx], destPtr, mPeriod, 0 );
    if( res != efl::noError )
    {
      throw std::runtime_error( "AudioSignalFlow: Error while copying input samples samples." );
    }
  }

  // call the process() method of the derived class to perform the specific processing.
  // TODO: would this be the appropriate place to catch all exceptions called during the rendering?
  process();

  // collect the generated output from the playback part of the communication area and 
  // fill the playbackSamples accordingly.
  std::size_t const numPlaybackChannels = numberOfPlaybackChannels( );
  for( std::size_t playbackIdx( 0 ); playbackIdx < numPlaybackChannels; ++playbackIdx )
  {
    SampleType const * const srcPtr = mCommArea->at( mPlaybackIndices[playbackIdx] );
    // Note: We cannot assume an alignment as we don't know the alignment of the passed playbackSamples.
    // TODO: Add optional argument to the AudioCallback interface to signal the alignment of the input and output samples.
    efl::ErrorCode const res = efl::vectorCopy( srcPtr, playbackSamples[playbackIdx], mPeriod, 0 );
    if( res != efl::noError )
    {
      throw std::runtime_error( "AudioSignalFlow: Error while copying output samples samples." );
    }
  }

  // TODO: use a sophisticated enumeration to signal error conditions
  callbackResult = 0; // Means 'no error'
}

std::size_t AudioSignalFlow::numberOfCaptureChannels() const
{
  if( !initialised() )
  {
    throw std::logic_error( "AudioSignalFlow::captureWidth() must be called only after initalisation of the object is complete." );
  }
  return mCaptureIndices.size();
}

std::size_t AudioSignalFlow::numberOfPlaybackChannels() const
{
  if( !initialised() )
  {
    throw std::logic_error( "AudioSignalFLow::playbackWidth() must be called only after initalisation of the object is complete." );
  }
  return mPlaybackIndices.size();
}

void 
AudioSignalFlow::registerComponent( Component * component, char const * componentName )
{
  std::string const myName( componentName );
  std::pair<std::string, Component*> myPair( myName, component );
  std::pair<ComponentTable::iterator, bool> res = mComponents.insert( myPair );
  if( !res.second ) 
  {
    throw std::invalid_argument( "A component with the given name already exists." );
  }
}

void 
AudioSignalFlow::assignCommunicationIndices( std::string const & componentName,
                                             std::string const & portName,
                                             std::initializer_list<AudioPort::SignalIndexType> const & indexVector )
{
  AudioPort & port = findPort( componentName, portName ); // throws an exception if component or port does not exist.
  port.assignCommunicationIndices( indexVector.begin( ), indexVector.end( ) );
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
  Component * component = findIt->second;
  AudioInput * audioIn = component->getPort<AudioInput>(portName.c_str( ));
  if( audioIn )
  {
    return *audioIn;
  }
  AudioOutput * audioOut = component->getPort<AudioOutput>( portName.c_str( ) );
  if( audioOut ) 
  {
    return *audioOut;
  }
  throw std::invalid_argument( "Port with that name does not exist." );
}

void AudioSignalFlow::assignCaptureIndices( AudioPort::SignalIndexType const * indexArrayPtr, std::size_t vecLength )
{
  // todo: check allowed indices
  mCaptureIndices.assign( indexArrayPtr, indexArrayPtr + vecLength );
}

void AudioSignalFlow::assignPlaybackIndices( AudioPort::SignalIndexType const * indexArrayPtr, std::size_t vecLength )
{
  mPlaybackIndices.assign( indexArrayPtr, indexArrayPtr + vecLength );
}

void AudioSignalFlow::assignCaptureIndices( std::initializer_list<AudioPort::SignalIndexType> const & indexVector )
{
  mCaptureIndices.assign( indexVector );
}

void AudioSignalFlow::assignPlaybackIndices( std::initializer_list<AudioPort::SignalIndexType> const & indexVector )
{
  mPlaybackIndices.assign( indexVector );
}

} // namespace ril
} // namespace visr
