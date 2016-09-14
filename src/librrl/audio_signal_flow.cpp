/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include "audio_connection_map.hpp"
#include "communication_area.hpp"
#include "scheduling_graph.hpp"

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
#include <functional>
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
  try
  {
    executeComponents();
  }
  catch( std::exception const & ex )
  {
    // TODO: Add error message to ex.what()
    throw ex;
  }

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

void AudioSignalFlow::executeComponents()
{
  for( ril::ProcessableInterface * pc : mProcessingSchedule )
  {
    try
    {
      pc->process();
    }
    catch( std::exception const & ex )
    {
      // TODO: Retrieve information about current object and add it to the exception message
      // TODO: Consider adding a name() function to ril::ProcessableInterface
      throw std::runtime_error( std::string("Exception while executing audio signal flow: ") + ex.what() );
    }
  }
}

std::size_t AudioSignalFlow::numberOfAudioCapturePorts() const
{
  return mTopLevelAudioInputs.size();
}

std::size_t AudioSignalFlow::numberOfAudioPlaybackPorts() const
{
  return mTopLevelAudioOutputs.size( );
}

/**
* Return the name of the capture port indexed by \p idx
* @throw std::out_of_range If the \p idx exceeds the number of capture ports.
*/
std::string const & AudioSignalFlow::audioCapturePortName( std::size_t idx ) const
{
  if( idx >= numberOfAudioCapturePorts() )
  {
    throw(std::out_of_range("AudioSignalFlow::audioCapturePortName(): index exceeds number of ports"));
  }
  return mTopLevelAudioInputs.at( idx )->name();
}

