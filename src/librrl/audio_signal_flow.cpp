/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include "communication_area.hpp"

#include <libril/audio_connection_descriptor.hpp>
#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/atomic_component.hpp>
#include <libril/composite_component.hpp>
#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_factory.hpp>
#include <libril/communication_protocol_type.hpp>
#include <libril/parameter_port_base.hpp>

#include <algorithm>
#include <ciso646>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <utility> // for std::pair and std::make_pair

namespace visr
{
namespace rrl
{

AudioSignalFlow::AudioSignalFlow( ril::Component & flow )
 : mFlow( flow )
 , mInitialised( false )
{
  std::stringstream checkMessages;
  bool const checkResult = checkFlow( mFlow, true, checkMessages );
  if( not checkResult )
  {
    // TODO: Devise other ways to transport messages.
    std::cout << "Messages during flow checking: " << checkMessages.str() << std::endl;
    throw std::runtime_error( "Signal flow graph is inconsistent." );
  }
  checkMessages.clear();
  bool const initAudioResult = initialiseAudioConnections( checkMessages );
}

AudioSignalFlow::~AudioSignalFlow()
{
}

// todo: make this method protected?
void AudioSignalFlow::initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                                    std::size_t alignmentElements /* = cVectorAlignmentSamples */ )
{
  mCommArea.reset( new rrl::CommunicationArea<ril::SampleType>( numberOfSignals, signalLength, alignmentElements ) );
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
  // process();

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
    : mPort( port )
    , mIndex( index)
  {
  }

  /**
   * 'less than operator', used for ordering in a map.
   */
  bool operator<(AudioSignalDescriptor const & rhs) const
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

  bool operator==(AudioSignalDescriptor const & rhs) const
  {
    return ( mPort == rhs.mPort ) and mIndex == rhs.mIndex;
  }

  ril::AudioPort const* mPort;
  SignalIndexType mIndex;

  // with proper C++11 support, this could be instantiated in place (using the constexpr mechanism)
  static SignalIndexType const cInvalidIndex;
};

struct CompareDescriptors
{
  bool operator()( AudioSignalDescriptor const & lhs, AudioSignalDescriptor const & rhs ) const
  {
    if( lhs.mPort < rhs.mPort )
    {
      return true;
    }
    else if( lhs.mPort == rhs.mPort )
    {
      return lhs.mIndex < rhs.mIndex;
    }
    return false;
  }
};

AudioSignalDescriptor::SignalIndexType const AudioSignalDescriptor::
cInvalidIndex = std::numeric_limits<AudioSignalDescriptor::SignalIndexType>::max();

/**
 * Format a port name
 */
std::string portWithComponentName( ril::AudioPort const * port )
{
  return port->parent( ).name( ) + "." + port->name( );
}

std::string printAudioSignalDescriptor( AudioSignalDescriptor const & desc )
{
  std::stringstream str;
  str << desc.mPort->parent( ).name( ) << "." << desc.mPort->name( ) << ":" << desc.mIndex;
  return str.str( );
}

using SignalConnectionMap = std::multimap< AudioSignalDescriptor, AudioSignalDescriptor, CompareDescriptors >;

/**
 * 
 */
bool fillConnectionMap( ril::Component const & component, SignalConnectionMap & connections, std::ostream & messages,
                        bool recursive = false )
{
  bool result = true; // Result variable, is set to false if an error occurs.
  using PortTable = std::set<ril::AudioPort const*>;
  PortTable sendPorts;
  PortTable receivePorts;

  // No connections possible.
  if( not component.isComposite( ) )
  {
    return true;
  }
  ril::CompositeComponent const & composite = dynamic_cast<ril::CompositeComponent const &>(component);
  // this could be moved to the PortLookup functionality.

  // First add the external ports of 'composite'. From the local viewpoint of this component, the directions are 
  // reversed, i.e. inputs are senders and outputs are receivers.
  for( ril::Component::AudioPortVector::const_iterator extPortIt = composite.audioPortBegin( );
    extPortIt != composite.audioPortEnd( ); ++extPortIt )
  {
    if( (*extPortIt)->direction( ) == ril::AudioPort::Direction::Input )
    {
      sendPorts.insert( *extPortIt );
    }
    else
    {
      receivePorts.insert( *extPortIt );
    }
  }
  // Add the ports of the contained components (without descending into the hierarchy)
  for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin( ) );
    compIt != composite.componentEnd( ); ++compIt )
  {
    ril::Component const & containedComponent = *(compIt->second);
    for( ril::Component::AudioPortVector::const_iterator intPortIt = containedComponent.audioPortBegin( );
      intPortIt != containedComponent.audioPortEnd( ); ++intPortIt )
    {
      if( (*intPortIt)->direction( ) == ril::AudioPort::Direction::Input )
      {
        receivePorts.insert( *intPortIt );
      }
      else
      {
        sendPorts.insert( *intPortIt );
      }
    }
  }
  for( ril::AudioConnectionTable::const_iterator connIt = composite.audioConnectionBegin( );
    connIt != composite.audioConnectionEnd( ); ++connIt )
  {
    ril::AudioConnection const connection = *connIt;
    if( sendPorts.find( connection.sender( ) ) == sendPorts.end( ) )
    {
      messages << "Audio signal flow connection check: In component \"" << composite.fullName( ) << "\", the send port \""
        << portWithComponentName( connection.sender( ) ) << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    if( receivePorts.find( connection.receiver( ) ) == receivePorts.end( ) )
    {
      // Todo: define flexible formatting of port names
      messages << "Audio signal flow connection check: In component \"" << composite.fullName( ) << "\", the receive port \""
        << portWithComponentName( connection.receiver( ) ) << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    ril::AudioChannelIndexVector const & sendIndices = connection.sendIndices( );
    ril::AudioChannelIndexVector const & receiveIndices = connection.receiveIndices( );
    if( receiveIndices.size( ) != sendIndices.size( ) )
    {
      messages << "Audio signal flow connection check: The channel index vectors of the connection \""
        << portWithComponentName( connection.sender( ) ) << "->" << portWithComponentName( connection.receiver( ) )
        << "are different." << std::endl;
      result = false;
      continue;
    }
    if( receiveIndices.size( ) > 0 ) // max_element cannot be dereferenced for empty sequences
    {
      ril::AudioChannelIndexVector::const_iterator maxSendIndex = std::max_element( sendIndices.begin( ), sendIndices.end( ) );
      ril::AudioChannelIndexVector::const_iterator maxReceiveIndex = std::max_element( receiveIndices.begin( ), receiveIndices.end( ) );
      if( *maxSendIndex >= connection.sender( )->width( ) )
      {
        messages << "Audio signal flow connection check: The send channel index of the connection \""
          << portWithComponentName( connection.sender( ) ) << "->" << portWithComponentName( connection.receiver( ) )
          << "\" exceeds the width of the send port." << std::endl;
        result = false;
        continue;
      }
      if( *maxReceiveIndex >= connection.receiver( )->width( ) )
      {
        messages << "Audio signal flow connection check: The receive channel index of the connection \""
          << portWithComponentName( connection.sender( ) ) << "->" << portWithComponentName( connection.receiver( ) )
          << "\" exceeds the width of the receive port." << std::endl;
        result = false;
        continue;
      }
      for( std::size_t runIdx( 0 ); runIdx < receiveIndices.size( ); ++runIdx )
      {
        // Table entries are sender, receiver
        connections.insert( std::make_pair( AudioSignalDescriptor( connection.receiver( ), receiveIndices[runIdx] ),
          AudioSignalDescriptor( connection.sender( ), sendIndices[runIdx] ) ) );
      }
    }
  }
  if( recursive )
  {
    for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin() );
         compIt != composite.componentEnd(); ++compIt )
    {
      result = result and fillConnectionMap( *(compIt->second), connections, messages, true );
    }
  }
}

} // namespace unnamed

