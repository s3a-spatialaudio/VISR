/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include <libril/audio_connection_descriptor.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/communication_area.hpp>
#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_factory.hpp>
#include <libril/communication_protocol_type.hpp>
#include <libril/parameter_port_base.hpp>

#include <libefl/vector_functions.hpp>

#include <ciso646>
#include <utility> // for std::pair and std::make_pair

namespace visr
{
namespace rrl
{

AudioSignalFlow::AudioSignalFlow( ril::Component & flow )
 : mFlow( flow )
 , mInitialised( false )
{
}

AudioSignalFlow::~AudioSignalFlow()
{
}

// todo: make this method protected?
void AudioSignalFlow::initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                                    std::size_t alignmentElements /* = cVectorAlignmentSamples */ )
{
  mCommArea.reset( new ril::CommunicationArea<ril::SampleType>( numberOfSignals, signalLength, alignmentElements ) );
}

/*static*/ void 
AudioSignalFlow::processFunction( void* userData,
                                  ril::SampleType const * const * captureSamples,
                                  ril::SampleType * const * playbackSamples,
                                  AudioInterface::CallbackResult& callbackResult )
{
  AudioSignalFlow* flowObj = reinterpret_cast<AudioSignalFlow*>( userData );
  flowObj->processInternal( captureSamples, playbackSamples, callbackResult );
}

void 
AudioSignalFlow::processInternal( ril::SampleType const * const * captureSamples,
                                  ril::SampleType * const * playbackSamples,
                                  AudioInterface::CallbackResult& callbackResult )
{
  // TODO: It needs to be checked beforehand that the widths of the input and output signal vectors match.

  // fill the capture part of the communication area.
  std::size_t const numCaptureChannels = numberOfCaptureChannels();
  for( std::size_t captureIdx( 0 ); captureIdx < numCaptureChannels; ++captureIdx )
  {
    ril::SampleType * const destPtr = mCommArea->at( mCaptureIndices[captureIdx] );
    // Note: We cannot assume an alignment as we don't know the alignment of the passed captureSamples.
    // TODO: Add optional argument to the AudioCallback interface to signal the alignment of the input and output samples.
    efl::ErrorCode const res = efl::vectorCopy( captureSamples[captureIdx], destPtr, mFlow.period(), 0 );
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
    ril::SampleType const * const srcPtr = mCommArea->at( mPlaybackIndices[playbackIdx] );
    // Note: We cannot assume an alignment as we don't know the alignment of the passed playbackSamples.
    // TODO: Add optional argument to the AudioCallback interface to signal the alignment of the input and output samples.
    efl::ErrorCode const res = efl::vectorCopy( srcPtr, playbackSamples[playbackIdx], mFlow.period(), 0 );
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

bool AudioSignalFlow::initialise( std::ostream & messages )
{
  if( not mFlow.isTopLevel() )
  { 
    messages << "the component is not a top-level element of a flow.";
    return false;
  }
  if( not mFlow.isComposite() )
  {
    ril::AtomicComponent& atom = static_cast<ril::AtomicComponent&>(mFlow);
    mProcessingSchedule.push_back( std::move( &atom ) );
  }
  else
  {
    ril::CompositeComponent & topLevel = dynamic_cast<ril::CompositeComponent&>(mFlow);
    // for( ril::CompositeComponent::Co)


  }
  return true;
}

bool AudioSignalFlow::initialiseAudioConnections( std::ostream & messages )
{
  return true;
}


void AudioSignalFlow::initialiseParameterInfrastructure()
{
  mCommunicationProtocols.clear();

#if 0
  // TODO: This should check whether the contained component is composite, and in this case iterate over the parameter connection table
  for( ril::ParameterConnectionTable::value_type const connectionDescriptor : mParameterConnectionTable )
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
#endif

#if 0
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
#endif
}

std::size_t AudioSignalFlow::numberCommunicationProtocols() const
{
  return mCommunicationProtocols.size();
}


bool AudioSignalFlow::checkFlow( ril::Component const & comp, bool locally, std::ostream & messages )
{
  if( not comp.isComposite() )
  {
    return true;
  }
  ril::CompositeComponent const & composite = dynamic_cast<ril::CompositeComponent const &>( comp );
  // First, check the connections level by level
  if( locally )
  {
    bool const localResult = checkCompositeLocal( composite, messages );
    bool overallResult = localResult;
    for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin( ) );
      compIt != composite.componentEnd( ); ++compIt )
    {
      bool const compRes = checkFlow( *(compIt->second), locally, messages );
      overallResult = overallResult and compRes;
    }
    return overallResult;
  }
  else
  {
    // No idea how to check hierarchically without flattening the graph.
    // Maybe we remove that option.
    return true;
  }
}

bool AudioSignalFlow::checkCompositeLocal( ril::CompositeComponent const & composite, std::ostream & messages )
{
  bool const audioCheck = checkCompositeLocalAudio( composite, messages );
  bool const paramCheck = checkCompositeLocalParameters( composite, messages );
  return audioCheck and paramCheck;
}

namespace // unnamed
{

struct AudioSignalDescriptor
{
public:
  using SignalIndexType = std::size_t;

