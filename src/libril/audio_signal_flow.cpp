/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include "audio_input.hpp"
#include "audio_output.hpp"
#include "atomic_component.hpp"
#include "communication_area.hpp"
#include "communication_protocol_base.hpp"
#include "communication_protocol_factory.hpp"
#include "communication_protocol_type.hpp"
#include "parameter_port_base.hpp"

#include <libefl/vector_functions.hpp>

#include <ciso646>
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
AudioSignalFlow::registerComponent( AtomicComponent * component, char const * componentName )
{
  std::string const myName( componentName );
  std::pair<std::string, AtomicComponent*> myPair( myName, component );
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
#if 0
  AudioPort & port = findPort( componentName, portName ); // throws an exception if component or port does not exist.
  port.assignCommunicationIndices( indexVector.begin( ), indexVector.end( ) );
  port.setAudioBasePointer( mCommArea->data() );
#else
  assignCommunicationIndices( componentName, portName, indexVector.begin(), indexVector.end() );
#endif
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parameter infrastructure

AudioSignalFlow::ParameterPortDescriptor::
ParameterPortDescriptor(std::string const & pComponent, std::string const & pPort)
 : mComponent( pComponent)
 , mPort(pPort)
{
}

bool AudioSignalFlow::ParameterPortDescriptor::
operator<(ParameterPortDescriptor const & rhs) const
{
  if (component() < rhs.component())
  {
    return true;
  }
  else if( rhs.component() < component() )
  {
    return false;
  }
  else return port() < rhs.port();
}

// Not used in the current code.
#if 0
AudioSignalFlow::ParameterConnection::
ParameterConnection( ParameterPortDescriptor const & pSender,
                     ParameterPortDescriptor const & pReceiver)
 : mSender(pSender)
 , mReceiver(pReceiver)
{
}

AudioSignalFlow::ParameterConnection::
ParameterConnection( std::string const & pSendComponent,
                     std::string const & pSendPort,
                     std::string const & pReceiveComponent,
                     std::string const & pReceivePort)
 : ParameterConnection( ParameterPortDescriptor( pSendComponent, pSendPort ),
	                    ParameterPortDescriptor( pReceiveComponent, pReceivePort) )
{
}

bool AudioSignalFlow::ParameterConnection::operator<(ParameterConnection const & rhs) const
{
  if(sender() < rhs.sender() )
  {
	return true;
  }
  else if(rhs.sender() < sender() )
  {
	  return false;
  }
  return receiver() < rhs.receiver();
}
#endif

void AudioSignalFlow::
connectParameterPorts( std::string const & sendComponent,
                       std::string const & sendPort,
                       std::string const & receiveComponent,
                       std::string const & receivePort)
{
  ParameterPortDescriptor const sendDescriptor( sendComponent, sendPort );
  ParameterPortDescriptor const receiveDescriptor( receiveComponent, receivePort);
  mParameterConnectionTable.insert(std::make_pair(sendDescriptor, receiveDescriptor) );
}


void AudioSignalFlow::initialiseParameterInfrastructure()
{
  mCommunicationProtocols.clear();

  // TODO: This should check whether the contained component is composite, and in this case iterate over the parameter connection table
  for( ParameterConnectionTable::value_type const connectionDescriptor : mParameterConnectionTable )
  {
    // Note: In case of hierarchical models, we would need to construct the full name here.
    ComponentTable::iterator const sendComponentIt= mComponents.find( connectionDescriptor.first.component() );
    if( sendComponentIt == mComponents.end() )
    {
      throw std::logic_error( std::string("AudioSignalFlow::initialiseParameterInfrastructure(): The specified sender component \"")
        + connectionDescriptor.first.component() + "\" of a parameter connection does not exist." );
    }
    ParameterPortBase * sendPort = sendComponentIt->second->findParameterPort( connectionDescriptor.first.port() );
    if( not sendPort )
    {
      throw std::logic_error( std::string( "AudioSignalFlow::initialiseParameterInfrastructure(): The specified send parameter port \"" )
        + connectionDescriptor.first.component() + ":" + connectionDescriptor.first.port() + "\" does not exist." );
    }
    ComponentTable::iterator const receiveComponentIt = mComponents.find( connectionDescriptor.second.component() );
    if( receiveComponentIt == mComponents.end() )
    {
      throw std::logic_error( std::string( "AudioSignalFlow::initialiseParameterInfrastructure(): The specified receiveer component \"" )
        + connectionDescriptor.first.component() + "\" of a parameter connection does not exist." );
    }
    ParameterPortBase * receivePort = receiveComponentIt->second->findParameterPort( connectionDescriptor.second.port() );
    if( not receivePort )
    {
      throw std::logic_error( std::string( "AudioSignalFlow::initialiseParameterInfrastructure(): The specified receive parameter port \"" )
        + connectionDescriptor.first.component() + ":" + connectionDescriptor.first.port() + "\" does not exist." );
    }
    // Check connection for protocol and type compatibility
    ril::CommunicationProtocolType const sendProtocolType = sendPort->protocolType();
    ril::CommunicationProtocolType const receiveProtocolType = receivePort->protocolType();
    if( sendProtocolType != receiveProtocolType )
    {
      throw std::invalid_argument( std::string("AudioSignalFlow::initialiseParameterInfrastructure(): The communication protocols of the connected parameter ports \"")
        + connectionDescriptor.first.component() + ":" + connectionDescriptor.first.port() + "\" and \""
        + connectionDescriptor.second.component() + ":" + connectionDescriptor.second.port() + "\" do not match." );
    }
    ril::ParameterType const sendParameterType = sendPort->parameterType();
    ril::ParameterType const receiveParameterType = receivePort->parameterType();
    if( sendParameterType != receiveParameterType )
    {
      throw std::invalid_argument( std::string( "AudioSignalFlow::initialiseParameterInfrastructure(): The parameter types of the connected parameter ports \"" )
        + connectionDescriptor.first.component() + ":" + connectionDescriptor.first.port() + "\" and \""
        + connectionDescriptor.second.component() + ":" + connectionDescriptor.second.port() + "\" do not match." );
    }
    ril::ParameterConfigBase const & sendParameterConfig = sendPort->parameterConfig();
    ril::ParameterConfigBase const & receiveParameterConfig = receivePort->parameterConfig();
    if( not sendParameterConfig.compare( receiveParameterConfig ) )
    {
      throw std::invalid_argument( std::string( "AudioSignalFlow::initialiseParameterInfrastructure(): The parameter configurations of the connected parameter ports \"" )
        + connectionDescriptor.first.component() + ":" + connectionDescriptor.first.port() + "\" and \""
        + connectionDescriptor.second.component() + ":" + connectionDescriptor.second.port() + "\" are not compatible." );
    }
    std::unique_ptr<CommunicationProtocolBase> protocolInstance = CommunicationProtocolFactory::create( sendProtocolType, sendParameterType, sendParameterConfig );
    if( not protocolInstance )
    {
      throw std::invalid_argument( std::string( "AudioSignalFlow::initialiseParameterInfrastructure(): Could not instantiate protocol object for parameter connection \"" )
        + connectionDescriptor.first.component() + ":" + connectionDescriptor.first.port() + "\" -> \""
        + connectionDescriptor.second.component() + ":" + connectionDescriptor.second.port() + "\"." );
    }
    //sendPort->connectProtocol( protocolInstance.get() );
    //receivePort->connectProtocol( protocolInstance.get() );
    protocolInstance->connectInput( receivePort );
    protocolInstance->connectOutput( sendPort );

    // Add the newly created protocol object to the flow's container for such objects, thus passing responsibility for deleting this object at the end of its lifetime.
    mCommunicationProtocols.push_back( std::move(protocolInstance) );
  }

  // Check whether all parameter ports are connected.
  // TODO: Consider to move that into a separate method.
  for( ComponentTable::value_type & comp : mComponents )
  {
    for( Component::ParameterPortContainer::const_iterator portIt( comp.second->parameterPortBegin()); portIt != comp.second->parameterPortEnd(); ++portIt )
    {
      ParameterPortBase * port = portIt->second;
      // TODO: Add checks to retrieve the connected protocol.
      //if( not port->connected() )
      //{
      //}
    }
  }
}

std::size_t AudioSignalFlow::numberCommunicationProtocols() const
{
  return mCommunicationProtocols.size();
}

} // namespace ril
} // namespace visr