bool AudioSignalFlow::checkCompositeLocalAudio( ril::CompositeComponent const & composite, std::ostream & messages )
{
  bool result = true; // Result variable, is set to false if an error occurs.
  using PortTable = std::set<ril::AudioPort const*>;
  PortTable sendPorts;
  PortTable receivePorts;

  // First add the external ports of 'composite'. From the local viewpoint of this component, the directions are 
  // reversed, i.e. inputs are senders and outputs are receivers.
  for( ril::Component::AudioPortVector::const_iterator extPortIt = composite.audioPortBegin();
       extPortIt != composite.audioPortEnd(); ++extPortIt )
  {
    if( (*extPortIt)->direction() == ril::AudioPort::Direction::Input )
    {
      sendPorts.insert( *extPortIt );
    }
    else
    {
      receivePorts.insert( *extPortIt );
    }
  }
  // Add the ports of the contained components (without descending into the hierarchy)
  for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin());
       compIt != composite.componentEnd(); ++compIt )
  {
    ril::Component const & containedComponent = *(compIt->second);
    for( ril::Component::AudioPortVector::const_iterator intPortIt = containedComponent.audioPortBegin( );
      intPortIt != containedComponent.audioPortEnd( ); ++intPortIt )
    {
      if( (*intPortIt)->direction( ) == ril::AudioPort::Direction::Input )
      {
        receivePorts.insert( *intPortIt );
      }
      else
      {
        sendPorts.insert( *intPortIt );
      }
    }
  }
  // Now populate the connection map
  SignalConnectionMap connections;
  for( ril::AudioConnectionTable::const_iterator connIt = composite.audioConnectionBegin();
       connIt != composite.audioConnectionEnd(); ++connIt )
  {
    ril::AudioConnection const connection = *connIt;
    if( sendPorts.find( connection.sender() ) == sendPorts.end() )
    {
      messages << "Audio signal flow connection check: In component \"" << composite.fullName() << "\", the send port \""
               << portWithComponentName( connection.sender() ) << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    if( receivePorts.find( connection.receiver() ) == receivePorts.end() )
    {
      // Todo: define flexible formatting of port names
      messages << "Audio signal flow connection check: In component \"" << composite.fullName() << "\", the receive port \""
        << portWithComponentName( connection.receiver() ) << "\" is not found." << std::endl;
      result = false;
      continue;
    }
    ril::AudioChannelIndexVector const & sendIndices = connection.sendIndices();
    ril::AudioChannelIndexVector const & receiveIndices = connection.receiveIndices();
    if( receiveIndices.size() != sendIndices.size() )
    {
      messages << "Audio signal flow connection check: The channel index vectors of the connection \""
        << portWithComponentName( connection.sender() ) << "->" << portWithComponentName( connection.receiver() )
        << "are different." << std::endl;
      result = false;
      continue;
    }
    if( receiveIndices.size() > 0 ) // max_element cannot be dereferenced for empty sequences
    {
      ril::AudioChannelIndexVector::const_iterator maxSendIndex = std::max_element( sendIndices.begin(), sendIndices.end() );
      ril::AudioChannelIndexVector::const_iterator maxReceiveIndex = std::max_element( receiveIndices.begin(), receiveIndices.end() );
      if( *maxSendIndex >= connection.sender()->width() )
      {
        messages << "Audio signal flow connection check: The send channel index of the connection \""
          << portWithComponentName( connection.sender() ) << "->" << portWithComponentName( connection.receiver() )
          << "\" exceeds the width of the send port." << std::endl;
        result = false;
        continue;
      }
      if( *maxReceiveIndex >= connection.receiver()->width() )
      {
        messages << "Audio signal flow connection check: The receive channel index of the connection \""
          << portWithComponentName( connection.sender() ) << "->" << portWithComponentName( connection.receiver() )
          << "\" exceeds the width of the receive port." << std::endl;
        result = false;
        continue;
      }
      for( std::size_t runIdx( 0 ); runIdx < receiveIndices.size(); ++runIdx )
      {
        // Table entries are sender, receiver
        connections.insert( std::make_pair( AudioSignalDescriptor( connection.receiver(), receiveIndices[runIdx] ),
                                            AudioSignalDescriptor( connection.sender(), sendIndices[runIdx] ) ) );
      }
    }
  }
  // Now check that all receivers are uniquely connected
  for( ril::AudioPort const * receivePort : receivePorts )
  {
    std::size_t const numChannels = receivePort->width();
    for( std::size_t channelIdx( 0 ); channelIdx < numChannels; ++channelIdx )
    {
      std::pair<SignalConnectionMap::const_iterator, SignalConnectionMap::const_iterator > findRange
        = connections.equal_range( AudioSignalDescriptor( receivePort, channelIdx ) );
      std::ptrdiff_t const numConnections = std::distance( findRange.first, findRange.second );
      assert( numConnections >= 0 );
      if( numConnections == 0 )
      {
        messages << "Audio signal flow connection check: The receive channel \"" << portWithComponentName( receivePort ) << ":" << channelIdx
                 << " is unconnected" << std::endl;
        result = false;
      }
      else if( numConnections > 1 )
      {
        messages << "Audio signal flow connection check: The receive channel \"" << portWithComponentName( receivePort ) << ":"
                 << channelIdx << " is connected to " << numConnections << " send channels: " << printAudioSignalDescriptor( findRange.first->second );
        ++findRange.first;
        for( ; findRange.first != findRange.second; ++findRange.first )
        {
          messages << ", " << printAudioSignalDescriptor( findRange.first->second );
        }
        messages << std::endl;
        result = false;
      }
    }
  }
  return result;
}