  AudioSignalDescriptor()
    : mPort( nullptr )
    , mIndex( cInvalidIndex )
  {
  }

  explicit AudioSignalDescriptor( ril::AudioPort const * port, SignalIndexType index )
    : mPort( nullptr )
    , mIndex()
  {
  }

  /**
   * 'less than operator', used for ordering in a map.
   */
  bool operator<(AudioSignalDescriptor const & rhs)
  {
    if( mPort < rhs.mPort )
    {
      return true;
    }
    else if( mPort == rhs.mPort )
    {
      return mIndex < rhs.mIndex;
    }
    return false;
  }

  ril::AudioPort const* mPort;
  SignalIndexType mIndex;

  // with proper C++11 support, this could be instantiated in place (using the constexpr mechanism)
  static SignalIndexType const cInvalidIndex;
};

AudioSignalDescriptor::SignalIndexType const AudioSignalDescriptor::
cInvalidIndex = std::numeric_limits<AudioSignalDescriptor::SignalIndexType>::max();

using SignalConnectionMap = std::multimap< AudioSignalDescriptor, AudioSignalDescriptor >;

}

bool AudioSignalFlow::checkCompositeLocalAudio( ril::CompositeComponent const & composite, std::ostream & messages )
{
  using PortTable = std::set<ril::AudioPort const*>;
  PortTable sendPorts;
  PortTable receivePorts;

  // First add the external ports of 'composite'. From the localviewpoint of this component, the directions are 
  // reversed, i.e. inputs are senders and outputs are receivers.
  for( ril::Component::AudioPortVector::const_iterator extPortIt = composite.audioPortBegin();
       extPortIt != composite.audioPortEnd(); ++extPortIt )
  {
    if( extPortIt->mPort->direction() == ril::AudioPort::Direction::Input )
    {
      sendPorts.insert( extPortIt->mPort );
    }
    else
    {
      receivePorts.insert( extPortIt->mPort );
    }
  }
  // Add the ports of the contained components (without descending into the hierarchy)
  for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin();
       compIt != composite.componentEnd(); ++compIt )
  {
    ril::Component const & containedComponent = *(compIt->second);
    for( ril::Component::AudioPortVector::const_iterator intPortIt = containedComponent.audioPortBegin( );
      intPortIt != containedComponent.audioPortEnd( ); ++intPortIt )
    {
      if( intPortIt->mPort->direction( ) == ril::AudioPort::Direction::Input )
      {
        sendPorts.insert( intPortIt->mPort );
      }
      else
      {
        receivePorts.insert( intPortIt->mPort );
      }
    }
  }
  // Now populate the connections map
  for( ril::CompositeComponent::audioConnectionBegin() )

  return true;
}

bool AudioSignalFlow::checkCompositeLocalParameters( ril::CompositeComponent const & composite, std::ostream & messages )
{
  return true;
}


} // namespace rrl
} // namespace visr