/**
* Return the name of the playback port indexed by \p idx
* @throw std::out_of_range If the \p idx exceeds the number of playback ports.
*/
std::string const & AudioSignalFlow::audioPlaybackPortName( std::size_t idx ) const
{
  if( idx >= numberOfAudioPlaybackPorts( ) )
  {
    throw(std::out_of_range( "AudioSignalFlow::audioPlaybackPortName(): index exceeds number of ports" ));
  }
  return mTopLevelAudioOutputs.at( idx )->name( );
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


namespace // unnamed
{

// TODO: Maybe define 'qualified name' and 'full name' methods for all ports.
std::string name( ril::ParameterPortBase const & port )
{
  return port.parent().name() + ":" + port.name();
}

std::string fullName( ril::ParameterPortBase const & port )
{
  return port.parent().fullName() + ":" + port.name();
}

bool checkParameterCompatibility( ril::ParameterPortBase const & sendPort, ril::ParameterPortBase const & receivePort,
                         std::ostream & messages)
{
  bool result = true;
  // Check connection for protocol and type compatibility
  ril::CommunicationProtocolType const sendProtocolType = sendPort.protocolType();
  ril::CommunicationProtocolType const receiveProtocolType = receivePort.protocolType();
  if( sendProtocolType != receiveProtocolType )
  {
    result = false;
    messages << "AudioSignalFlow::initialiseParameterInfrastructure(): The communication protocols of the connected parameter ports \""
             << fullName( sendPort ) << "\" and \"" << fullName( receivePort ) << "\" do not match.\n";
  }
  ril::ParameterType const sendParameterType = sendPort.parameterType();
  ril::ParameterType const receiveParameterType = receivePort.parameterType();
  if( sendParameterType != receiveParameterType )
  {
    result = false;
    messages << "AudioSignalFlow::initialiseParameterInfrastructure(): The parameter types of the connected parameter ports \""
             << fullName( sendPort ) << "\" and \"" << fullName( receivePort ) << "\" do not match.\n";
  }
  ril::ParameterConfigBase const & sendParameterConfig = sendPort.parameterConfig();
  ril::ParameterConfigBase const & receiveParameterConfig = receivePort.parameterConfig();
  if( not sendParameterConfig.compare( receiveParameterConfig ) )
  {
    result = false;
    messages << "AudioSignalFlow::initialiseParameterInfrastructure(): The parameter configurations of the connected parameter ports \""
             << fullName( sendPort ) << "\" and \"" << fullName( receivePort ) << "\" are not compatible.\n";
  }
  return result;
}

} // unnamed namespace

void AudioSignalFlow::initialiseParameterInfrastructure()
{
  mCommunicationProtocols.clear();
  if( not mFlow.isComposite() )
  {
    // If the flow is not composite, we also have to initialise the top-level parameter ports.
    return;
  }

  // TODO: 
  // - Recursively iterate over all composite components
  // - Collect all parameter ports (concrete and placeholders) in containers.
  // - Collect all connections and store them in a lookup map.
  // - Infer all connections from the receive end to check whether 

#if 0
  // TODO: This should check whether the contained component is composite, and in this case iterate over the parameter connection table
  for( ril::ParameterConnectionTable::const_iterator portIt( mFlow.parameterC : mParameterConnectionTable )
  {
    ril::ParameterConnectionTable::value_type const connectionDescriptor : mParameterConnectionTable

    // Note: In case of hierarchical models, we would need to construct the full name here.
    ril::ComponentTable::iterator const sendComponentIt= mComponents.find( connectionDescriptor.first.component() );
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


    std::unique_ptr<ril::CommunicationProtocolBase> protocolInstance
      = ril::CommunicationProtocolFactory::create( sendProtocolType, sendParameterType, sendParameterConfig );
    if( not protocolInstance )
    {
      throw std::invalid_argument( std::string( "AudioSignalFlow::initialiseParameterInfrastructure(): Could not instantiate protocol object for parameter connection \"" )
        + connectionDescriptor.first.component() + ":" + connectionDescriptor.first.port() + "\" -> \""
        + connectionDescriptor.second.component() + ":" + connectionDescriptor.second.port() + "\"." );
    }

    sendPort->connectProtocol( protocolInstance.get() );
    receivePort->connectProtocol( protocolInstance.get() );

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
  try
  {
    AudioConnectionMap const connections( composite, false );
    for( ril::AudioPort const * receivePort : receivePorts )
    {
      std::size_t const numChannels = receivePort->width( );
      for( std::size_t channelIdx( 0 ); channelIdx < numChannels; ++channelIdx )
      {
        std::pair<AudioConnectionMap::const_iterator, AudioConnectionMap::const_iterator > findRange
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
  }
  catch( std::exception const & ex )
  {
    messages << ex.what();
    result = false;
  }

  // Now check that all receivers are uniquely connected
  return result;
}

bool AudioSignalFlow::checkCompositeLocalParameters( ril::CompositeComponent const & composite, std::ostream & messages )
{
  bool result = true; // Result variable, is set to false if an error occurs.
  using PortTable = std::set<ril::ParameterPortBase const*>;
  PortTable sendPorts;
  PortTable receivePorts;

  // First add the external ports of 'composite'. From the local viewpoint of this component, the directions are 
  // reversed, i.e. inputs are senders and outputs are receivers.
  for( ril::Component::ParameterPortContainer::const_iterator extPortIt = composite.parameterPortBegin();
    extPortIt != composite.parameterPortEnd(); ++extPortIt )
  {
    if( extPortIt->second->direction() == ril::ParameterPortBase::Direction::Input )
    {
      sendPorts.insert( extPortIt->second );
    }
    else
    {
      receivePorts.insert( extPortIt->second );
    }
  }
  // Add the ports of the contained components (without descending into the hierarchy)
  for( ril::CompositeComponent::ComponentTable::const_iterator compIt( composite.componentBegin() );
    compIt != composite.componentEnd(); ++compIt )
  {
    ril::Component const & containedComponent = *(compIt->second);
    for( ril::Component::ParameterPortContainer::const_iterator intPortIt = containedComponent.parameterPortBegin();
      intPortIt != containedComponent.parameterPortEnd(); ++intPortIt )
    {
      if( intPortIt->second->direction() == ril::ParameterPortBase::Direction::Input )
      {
        receivePorts.insert( intPortIt->second );
      }
      else
      {
        sendPorts.insert( intPortIt->second );
      }
    }
  }
#if 0
  // Now populate the connection map
  try
  {
    AudioConnectionMap const connections( composite, false );
    for( ril::AudioPort const * receivePort : receivePorts )
    {
      std::size_t const numChannels = receivePort->width();
      for( std::size_t channelIdx( 0 ); channelIdx < numChannels; ++channelIdx )
      {
        std::pair<AudioConnectionMap::const_iterator, AudioConnectionMap::const_iterator > findRange
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
  }
  catch( std::exception const & ex )
  {
    messages << ex.what();
    result = false;
  }
#endif
  return result;
}

namespace // unnamed
{

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
        // In the top-level component, an input port is both a concrete/placeholder input and an external capture port
        if( comp.isTopLevel( ) )
        {
          mExternalCapturePorts.insert( *extPortIt );
        }
        if( comp.isComposite( ) )
        {
          mPlaceholderReceivePorts.insert( *extPortIt );
        }
        else
        {
          mConcreteReceivePorts.insert( *extPortIt );
        }
      }
      else
      {
        // For the top-level component, an output port is both a concrete/placeholder output and an external playback port
        if( comp.isTopLevel( ) )
        {
          mExternalPlaybackPorts.insert( *extPortIt );
        }
        if( comp.isComposite( ) )
        {
          mPlaceholderSendPorts.insert( *extPortIt );
        }
        else
        {
          mConcreteSendPorts.insert( *extPortIt );
        }
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

void assignConsecutiveIndices( ril::AudioPort * port, ril::AudioPort::SignalIndexType & index, rrl::CommunicationArea<ril::SampleType> & commArea )
{
  std::size_t const portWidth = port->width( );
  std::vector<ril::AudioPort::SignalIndexType> indexVec( portWidth );
  ril::AudioPort::SignalIndexType idx = index;
  std::generate( indexVec.begin( ), indexVec.end( ), [&idx] { return idx++; } );
  port->setAudioBasePointer( commArea.data( ) );
  port->setAudioChannelStride( commArea.signalStride() );
  port->assignCommunicationIndices( indexVec.begin( ), indexVec.end( ) );
  index += portWidth;
}

} // namespace unnamed.

bool AudioSignalFlow::initialiseAudioConnections( std::ostream & messages )
{
  mTopLevelAudioInputs.clear();
  mTopLevelAudioOutputs.clear();
  mCaptureIndices.clear();
  mPlaybackIndices.clear();
  bool result = true; // Result variable, is set to false if an error occurs.
  PortLookup const portLookup( mFlow, true /*recursive*/ );

  // Compute the number of audio channels for the different categories that are kept in the communicatio area.
  //  std::size_t const numPlaybackChannels = countAudioChannels( portLookup.mExternalPlaybackPorts );
  std::size_t const numCaptureChannels = countAudioChannels( portLookup.mExternalCapturePorts );
  std::size_t const numConcreteOutputChannels = countAudioChannels( portLookup.mConcreteSendPorts );
  std::size_t const totalSignalChannels = numCaptureChannels + numConcreteOutputChannels;

  std::size_t const period = mFlow.period();

  mCommArea.reset( new CommunicationArea<ril::SampleType>( totalSignalChannels, period, ril::cVectorAlignmentSamples ) );

  // Assign consecutive indices to the ports that need physical communication vectors.
  // First we do that for the external capture ports, because this part is identical for atomic and component top-level signal flows.
  ril::AudioPort::SignalIndexType offset = 0;
  std::size_t const captureSignalOffset = offset;
  std::for_each( portLookup.mExternalCapturePorts.begin( ), portLookup.mExternalCapturePorts.end( ),
    std::bind( assignConsecutiveIndices, std::placeholders::_1, std::ref( offset ), std::ref( *mCommArea ) ) );
  // Initialise the 'concrete' internal receive ports.
  std::size_t const concreteOutputSignalOffset = offset;
  std::for_each( portLookup.mConcreteSendPorts.begin( ), portLookup.mConcreteSendPorts.end( ),
    std::bind( assignConsecutiveIndices, std::placeholders::_1, std::ref( offset ), std::ref( *mCommArea ) ) );
  if( offset != mCommArea->numberOfSignals( ) )
  {
    throw std::logic_error( "AudioSignalFlow::initialiseAudioConnections(): number of allocated " );
  }

  // Initialise the data structures for the external inputs (capture ports)
  mTopLevelAudioInputs.reserve( portLookup.mExternalCapturePorts.size( ) );
  mCaptureIndices.resize( numCaptureChannels );
  std::size_t captureOffset = 0;
  for( ril::AudioPort * capturePort : portLookup.mExternalCapturePorts )
  {
    mTopLevelAudioInputs.push_back( capturePort );
    std::copy( capturePort->indices( ), capturePort->indices( ) + capturePort->width( ), &mCaptureIndices[captureOffset] );
    captureOffset += capturePort->width( );
  }

  // Allocate space for the external playback ports and indices.
  // Filling is different whether the top-level flow is composite or atomic.
  mPlaybackIndices.resize( countAudioChannels( portLookup.mExternalPlaybackPorts ) );
  mTopLevelAudioOutputs.reserve( portLookup.mExternalPlaybackPorts.size( ) );
  std::size_t playbackIndexOffset = 0;

  if( mFlow.isComposite() ) // Internal interconnections are specific to composite top-level flows.
  {
    AudioConnectionMap allConnections;
    std::stringstream errMessages;
    if( not allConnections.fill( mFlow, errMessages, true/*recursive*/ ) )
    {
      throw std::logic_error( "Filling of connection map failed:" + errMessages.str() );
    }
    std::cout << "\n\n\nAll connections:\n" << allConnections << std::endl;

    // Create a new connection map that removes the intermediate placeholder ports.
    // Precondition: The connection map has been checked for consistency 
    // (i.e., each receive port is connected to exactly one sender.
    // @note we need to check for cycles in the graph that are created by placeholder ports.
    // Otherwise this could create infinite recursions in the placeholder resolution code.
    // TODO: Factor this into a free function of member of a connection map.

    AudioConnectionMap concreteConnections;
    try
    {
      concreteConnections.resolvePlaceholders( allConnections );
    }
    catch( std::exception const & ex )
    {
      errMessages << ex.what() << '\n';
      result = false;
    }

    std::cout << "\n\n\nConcrete connections:\n" << concreteConnections << std::endl;

    // Initialise the concrete, i.e., real receive ports
    // Note: this assumes that all send ports (concrete and external capture ports) have already been initialised
    for( ril::AudioPort * receivePort : portLookup.mConcreteReceivePorts )
    {
      std::size_t const portWidth = receivePort->width( );
      std::vector<ril::AudioPort::SignalIndexType> receiveIndices( portWidth );
      for( std::size_t chIdx( 0 ); chIdx < portWidth; ++chIdx )
      {
        // equal_range is used to detect multiple connections
        // Normally this should have been detected before in the checking phase.
        std::pair<AudioConnectionMap::const_iterator, AudioConnectionMap::const_iterator >
          findRange = concreteConnections.equal_range( AudioSignalDescriptor( receivePort, chIdx ) );
        if( findRange.first == concreteConnections.end( ) )
        {
          throw std::runtime_error( "Audio signal connections : Did not find connection entry for receive channel." );
        }
        if( std::distance( findRange.first, findRange.second ) != 1 )
        {
          throw std::runtime_error( "Audio signal connections : Multiple connections detected." );
        }

        receiveIndices[chIdx] = findRange.first->second.mPort->indices( )[findRange.first->second.mIndex];
      }
      receivePort->setAudioBasePointer( mCommArea->data( ) );
      receivePort->setAudioChannelStride( mCommArea->signalStride( ) );
      receivePort->assignCommunicationIndices( receiveIndices.begin( ), receiveIndices.end( ) );
    }

    // same for the external playback ports
    // Note: the ordering of the external ports is defined by the order they are arranged in this set.
    for( ril::AudioPort * playbackPort : portLookup.mExternalPlaybackPorts )
    {
      std::size_t const portWidth = playbackPort->width( );
      std::vector<ril::AudioPort::SignalIndexType> receiveIndices( portWidth );
      for( std::size_t chIdx( 0 ); chIdx < portWidth; ++chIdx )
      {
        // equal_range is used to detect multiple connections
        // Normally this should have been detected before in the checking phase.
        std::pair<AudioConnectionMap::const_iterator, AudioConnectionMap::const_iterator >
          findRange = concreteConnections.equal_range( AudioSignalDescriptor( playbackPort, chIdx ) );
        if( findRange.first == concreteConnections.end( ) )
        {
          throw std::runtime_error( "Audio signal connections : Did not find connection entry for receive channel." );
        }
        if( std::distance( findRange.first, findRange.second ) != 1 )
        {
          throw std::runtime_error( "Audio signal connections : Multiple connections detected." );
        }

        receiveIndices[chIdx] = findRange.first->second.mPort->indices( )[findRange.first->second.mIndex];
      }
      playbackPort->setAudioBasePointer( mCommArea->data( ) );
      playbackPort->setAudioChannelStride( mCommArea->signalStride( ) );
      playbackPort->assignCommunicationIndices( receiveIndices.begin( ), receiveIndices.end( ) );

      mTopLevelAudioOutputs.push_back( playbackPort );
      std::copy( receiveIndices.begin( ), receiveIndices.end( ), &mPlaybackIndices[playbackIndexOffset] );
      playbackIndexOffset += portWidth;
    }

    // Set up the scheduling graph
    // For the moment this graph takes cares only about the audio connections.
    SchedulingGraph depGraph;
    depGraph.initialise( mFlow, concreteConnections );
    auto const schedule = depGraph.sequentialSchedule();
    mProcessingSchedule.assign( schedule.begin(), schedule.end() );

  } // Additional handling for composite top-level flows finished
  else // The flow consists only of a single atomic component.
  {
    // We need a separate treatment for the external playback indices.
    // This are linked (channel-by-channel) to the respective 'physical' output port of the atomic component.
    for( ril::AudioPort * playbackPort : portLookup.mExternalPlaybackPorts )
    {
      mTopLevelAudioOutputs.push_back( playbackPort );
      std::copy( playbackPort->indices(), playbackPort->indices( )+playbackPort->width(), &mPlaybackIndices[playbackIndexOffset] );
      playbackIndexOffset += playbackPort->width();
    }

    ril::AtomicComponent * atom = static_cast<ril::AtomicComponent *>( &mFlow );
    mProcessingSchedule.clear();
    mProcessingSchedule.push_back( atom );
  } 

  return result;
}

} // namespace rrl
} // namespace visr