bool AudioSignalFlow::checkCompositeLocalParameters( ril::CompositeComponent const & composite, std::ostream & messages )
{
  // TODO: Implement me!
  return true;
}

namespace // unnamed
{

  /**
   * Utility function to check whether a port is considered as a placeholder or a concrete instance.
   * @todo consider moving to a graph checking and manipulation library to be defined.
   */
  bool isPlaceholder( ril::AudioPort const * port )
  {
    if( not port->parent().isComposite() )
    {
      return false;
    }
    if( port->parent().isTopLevel() )
    {
      // A toplevel port is not considered as a placeholder here
      // (It is either replaced by a real port or is handled in a special way.)
      return false;
    }
    return true;
  }

using PortTable = std::set<ril::AudioPort *>;

/**
 * Helper class to traverse through the hierarchical model to collect all ports.
 * The key apect is that this method can be called recursively.
 * @todo Maybe use the same class to collect other data (atomic ports, parameter ports)
 */
struct PortLookup
{
public:
  explicit PortLookup( ril::Component const & comp, bool recurse = true )
  {
    traverseComponent( comp, recurse );
  }

  void traverseComponent( ril::Component const & comp, bool recurse )
  {
    for( ril::Component::AudioPortVector::const_iterator extPortIt = comp.audioPortBegin();
      extPortIt != comp.audioPortEnd(); ++extPortIt )
    {
      if( (*extPortIt)->direction() == ril::AudioPort::Direction::Input )
      {
        comp.isTopLevel() ? mExternalCapturePorts.insert( *extPortIt )
          : comp.isComposite() ? mPlaceholderReceivePorts.insert( *extPortIt )
          : mConcreteReceivePorts.insert( *extPortIt );
      }
      else
      {
        comp.isTopLevel( ) ? mExternalPlaybackPorts.insert( *extPortIt )
          : comp.isComposite( ) ? mPlaceholderSendPorts.insert( *extPortIt )
          : mConcreteSendPorts.insert( *extPortIt );
      }
    }
    if( comp.isComposite() )
    {
      ril::CompositeComponent const & composite = dynamic_cast<ril::CompositeComponent const &>(comp);
      // Add the ports of the contained components (without descending into the hierarchy)
      for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin() );
           compIt != composite.componentEnd(); ++compIt )
      {
        traverseComponent( *(compIt->second), recurse );
      }
    }
  }

  PortTable mPlaceholderReceivePorts;
  PortTable mPlaceholderSendPorts;
  PortTable mConcreteSendPorts;
  PortTable mConcreteReceivePorts;
  PortTable mExternalCapturePorts;
  PortTable mExternalPlaybackPorts;
};

/**
 * Helper function to sum up the audio channels of a list of ports.
 */
std::size_t countAudioChannels( PortTable const & portList )
{
  return std::accumulate( portList.begin( ), portList.end( ), static_cast<std::size_t>(0) ,
    []( std::size_t acc, ril::AudioPort const * port ) { return acc + port->width( ); } );
}

}

bool AudioSignalFlow::initialiseAudioConnections( std::ostream & messages )
{
  mTopLevelAudioInputs.clear();
  mTopLevelAudioOutputs.clear();
  mCaptureIndices.clear();
  mPlaybackIndices.clear();

  bool result = true; // Result variable, is set to false if an error occurs.
  PortLookup const portLookup( mFlow, true /*recursive*/ );

  // Compute the number of audio channels for the different categories that are kept in the communicatio area.
  std::size_t const numConcreteOutputChannels = countAudioChannels( portLookup.mConcreteReceivePorts );
  std::size_t const numPlaybackChannels = countAudioChannels( portLookup.mExternalPlaybackPorts );
  std::size_t const numCaptureChannels = countAudioChannels( portLookup.mExternalCapturePorts );

  std::size_t const period = mFlow.period( );

  mCommArea.reset( new CommunicationArea<ril::SampleType>( numConcreteOutputChannels, period, ril::cVectorAlignmentSamples ) );

  SignalConnectionMap allConnections;
  std::stringstream errMessages;
  if( not fillConnectionMap( mFlow, allConnections, errMessages, true/*recursive*/ ) )
  {
    throw std::logic_error( "Filling of connection map failed:" + errMessages.str() );
  }

  std::size_t portOffset = 0;
  for( ril::AudioPort * port : portLookup.mConcreteReceivePorts )
  {
    std::size_t const portWidth = port->width();
  }

  return result;
}

} // namespace rrl
} // namespace visr
