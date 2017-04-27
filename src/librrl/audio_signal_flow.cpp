/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include "audio_connection_map.hpp"
#include "communication_area.hpp"
#include "integrity_checking.hpp"
#include "parameter_connection_graph.hpp"
#include "parameter_connection_map.hpp"
#include "port_utilities.hpp"
#include "scheduling_graph.hpp"

#include "signal_routing_internal.hpp"

#include <libefl/alignment.hpp>
#include <libefl/vector_functions.hpp>

#include <libril/atomic_component.hpp>
#include <libril/audio_sample_type.hpp>
#include <libril/parameter_input.hpp>
#include <libril/parameter_output.hpp>
#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_factory.hpp>
#include <libril/communication_protocol_type.hpp>
#include <libril/detail/compose_message_string.hpp>


#include <libvisr_impl/audio_connection_descriptor.hpp>
#include <libvisr_impl/audio_port_base_implementation.hpp>
#include <libvisr_impl/component_implementation.hpp>
#include <libvisr_impl/composite_component_implementation.hpp>
#include <libvisr_impl/parameter_port_base_implementation.hpp>

#include <algorithm>
#include <cassert>
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

AudioSignalFlow::AudioSignalFlow( Component & flow )
 : mFlow( flow.implementation() )
 , mInitialised( false )
{
  std::stringstream checkMessages;
  bool const checkResult = checkConnectionIntegrity( mFlow, true/* hierarchical*/, checkMessages );
  if( not checkResult )
  {
    // TODO: Devise other ways to transport messages.
    std::cout << "Messages during flow checking: " << checkMessages.str() << std::endl;
    throw std::runtime_error( "Signal flow graph is inconsistent." );
  }
  checkMessages.clear();

  // TODO: homogenise interfaces between parameter and audio connection maps.
  AudioConnectionMap const originalAudioConnections( mFlow, true );
  AudioConnectionMap const flatAudioConnections = originalAudioConnections.resolvePlaceholders();

  ParameterConnectionMap originalParameterConnections;
  if( not fillRecursive( originalParameterConnections, mFlow, checkMessages ) )
  {
    throw std::runtime_error(checkMessages.str() );
  }
  ParameterConnectionMap const flatParameterConnections = resolvePlaceholders( originalParameterConnections );

  AudioConnectionMap adjustedAudioConnections;
  bool const initAudioResult = initialiseAudioConnections( checkMessages, flatAudioConnections,
                                                           adjustedAudioConnections);
  if( not initAudioResult )
  {
    // TODO: Devise other ways to transport messages.
    std::cout << "Messages during audio initialisation: " << checkMessages.str() << std::endl;
    throw std::runtime_error( "AudioSignalFlow: Audio connections could not be initialised." );
  }


  ParameterConnectionMap adjustedParameterConnections;
  bool const initParamResult = initialiseParameterInfrastructure( checkMessages, flatParameterConnections,
                                                                  adjustedParameterConnections);
  if( not initParamResult )
  {
    std::cout << "Messages during parameter initialisation: " << checkMessages.str() << std::endl;
    throw std::runtime_error( "AudioSignalFlow: Parameter infrastructure could not be initialised." );
  }



  // TODO: Use the full list of components 
  bool const initScheduleResult = initialiseSchedule( checkMessages, adjustedAudioConnections, adjustedParameterConnections );
  if( not initScheduleResult )
  {
    std::cout << "Messages during schedule computation: " << checkMessages.str() << std::endl;
    throw std::runtime_error( "AudioSignalFlow: Execution schedule could not be created." );
  }
  mInitialised = true;
}

AudioSignalFlow::~AudioSignalFlow()
{
}

std::size_t AudioSignalFlow::period() const
{
  return mFlow.period();
}

/*static*/ void 
AudioSignalFlow::processFunction( void* userData,
                                  SampleType const * const * captureSamples,
                                  SampleType * const * playbackSamples,
                                  AudioInterface::CallbackResult& callbackResult )
{
  AudioSignalFlow* flowObj = reinterpret_cast<AudioSignalFlow*>( userData );
  callbackResult = flowObj->process( captureSamples, playbackSamples );
}


AudioInterface::CallbackResult
AudioSignalFlow::process( SampleType const * const * captureSamples,
                          SampleType * const * playbackSamples )
{
  // TODO: It needs to be checked beforehand that the widths of the input and output signal vectors match.
#if 1
  // This assumes that all capture ports have the default sample type "SampleType"
  for( std::size_t chIdx( 0 ); chIdx < numberOfCaptureChannels(); ++chIdx )
  {
    SampleType * chPtr = reinterpret_cast<SampleType*>(mCaptureChannels[chIdx]);
    efl::ErrorCode const res = efl::vectorCopy( captureSamples[chIdx], chPtr, mFlow.period(), 0 );
    if( res != efl::noError )
    {
      throw std::runtime_error( "AudioSignalFlow: Error while copying input samples samples." );
    }
  }
#else
  // TODO: Checking should be done during initialisation.
  if( mTopLevelAudioInputs.size() > 1 )
  {
    throw std::runtime_error( "AudioSignalFlow::process(): At the moment at most one top-level audio input is allowed." );
  }
  if( mTopLevelAudioOutputs.size() > 1 )
  {
    throw std::runtime_error( "AudioSignalFlow::process(): At the moment at most one top-level audio output is allowed." );
  }
  if( not mTopLevelAudioInputs.empty() )
  {
    impl::AudioPortBaseImplementation & input = *mTopLevelAudioInputs[0];
    if( input.sampleType() != AudioSampleType::TypeToId<SampleType>::id )
    {
      throw std::runtime_error( "AudioSignalFlow::process(): The sample type of the top-level audio input differs from the input sample type." );
    }
    std::size_t const width{ input.width() };
    SampleType * const basePointer{  static_cast<SampleType*>(input.basePointer()) };
    std::size_t const channelStride{ input.channelStrideSamples() };
    for( std::size_t chIdx(0); chIdx < width; ++chIdx )
    {
      SampleType * const chPtr = basePointer + chIdx * channelStride;
      efl::ErrorCode const res = efl::vectorCopy( captureSamples[chIdx], chPtr, mFlow.period(), 0 );
      if( res != efl::noError )
      {
        throw std::runtime_error( "AudioSignalFlow: Error while copying input samples samples." );
      }
    }
  }
#endif
  try
  {
    executeComponents();
  }
  catch( std::exception const & ex )
  {
    throw( std::string( "Error during execution of processing schedule: ") + ex.what() );
    return 1;
  }
#if 1
  // This assumes that all capture ports have the default sample type "SampleType"
  for( std::size_t chIdx( 0 ); chIdx < numberOfPlaybackChannels(); ++chIdx )
  {
    SampleType const * chPtr = reinterpret_cast<SampleType const*>(mPlaybackChannels[chIdx]);
    efl::ErrorCode const res = efl::vectorCopy( chPtr, playbackSamples[chIdx], mFlow.period(), 0 );
    if( res != efl::noError )
    {
      throw std::runtime_error( "AudioSignalFlow: Error while copying output samples samples." );
    }
  }
#else
  if( not mTopLevelAudioOutputs.empty() )
  {
    impl::AudioPortBaseImplementation const & output = *mTopLevelAudioOutputs[0];
    if( output.sampleType() != AudioSampleType::TypeToId<SampleType>::id )
    {
      throw std::runtime_error( "AudioSignalFlow::process(): The sample type of the top-level audio output differs from the input sample type." );
    }
    std::size_t const width{ output.width() };
    SampleType const * const basePointer{ static_cast<SampleType const* const>(output.basePointer()) };
    std::size_t const channelStride{ output.channelStrideSamples() };
    for( std::size_t chIdx( 0 ); chIdx < width; ++chIdx )
    {
      SampleType const * const chPtr = basePointer + chIdx * channelStride;
      efl::ErrorCode const res = efl::vectorCopy( chPtr, playbackSamples[chIdx], mFlow.period(), 0 );
      if( res != efl::noError )
      {
        throw std::runtime_error( "AudioSignalFlow: Error while copying output samples samples." );
      }
    }
  }
#endif
  // TODO: use a sophisticated enumeration to signal error conditions
  return 0; // Means 'no error'
}

void AudioSignalFlow::executeComponents()
{
  for( AtomicComponent * pc : mProcessingSchedule )
  {
    try
    {
      pc->process();
    }
    catch( std::exception const & ex )
    {
      // TODO: Retrieve information about current object and add it to the exception message
      // TODO: Consider adding a name() function to ProcessableInterface
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
char const * AudioSignalFlow::audioCapturePortName( std::size_t idx ) const
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
char const * AudioSignalFlow::audioPlaybackPortName( std::size_t idx ) const
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
  return mCaptureChannels.size();
}

std::size_t AudioSignalFlow::numberOfPlaybackChannels() const
{
  if( !initialised() )
  {
    throw std::logic_error( "AudioSignalFLow::playbackWidth() must be called only after initalisation of the object is complete." );
  }
  return mPlaybackChannels.size();
}

bool AudioSignalFlow::initialiseParameterInfrastructure( std::ostream & messages, ParameterConnectionMap const & originalConnections, ParameterConnectionMap & finalConnections )
{
  bool result = true;
  mCommunicationProtocols.clear();
  if( not mFlow.isComposite() )
  {
    // TODO: Maybe move to a specific method for atomic flows
    for( auto paramPort : mFlow.parameterPorts() )
    {
      visr::ParameterType const paramType = paramPort->parameterType();
      CommunicationProtocolType const protocol = paramPort->protocolType();
      ParameterConfigBase const & paramConfig = paramPort->parameterConfig();
      std::unique_ptr<CommunicationProtocolBase> protocolInstance
        = CommunicationProtocolFactory::createProtocol( protocol, paramType, paramConfig );
      if( not protocolInstance )
      {
        messages << "AudioSignalFlow: Could not instantiate communication protocol for parameter connection.\n";
        result = false;
      }
      if( paramPort->direction() == PortBase::Direction::Input )
      {
        std::unique_ptr<CommunicationProtocolBase::Output> protocolOutput = CommunicationProtocolFactory::createOutput( protocol );
	// TODO: Do we need to care for failing dynamic_casts (this would be a logical error).
        ParameterInputBase & paramInput = dynamic_cast<ParameterInputBase&>(paramPort->containingPort());
        protocolInstance->connectInput( &(paramInput.protocolInput()) );
        protocolInstance->connectOutput( protocolOutput.get() );
        ProtocolReceiveEndpoints::value_type insertPair{ std::string( paramPort->name() ), std::move( protocolOutput ) };
        mProtocolReceiveEndpoints.insert( std::move( insertPair ) );
      }
      else
      {
        std::unique_ptr<CommunicationProtocolBase::Input> protocolInput = CommunicationProtocolFactory::createInput( protocol );
        ParameterOutputBase & paramOutput = dynamic_cast<ParameterOutputBase&>(paramPort->containingPort());
        protocolInstance->connectOutput( &(paramOutput.protocolOutput()) );
        protocolInstance->connectInput( protocolInput.get() );
        ProtocolSendEndpoints::value_type insertPair{ std::string( paramPort->name() ), std::move( protocolInput ) };
        mProtocolSendEndpoints.insert( std::move( insertPair ) );
      }
      mCommunicationProtocols.push_back( std::move( protocolInstance ) );
    }
    return result;
  }
  // Composite component case starts here.
  ParameterConnectionGraph const connectionGraph( originalConnections );
  for( ParameterConnectionGraph::ConnectedPorts const & connection : connectionGraph.connectedPorts() )
  {
    // First check the connection topology.
    std::size_t const numSenders = connection.numSenders();
    std::size_t const numReceivers = connection.numReceivers();
    // This should not happen if the connections are created with registerParameterConnection()
    if( numSenders < 1 )
    {
      messages << "AudioSignalFlow: A parameter connection must have at least one sending port.\n";
      result = false;
    }
    if( numReceivers < 1 )
    {
      messages << "AudioSignalFlow: A parameter connection must have at least one sending port.\n";
      result = false;
    }

    if( numSenders < 1 )
    {
      continue;
    }

    // Use one port as reference, all other ports in this connection must have compatible porotocols and parameters.
    impl::ParameterPortBaseImplementation const * refPort = connection.sendPorts()[0];
    // We checked before that connection.sendPorts() is not empty.
    // Don't check against the first port which is the reference.
    for( auto portIt( ++connection.sendPorts().begin() ); portIt < connection.sendPorts().end(); ++portIt )
    {
      bool const res = checkParameterPortCompatibility( *refPort, **portIt, messages );
      if( not res )
      {
        result = false;
        continue;
      }
    }
    for( auto portIt( connection.receivePorts().begin() ); portIt < connection.receivePorts().end(); ++portIt )
    {
      bool const res = checkParameterPortCompatibility( *refPort, **portIt, messages );
      if( not res )
      {
        result = false;
        continue;
      }
    }
    try
    {
      visr::ParameterType const paramType = refPort->parameterType();
      CommunicationProtocolType const protocol = refPort->protocolType();
      ParameterConfigBase const & paramConfig = refPort->parameterConfig();
      std::unique_ptr<CommunicationProtocolBase> protocolInstance
        = CommunicationProtocolFactory::createProtocol( protocol, paramType, paramConfig );
      if( not protocolInstance )
      {
        messages << "AudioSignalFlow: Could not instantiate communication protocol for parameter connection.\n";
        result = false;
      }
      // Connect to all ports of the connection.
      // This will also fail if the connection does not match the arity (one-to-one, one-to-many ...) of the protocol
      for( auto port : connection.sendPorts() )
      {
        if( port->parent().isTopLevel() ) // Whether this is 'symbolic' input on the top level.
        {
          // This is a toplevel parameter input. We need to create a communication protocol output port for that.
          // TODO: Improve exception safety!
          std::unique_ptr<CommunicationProtocolBase::Output> protocolOutput = CommunicationProtocolFactory::createOutput( protocol );
          protocolInstance->connectOutput( protocolOutput.get() );
          ProtocolReceiveEndpoints::value_type insertPair{ std::string(port->name()), std::move(protocolOutput) };
          mProtocolReceiveEndpoints.insert( std::move(insertPair) );
        }
        else
        {
          try
          {
            ParameterOutputBase & paramOutput = dynamic_cast<ParameterOutputBase&>(port->containingPort());
            protocolInstance->connectOutput( &(paramOutput.protocolOutput()) );
          }
          catch( std::exception const & ex )
          {
            result = false;
            messages << "Error connection protocol output to output port \"" << fullyQualifiedName( *port )
                     << "\": " << ex.what() << ".\n";
          }
        }
      }
      for( auto port: connection.receivePorts() )
      {
        if( port->parent().isTopLevel() ) // Whether this is 'symbolic' output on the top level.
        {
          // This is a toplevel parameter output. We need to create a communication protocol input port for that.
          // TODO: Improve exception safety!
          auto protocolIt = mProtocolSendEndpoints.insert( std::make_pair( port->name(), CommunicationProtocolFactory::createInput( protocol ) ) );
          protocolInstance->connectInput( protocolIt.first->second.get() );
        }
        else
        {
          try
          {
            ParameterInputBase & paramInput = dynamic_cast<ParameterInputBase&>(port->containingPort());
            protocolInstance->connectInput( &(paramInput.protocolInput()) );
          }
          catch( std::exception const & ex )
          {
            result = false;
            messages << "Error connection protocol output to output port \"" << fullyQualifiedName( *port )
                     << "\": " << ex.what() << ".\n";
          }
        }
      }
      mCommunicationProtocols.push_back( std::move( protocolInstance ) );
    }
    catch( const std::exception& ex )
    {
      messages << "AudioSignalFlow: Error while initialising the communication parameter infrastructure: "
               << ex.what() << ".\n";
      result = false;
    }
  }

  // At the moment, there are no infrastructure modifications.
  // Therefore the connection map is passed back unaltered.
  finalConnections = originalConnections;
  return result;
}

CommunicationProtocolBase::Output & AudioSignalFlow::externalParameterReceivePort( char const * portName )
{
  auto const findIt = mProtocolReceiveEndpoints.find( std::string(portName) );
  if( findIt == mProtocolReceiveEndpoints.end() )
  {
    throw std::out_of_range( detail::composeMessageString("External receive port named \"", portName , "\" does not exist." ) );
  }
  return *(findIt->second);
}

CommunicationProtocolBase::Input & AudioSignalFlow::externalParameterSendPort( char const * portName )
{
  auto const findIt = mProtocolSendEndpoints.find( std::string( portName ) );
  if( findIt == mProtocolSendEndpoints.end() )
  {
    throw std::out_of_range(
      detail::composeMessageString("External send port named \"", portName , "\" does not exist." ) );
  }
  return *(findIt->second);
}

bool AudioSignalFlow::initialiseSchedule( std::ostream & messages,
                                          AudioConnectionMap const & audioConnections,
                                          ParameterConnectionMap const & parameterConnections )
{
  bool result = true;
  if( not mFlow.isComposite() )
  {
    mProcessingSchedule.clear();
    AtomicComponent* atom = dynamic_cast<AtomicComponent *>( &(mFlow.component()) );
    if( not atom )
    {
      throw std::logic_error( "AudioSignalFlow: Internal error: Casting of non-composite component to AtomicComponent failed.");
    }
    mProcessingSchedule.push_back( atom );
  }
  else
  {
    SchedulingGraph depGraph;
    // TODO: check result!
    depGraph.initialise( mFlow, audioConnections, parameterConnections );
    auto const schedule = depGraph.sequentialSchedule();
    mProcessingSchedule.assign( schedule.begin(), schedule.end() );
  }
  return result;
}


std::size_t AudioSignalFlow::numberCommunicationProtocols() const
{
  return mCommunicationProtocols.size();
}

namespace // unnamed
{
#if 0
/**
 * Helper function to sum up the audio channels of a list of ports.
 */
std::size_t countAudioChannels( PortLookup<impl::AudioPortBaseImplementation>::PortTable const & portList )
{
  return std::accumulate( portList.begin( ), portList.end( ), static_cast<std::size_t>(0) ,
    []( std::size_t acc, impl::AudioPortBaseImplementation const * port ) { return acc + port->width( ); } );
}

void assignConsecutiveIndices( impl::AudioPortBaseImplementation * port, std::size_t & index, rrl::CommunicationArea<SampleType> & commArea )
{

  std::size_t const portWidth = port->width( );
  std::vector<std::size_t> indexVec( portWidth );
  std::size_t idx = index;
  std::generate( indexVec.begin( ), indexVec.end( ), [&idx] { return idx++; } );

  // TODO: This part needs replacing (checking for consecutivity, rearranging if necessary, assigning of base pointer and stride)
#ifdef NEW_AUDIO_CONNECTION
  port->setAudioBasePointer( commArea.data( ) );
  port->setAudioChannelStride( commArea.signalStride() );
  port->assignCommunicationIndices( indexVec.begin( ), indexVec.end( ) );
  index += portWidth;
#endif
}
#endif
// 
template<typename DataType, typename SetType >
bool inSet( DataType const & key, SetType set )
{
  return set.count(key) > 0;
}


/**
 * Data structure to hold an adjacency wish.
 * Format:
 * get<0>: The first send port of the requested adjacency pair
 * get<1>: The second send port of the requested adjacency pair.
 * get<2>: The receive port requesting the adjacent pair of send ports.
 */
using AdjacencyWish = std::tuple<impl::AudioPortBaseImplementation const *, impl::AudioPortBaseImplementation const *, impl::AudioPortBaseImplementation const * >;

using AdjacencyWishList = std::multiset<AdjacencyWish>;

// Data structure holding an ordered list of 
using ConnectionList = std::vector<std::pair<AudioChannel, AudioChannel > >;

using ChannelVector = std::vector<AudioChannel>;

/**
 * Internal function used to detect the first contiguity gap of the send ports 
 */
ChannelVector::const_iterator findContiguityGap( ChannelVector::const_iterator beginIt, ChannelVector::const_iterator endIt)
{
  return std::adjacent_find( beginIt, endIt,
   []( AudioChannel const & lhs, AudioChannel const & rhs )
  { return (lhs.port() != rhs.port()) or (rhs.channel() != lhs.channel() + 1);} );
}

/**
 * Check whether a connection list is contiguous, i.e., refers to a contiguous index range of a single send port.
 * @pre The elements of \p connections are ordered according to their receive channel index.
 */
bool contiguousRange( ChannelVector const & channels )
{
  return findContiguityGap(channels.begin(), channels.end()) == channels.end();
}

std::tuple<bool, AdjacencyWishList> possiblyContiguousRange( ChannelVector const & channels, impl::AudioPortBaseImplementation * receivePort )
{
  AdjacencyWishList wishList;
  for( ChannelVector::const_iterator runIt = channels.begin();; )
  {
    ChannelVector::const_iterator gapIt = findContiguityGap( runIt, channels.end() );
    if( gapIt == channels.end() )
    {
      return std::make_tuple( true, wishList );
    }
    ChannelVector::const_iterator nextIt = gapIt + 1;
    if( nextIt == channels.end() ) // Shouldn't be possible due to the workings of adjacent_find.
    {
      return std::make_tuple( true, wishList );
    }
    if( (runIt->channel() != runIt->port()->width()-1) or nextIt->channel() != 0 )
    {
      return std::make_tuple( false, AdjacencyWishList{} );
    }
    wishList.insert( std::make_tuple( runIt->port(), nextIt->port(), receivePort ) );
    runIt = nextIt;
  }
}

ChannelVector sendChannels( impl::AudioPortBaseImplementation const * receivePort, AudioConnectionMap const & flatConnections )
{
  ConnectionList receivingConnections;
  std::copy_if( flatConnections.begin(), flatConnections.end(), std::back_inserter( receivingConnections ),
      [receivePort]( AudioConnectionMap::value_type const & conn ) { return conn.second.port() == receivePort; } );
  if( receivingConnections.size() != receivePort->width() )
  {
    std::stringstream str; str << "AudioSignalFlow: Number of incoming channels (" << receivingConnections.size() << ") to receive port " << fullyQualifiedName( *receivePort )
      << " does not match the port width (" << receivePort->width() << ").";
    throw std::logic_error( str.str() );
  }
  ChannelVector sendRange( receivePort->width(), AudioChannel( nullptr, 0 ) );
  for( ConnectionList::value_type const & conn : receivingConnections )
  {
    std::size_t receiveChannel = conn.second.channel();
    if( receiveChannel >= receivePort->width() )
    {
      std::stringstream str; str << "AudioSignalFlow: Connection entry " << conn.first << "->" << conn.second << " exceeds the receiver port width (" << receivePort->width() << ").";
      throw std::logic_error( str.str() );
    }
    if( sendRange[receiveChannel].port() != nullptr )
    {
      std::stringstream str; str << "AudioSignalFlow: More than one connection to receive channel " << conn.second << ".";
      throw std::logic_error( str.str() );
    }
    sendRange[receiveChannel] = conn.first;
  }
  return sendRange;
}

// Hold the data 
using OffsetList = std::vector<std::pair<impl::AudioPortBaseImplementation const *,  std::size_t> >;

using OffsetTable = std::map<AudioSampleType::Id, OffsetList >;

std::size_t calcBufferRequirement( impl::AudioPortBaseImplementation const * port, std::size_t period, std::size_t alignment )
{
  return port->width() * efl::nextAlignedSize( period * port->sampleSize(), alignment );
}

} // namespace unnamed.

bool AudioSignalFlow::initialiseAudioConnections( std::ostream & messages, AudioConnectionMap const & originalConnections, AudioConnectionMap & finalConnections )
{
  mTopLevelAudioInputs.clear();
  mTopLevelAudioOutputs.clear();
  mCaptureChannels.clear();
  mPlaybackChannels.clear();

  AudioConnectionMap tmpConnections; // To be assigned to or swapped with finalConnections after the initialisation was successful.

  bool result = true; // Result variable, is set to false if an error occurs.


  // Collect all used audio sample types.
  std::set<AudioSampleType::Id> usedSampleTypes;
  std::transform( originalConnections.begin(), originalConnections.end(),
    std::inserter( usedSampleTypes, usedSampleTypes.begin() ), []( AudioConnectionMap::value_type const & connection ) { return connection.first.port()->sampleType(); } );

  std::size_t const alignment = cVectorAlignmentBytes; // Maybe replace by a function that returns the current alignment setting.
  std::size_t const blockSize = mFlow.period();
  std::size_t totalAudioPoolSize = 0;

  using PortOffsetLookup = std::map<impl::AudioPortBaseImplementation *, std::size_t>;
  std::map<AudioSampleType::Id, PortOffsetLookup> allSendPortOffsets;
  std::map<AudioSampleType::Id, PortOffsetLookup> allReceivePortOffsets;

  for( AudioSampleType::Id sampleTypeId : usedSampleTypes )
  {
    PortOffsetLookup sendOffsets;
    PortOffsetLookup receiveOffsets;
    AudioConnectionMap typedConnections;
    std::copy_if(originalConnections.begin(), originalConnections.end(), std::inserter( typedConnections, typedConnections.begin()),
                 [sampleTypeId]( AudioConnectionMap::value_type const & connection ) { return connection.first.port()->sampleType() == sampleTypeId; } );
    std::vector<impl::AudioPortBaseImplementation*> allSendPorts;
    std::transform( typedConnections.begin(), typedConnections.end(), std::back_inserter(allSendPorts), []( AudioConnectionMap::value_type const & conn ){ return conn.first.port();} );
    std::sort( allSendPorts.begin(), allSendPorts.end() ); // Sorting is a required before unique.
    allSendPorts.erase( std::unique( allSendPorts.begin(), allSendPorts.end() ), allSendPorts.end() ); // remove duplicates.
    // Partition the send ports such that the external ports come first.
    std::partition( allSendPorts.begin(), allSendPorts.end(), [](impl::AudioPortBaseImplementation const * port ){ return isToplevelPort(port); } );

    std::size_t sendPortOffset = 0;
    for( impl::AudioPortBaseImplementation * sendPort: allSendPorts )
    {
      sendOffsets.insert( std::make_pair( sendPort, sendPortOffset ) );
      sendPortOffset += sendPort->width();
    }
    // sendPortOffset now contains the port index one past the all original send ports.

    // Now check the receive ports and see where we need routing components to get the receive ports continuous.
    std::vector<impl::AudioPortBaseImplementation*> allReceivePorts;
    std::transform( typedConnections.begin(), typedConnections.end(), std::back_inserter(allReceivePorts), []( AudioConnectionMap::value_type const & conn ){ return conn.second.port();} );
    std::sort( allReceivePorts.begin(), allReceivePorts.end() ); // Sorting is a required before unique.
    allReceivePorts.erase( std::unique( allReceivePorts.begin(), allReceivePorts.end() ), allReceivePorts.end() );
    // Partition the send ports such that the external ports come first.
    std::partition( allReceivePorts.begin(), allReceivePorts.end(), [](impl::AudioPortBaseImplementation const * port ){ return isToplevelPort(port); } );
    for( impl::AudioPortBaseImplementation * receivePort: allReceivePorts )
    {
      if( receivePort->width() == 0 ) // Zero-wdth port are legal, but we need to skip the computation (sends[0] would be illegal)
      {
        continue;
      }
      AudioConnectionMap localConnections; // all connections ending in receivePort
      std::copy_if( typedConnections.begin(), typedConnections.end(), std::inserter(localConnections, localConnections.begin()),
                    [receivePort](AudioConnectionMap::value_type const & connection ){ return connection.second.port() == receivePort; } );

      ChannelVector sends = sendChannels( receivePort, localConnections ); // extracting by receivePort would not be necessary (already done for localConnections)
      if( contiguousRange(sends) )
      {
        impl::AudioPortBaseImplementation * correspondingSendPort = sends[0].port();
        auto const sendLookupIt = sendOffsets.find( correspondingSendPort );
        if( sendLookupIt == sendOffsets.end() )
        {
          throw std::logic_error("Internal logic error: Send port not found in offset lookup table.");
        }
        std::size_t const receivePortOffset = sendLookupIt->second + sends[0].channel();

        receiveOffsets.insert( std::make_pair( receivePort, receivePortOffset ) );
        tmpConnections.insert( localConnections.begin(), localConnections.end() );
      }
      else
      {
        // Create index list
        // Instantiate internal routing component
        // Create connection entries t
        // Add entry to sendPortOffsets,
        // add width to sendPortOffset counter

        throw std::runtime_error( "Not implemented yet");
      }
    }
    allSendPortOffsets.insert( std::make_pair(sampleTypeId, std::move(sendOffsets) ) );
    allReceivePortOffsets.insert( std::make_pair(sampleTypeId, std::move(receiveOffsets) ) );

    std::size_t const typedPoolSize =  sendPortOffset * efl::nextAlignedSize( blockSize*AudioSampleType::typeSize(sampleTypeId), alignment );
    totalAudioPoolSize += typedPoolSize;
  }
  mAudioSignalPool.reset( new AudioSignalPool( totalAudioPoolSize, alignment ) );

  // Second run: Do the actual buffer assignment (for contiguous input and output ranges)
  std::size_t poolOffsetBytes = 0;
  for( AudioSampleType::Id sampleTypeId : usedSampleTypes )
  {
    std::size_t const typeOffset = poolOffsetBytes;
    std::size_t typedChannelCount = 0;
    std::size_t const channelSizeBytes = efl::nextAlignedSize( blockSize*AudioSampleType::typeSize(sampleTypeId), alignment );
    if( channelSizeBytes % AudioSampleType::typeSize(sampleTypeId) )
    {
      throw std::logic_error( "AudioSignalFlow::initialiseAudio(): Internal logic error"
                              "Aligned channel stride is not a multiple of the sample size." );
    }
    std::size_t const channelSizeSamples = channelSizeBytes / AudioSampleType::typeSize(sampleTypeId);

    PortOffsetLookup const sendOffsets = allSendPortOffsets[sampleTypeId] ;
    PortOffsetLookup const receiveOffsets = allReceivePortOffsets[sampleTypeId];

    for( auto const sendEntry : sendOffsets )
    {
      std::size_t const portWidth = sendEntry.first->width();
      std::size_t const channelOffset = sendEntry.second;
      std::size_t finalByteOffset = typeOffset + channelSizeBytes * channelOffset;
      sendEntry.first->setBufferConfig( mAudioSignalPool->basePointer() + finalByteOffset, channelSizeSamples );

      typedChannelCount += portWidth;
    }
    poolOffsetBytes += typedChannelCount * channelSizeBytes;

    // Same for receive ports.
    for( auto const receiveEntry : receiveOffsets )
    {
      impl::AudioPortBaseImplementation * port = receiveEntry.first;
      std::size_t const portWidth = port->width();
      // Extra sanity checking whether the insertion of internal signal routing primitives has been successful.
      ChannelVector sends = sendChannels( port, tmpConnections );
      if( not contiguousRange( sends ) )
      {
        throw std::logic_error( "AudioSignalFlow::initialiseAudio(): Internal logic error"
            "Non-contiguous port input range despite previous modification stage." );
      }
      std::size_t const channelOffset = receiveEntry.second;
      std::size_t finalByteOffset = typeOffset + channelSizeBytes * channelOffset;
      receiveEntry.first->setBufferConfig( mAudioSignalPool->basePointer() + finalByteOffset, channelSizeSamples );
    }
  }
  if( poolOffsetBytes != totalAudioPoolSize )
  {
    throw std::logic_error( "AudioSignalFlow::initialiseAudio(): Internal logic error: "
        "Size of audio channel buffers differs from previously calculated value." );
  }

  mTopLevelAudioInputs.clear();
  std::transform( tmpConnections.begin(), tmpConnections.end(), std::back_inserter(mTopLevelAudioInputs),
    []( AudioConnectionMap::value_type const & conn ){ return conn.first.port();} );
  std::sort(mTopLevelAudioInputs.begin(), mTopLevelAudioInputs.end());
  mTopLevelAudioInputs.erase( std::unique(mTopLevelAudioInputs.begin(), mTopLevelAudioInputs.end()), mTopLevelAudioInputs.end() );
  auto const topLevelInputsEnd = std::partition(mTopLevelAudioInputs.begin(), mTopLevelAudioInputs.end(),
        [](impl::AudioPortBaseImplementation const * port ){ return isToplevelPort(port); } );
  mTopLevelAudioInputs.erase( topLevelInputsEnd, mTopLevelAudioInputs.end() );

  mTopLevelAudioOutputs.clear();
  std::transform( tmpConnections.begin(), tmpConnections.end(), std::back_inserter(mTopLevelAudioOutputs),
    []( AudioConnectionMap::value_type const & conn ){ return conn.second.port();} );
  std::sort(mTopLevelAudioOutputs.begin(), mTopLevelAudioOutputs.end());
  mTopLevelAudioOutputs.erase( std::unique(mTopLevelAudioOutputs.begin(), mTopLevelAudioOutputs.end()), mTopLevelAudioOutputs.end() );
  auto const topLevelOutputsEnd = std::partition(mTopLevelAudioOutputs.begin(), mTopLevelAudioOutputs.end(),
        [](impl::AudioPortBaseImplementation const * port ){ return isToplevelPort(port); } );
  mTopLevelAudioOutputs.erase( topLevelOutputsEnd, mTopLevelAudioOutputs.end() );

  finalConnections = tmpConnections;

#if 0
  PortLookup<impl::AudioPortBaseImplementation> const allPorts( mFlow, true /*recursive*/ );


  // Count the buffer size
  std::size_t totalBufferSize = 0;
  for( impl::AudioPortBaseImplementation const * port : allPorts.allNonPlaceholderSendPorts() )
  {
    totalBufferSize += calcBufferRequirement( port, blockSize, alignment );
  }
  // TODO: We should be able to handle them together.
  for( impl::AudioPortBaseImplementation const * port : allPorts.externalPlaybackPorts() )
  {
    if( mFlow.isComposite() )
    {
      ChannelVector sends = sendChannels( port, originalConnections );
      if( not contiguousRange( sends ) )
      {
        totalBufferSize += calcBufferRequirement( port, blockSize, alignment );
      }
    }
    else
    {
      totalBufferSize += calcBufferRequirement( port, blockSize, alignment );
    }
  }
  for( impl::AudioPortBaseImplementation const * port : allPorts.concreteReceivePorts() )
  {
    ChannelVector sends = sendChannels( port, originalConnections );
    if( not contiguousRange( sends ) )
    {
      totalBufferSize += calcBufferRequirement( port, blockSize, alignment );
    }
  }
  mAudioSignalPool.reset( new AudioSignalPool( totalBufferSize, alignment ) );
#endif


#if 0
  // For the actual assignment, operate per sample type



  std::size_t currentOffset = 0;
  for( AudioSampleType::Id sampleType : usedSampleTypes )
  {
    // std::size_t const dataTypeOffset= currentOffset;

    using PortOffsetLookup = std::map<impl::AudioPortBaseImplementation const *, std::size_t>;
    PortOffsetLookup sendOffsetLookup;
//    std::size_t numTypedChannels = 0; // Channels allocated for that type.
    std::size_t const channelStrideBytes = efl::nextAlignedSize( blockSize * AudioSampleType::typeSize( sampleType ), alignment );
    std::size_t const channelStrideSamples = channelStrideBytes / AudioSampleType::typeSize( sampleType );
    std::vector<impl::AudioPortBaseImplementation * > typedSendPorts;
    // First, get and assign all the capture ports. This keeps the capture ports in front.
    // Note: It should be possible to put both types in the list, one after each other, and then handle them in the same loop.
    std::copy_if( allPorts.externalCapturePorts().begin(), allPorts.externalCapturePorts().end(), std::back_inserter( typedSendPorts ),
      [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
    if( mFlow.isComposite() )
    {
      std::copy_if( allPorts.concreteSendPorts().begin(), allPorts.concreteSendPorts().end(), std::back_inserter( typedSendPorts ),
      [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
    }
    else
    {
      // In case of atomic top-level components, 
      assert( allPorts.concreteReceivePorts().empty() and allPorts.concreteSendPorts().empty() );
      std::copy_if( allPorts.externalPlaybackPorts().begin(), allPorts.externalPlaybackPorts().end(), std::back_inserter( typedSendPorts ),
        [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
    }

    for( impl::AudioPortBaseImplementation * port : typedSendPorts )
    {
      sendOffsetLookup.insert( std::make_pair( port, currentOffset ) );
      port->setBufferConfig( mAudioSignalPool->basePointer() + currentOffset, channelStrideSamples );
      currentOffset += port->width() * channelStrideBytes;
    }
    // Now tackle all receive ports. Put the external playback ports first in the list.
    std::vector<impl::AudioPortBaseImplementation * > typedReceivePorts;
    if( mFlow.isComposite() ) // In case of an atomic top-level component, there are no receive ports that need to be connected
    {
      std::copy_if( allPorts.externalPlaybackPorts().begin(), allPorts.externalPlaybackPorts().end(), std::back_inserter( typedReceivePorts ),
        [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
      std::copy_if( allPorts.concreteReceivePorts().begin(), allPorts.concreteReceivePorts().end(), std::back_inserter( typedReceivePorts ),
        [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
    }
    for( impl::AudioPortBaseImplementation * port : typedReceivePorts )
    {
      port->setChannelStrideSamples( channelStrideSamples );
      ChannelVector const sends = sendChannels( port, originalConnections );
      if( port->width() == 0 )
      {
        continue; // Ports of width zero are permitted but require no handling.
      }
      if( contiguousRange(sends) )
      {
        std::size_t const sendStartIndex= sends[0].channel();
        PortOffsetLookup::const_iterator findIt = sendOffsetLookup.find( sends[0].port() );
        if( findIt == sendOffsetLookup.end() )
        {
          messages << "AudioFignalFlow: internal error: Send port of connection not found.\n";
          return false;
        }
        std::size_t const sendPortBaseIndex = findIt->second;
        char* basePointer = mAudioSignalPool->basePointer()+sendPortBaseIndex + channelStrideBytes * sendStartIndex;
	port->setBasePointer( basePointer );
        port->setChannelStrideSamples( channelStrideSamples );
      }
      else
      {
        // We need to insert an internal atomic component to route the inputs into a contiguous order
        std::stringstream routingCompName;
        routingCompName << fullyQualifiedName( *port ) << "_inputrouting";

        // Abusing the flow as parent component is slightly messy, but should be harmless.
        // Needs to be changed when we do a more thorough separation between logical hierarchy and execution model layout.
        // The cast should not fail because if the top-level flow mFlow is atomic, all input ranges are
        // necessarily contiguous.
        CompositeComponent* parentComp = dynamic_cast<CompositeComponent*>(&(mFlow.component()));

        if( not parentComp )
        {
          throw std::logic_error( "Internal logic error: Detected a noncontiguous channel range for an input port of an atomic top-level component." );
        }

        static const std::size_t cInvalidIndex = std::numeric_limits<std::size_t>::max();
        std::vector<std::size_t> sendOffsets( port->width(), cInvalidIndex );
        for( std::size_t chIdx(0); chIdx < port->width(); ++chIdx )
        {
          PortOffsetLookup::const_iterator findIt = sendOffsetLookup.find( sends[0].port() );
          if( findIt == sendOffsetLookup.end() )
          {
            messages << "AudioFignalFlow: internal error: Send port of connection not found.\n";
            return false;
          }
          std::size_t const sendIndex= sends[chIdx].channel();
          std::size_t const sendPortBaseIndex = findIt->second; // the channel offset (in bytes) 
          sendOffsets[chIdx] = sendPortBaseIndex + channelStrideBytes * sendIndex;
        }

        // Find and subtract the minimum offset. This also accounts to the offset up to this sample type,
        // which might not be a multiple of the channel stride.
        auto const minOffsetIt = std::min_element( sendOffsets.cbegin(), sendOffsets.cend() );
        assert( minOffsetIt != sendOffsets.end() );
        std::size_t const minOffset = *minOffsetIt;
        std::for_each( sendOffsets.begin(), sendOffsets.end(),
                       [minOffset](std::size_t & val){ val -= minOffset;} );
        // All offsets must be multiples of the channel stride now.
        if( std::find_if( sendOffsets.begin(), sendOffsets.end(), 
                          [channelStrideBytes](std::size_t & val){ return val % channelStrideBytes != 0; } ) != sendOffsets.end() )
        {
          throw std::logic_error( "Internal logic error: Send offsets for a noncontiguous input port are not multiples of the channels." );          
        }
        // Transform channel strides to channel indices.
        std::for_each( sendOffsets.begin(), sendOffsets.end(),
                       [channelStrideBytes](std::size_t & val){ val /= channelStrideBytes;} );

        std::unique_ptr<AtomicComponent> routingComp
          = createSignalRoutingComponent( sampleType,
                                          mFlow.context(),
                                          routingCompName.str().c_str(),
                                          static_cast<CompositeComponent*>(&(mFlow.component())),
                                          port->width(),
                                          sendOffsets );
        try
        {
          AudioPortBase& routingIn = routingComp->audioPort("in");
          routingIn.implementation().setBufferConfig( mAudioSignalPool->basePointer()+minOffset, channelStrideSamples );
        }
        catch( std::exception const & ex )
        {
          throw std::logic_error( detail::composeMessageString("Internal logic error: Unable to retrieve input port of internal component \"", routingCompName.str().c_str(), "\": ", ex.what(), "." ) );
        }

        try
        {
          AudioPortBase& routingOut = routingComp->audioPort("in");
          routingOut.implementation().setBufferConfig( mAudioSignalPool->basePointer()+currentOffset, channelStrideSamples );
          port->setBufferConfig( mAudioSignalPool->basePointer() + currentOffset, channelStrideSamples );

        }
        catch( std::exception const & ex )
        {
          throw std::logic_error( detail::composeMessageString("Internal logic error: Unable to retrieve output port of internal component \"", routingCompName.str().c_str(), "\": ", ex.what(), "." ) );
        }

        currentOffset += port->width() * channelStrideBytes;
        mInfrastructureComponents.push_back( std::move(routingComp) );
      }
    }
  }
  if( currentOffset != totalBufferSize )
  {
    messages << "AudioSignalFlow: internal screwup: size of data allocated differs from previously computed value.";
    return false;
  }

  // Setup the data structures for communication with the outside world.
  mTopLevelAudioInputs.assign( allPorts.externalCapturePorts().begin(), allPorts.externalCapturePorts().end() );
  mTopLevelAudioOutputs.assign( allPorts.externalPlaybackPorts().begin(), allPorts.externalPlaybackPorts().end() );
  // Count the number of external input (capture) and output (playback) ports
  // Note that this does not account for the ordering imposed by multiple capture or playback ports.
  std::size_t const numberOfCaptureChannels = std::accumulate( allPorts.externalCapturePorts().begin(), allPorts.externalCapturePorts().end(),
    static_cast<std::size_t>(0), []( std::size_t val, impl::AudioPortBaseImplementation const * port ) { return val + port->width(); } );
  std::size_t numberOfPlaybackChannels = std::accumulate( allPorts.externalPlaybackPorts().begin(), allPorts.externalPlaybackPorts().end(),
    static_cast<std::size_t>(0), []( std::size_t val, impl::AudioPortBaseImplementation const * port ) { return val + port->width(); } );;
  mCaptureChannels.clear(); mCaptureChannels.reserve( numberOfCaptureChannels );
  mPlaybackChannels.clear(); mPlaybackChannels.reserve( numberOfPlaybackChannels );

#endif

  // Not sure whether we want to keep that.
  std::size_t externalCaptureChannelIdx = 0;
  for( impl::AudioPortBaseImplementation * port : mTopLevelAudioInputs )
  {
    char * basePointer = static_cast<char*>(port->basePointer());
    std::size_t const width = port->width();
    std::size_t const stride = port->channelStrideBytes();
    for( std::size_t chIdx( 0 ); chIdx < width; ++chIdx, ++externalCaptureChannelIdx )
    {
      mCaptureChannels.push_back( basePointer + chIdx * stride );
    }
  }
  std::size_t externalPlaybackChannelIdx = 0;
  for( impl::AudioPortBaseImplementation * port : mTopLevelAudioOutputs )
  {
    char * basePointer = static_cast<char*>(port->basePointer());
    std::size_t const width = port->width();
    std::size_t const stride = port->channelStrideBytes();
    for( std::size_t chIdx( 0 ); chIdx < width; ++chIdx, ++externalPlaybackChannelIdx )
    {
      mPlaybackChannels.push_back( basePointer + chIdx * stride );
    }
  }

  //// Collect all audio sample types used.
  //std::set<AudioSampleType::Id> usedSampleTypes;
  //std::transform( allPorts.allNonPlaceholderReceivePorts().begin(), allPorts.allNonPlaceholderReceivePorts().end(),
  //  std::inserter( usedSampleTypes, usedSampleTypes.begin() ), [](impl::AudioPortBaseImplementation const * port ){ return port->sampleType(); } );
  //std::transform( allPorts.allNonPlaceholderSendPorts().begin(), allPorts.allNonPlaceholderSendPorts().end(),
  //  std::inserter( usedSampleTypes, usedSampleTypes.begin() ), []( impl::AudioPortBaseImplementation const * port ) { return port->sampleType(); } );

  //std::vector<std::size_t> channelBufferOffsets( usedSampleTypes.size(), 0 );

  //for( AudioSampleType::Id sampleType : usedSampleTypes )
  //{
  //  std::size_t channelBufferOffset = totalBufferSize;
  //  // Select all receive ports with a given port 
  //  using PortTable = PortLookup<impl::AudioPortBaseImplementation>::PortTable;
  //  PortTable typedReceivePorts;
  //  std::copy_if( allPorts.allNonPlaceholderReceivePorts().begin(), allPorts.allNonPlaceholderReceivePorts().end(), std::inserter( typedReceivePorts, typedReceivePorts.begin() ),
  //    [sampleType](impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );

  //  // Sort the ports into three categories:
  //  PortTable contiguousReceivePorts;    // Ports that are connected to a contiguous range.
  //  PortTable nonContiguousReceivePorts; // Ports that are not connected to a contiguous range (and cannot be made to)
  //  AdjacencyWishList adjacencyWishList; // Data structure holding receive ports with adjacency wishes, i.e., putting two send ports directly after another to avoid an explicit copy operation.
  //  for( impl::AudioPortBaseImplementation * receivePort : typedReceivePorts )
  //  {
  //    ConnectionList receivingConnections;
  //    std::copy_if( flatConnections.begin(), flatConnections.end(), std::back_inserter(receivingConnections),
  //      [receivePort]( AudioConnectionMap::ValueType const & conn ){ return conn.second.port() == receivePort; } );
  //    if( receivingConnections.size() != receivePort->width() )
  //    {
  //      messages << "AudioSignalFlow: Number of incoming channels (" << receivingConnections.size() << ") to receive port " << fullyQualifiedName( *receivePort) 
  //               << " does not match the port width (" << receivePort->width() << ").\n";
  //      return false;
  //    }
  //    ChannelVector sendRange( receivePort->width(), AudioChannel(nullptr,0) );
  //    for( ConnectionList::value_type const & conn : receivingConnections )
  //    {
  //      std::size_t receiveChannel = conn.second.channel();
  //      if( receiveChannel >= receivePort->width() )
  //      {
  //        messages << "AudioSignalFlow: Connection entry " << conn.first << "->" << conn.second << " exceeds the receiver port width (" << receivePort->width() << ").\n";
  //        return false;
  //      }
  //      if( sendRange[receiveChannel].port() != nullptr )
  //      {
  //        messages << "AudioSignalFlow: More than one connection to receive channel " << conn.second << ".\n";
  //        return false;
  //      }
  //      sendRange[receiveChannel] = conn.first;
  //    }
  //    if( std::find_if(sendRange.begin(), sendRange.end(), []( auto const & val ){ return val.port() == nullptr; } )
  //      != sendRange.end() )
  //    {
  //      messages << "AudioSignalFlow: Not all channels of receive port " << fullyQualifiedName( *receivePort ) << ".\n";
  //      return false;
  //    }
  //    if( contiguousRange( sendRange ) )
  //    {
  //      contiguousReceivePorts.insert( receivePort );
  //    }
  //    else
  //    {
  //      auto const ret = possiblyContiguousRange( sendRange, receivePort );
  //      if( std::get<0>(ret) )
  //      {
  //        AdjacencyWishList const & wish = std::get<1>( ret );
  //        adjacencyWishList.insert( wish.begin(), wish.end() );
  //      }
  //      else
  //      {
  //        nonContiguousReceivePorts.insert( receivePort );
  //      }
  //    }
  //  } // iterate over receive ports

  //  // Resolve adjacency wishes: Here we use a simple strategy to grant the first wish for a 'next port' and to reject all that follow.
  //  // More elaborate strategies might minimise the amount of data copied, or consider locality.
  //  AdjacencyWishList grantedAdjacencyWishes;
  //  AdjacencyWishList rejectedAdjacencyWishes;
  //  for( AdjacencyWishList::const_iterator wishIt(adjacencyWishList.begin()); wishIt != adjacencyWishList.end(); ) //  wishIt != adjacencyWishList.end(); ++wishIt
  //  {
  //    impl::AudioPortBaseImplementation const * currFirstPort = std::get<0>( *wishIt );
  //    AdjacencyWishList::const_iterator nextIt = std::find_if( wishIt, adjacencyWishList.end(), 
  //      [currFirstPort](auto const & val ) { return std::get<0>(val) != currFirstPort; } );
  //    assert( wishIt != nextIt );
  //    grantedAdjacencyWishes.insert( *wishIt );
  //    for( ;wishIt != nextIt; ++wishIt )
  //    {
  //      rejectedAdjacencyWishes.insert( *wishIt );
  //    }
  //  }
  //  OffsetList sendOffsets;
  //  std::size_t const channelSize = efl::nextAlignedSize( mFlow.period() * AudioSampleType::typeSize( sampleType ), alignment );
  //  std::size_t offsetCounter = 0;
  //  // Do not handle the external capture ports specially.
  //  while( !grantedAdjacencyWishes.empty() )
  //  {
  //    AdjacencyWish wish = *grantedAdjacencyWishes.begin();
  //    grantedAdjacencyWishes.erase( grantedAdjacencyWishes.begin() );
  //    sendOffsets.push_back( std::make_pair( std::get<0>( wish ), offsetCounter ) );
  //    offsetCounter += channelSize;
  //    impl::AudioPortBaseImplementation const * nextPort = std::get<1>( wish );
  //    for(;;)
  //    {
  //      if( std::find_if( grantedAdjacencyWishes.begin(), grantedAdjacencyWishes.end(),
  //        [nextPort]( AdjacencyWish const & w ){ return std::get<0>( w ) == nextPort; } ) == grantedAdjacencyWishes.end() )
  //      {
  //        break;
  //      }
  //    } // next 
  //  }

    //for( AdjacencyWishList::const_iterator wishIt( grantedAdjacencyWishes.begin()); wishIt != grantedAdjacencyWishes.end(); )
    //{
    //}

    //// Insert additional 'internal routing' components 
    //sendOffsets.push_back( std::make_pair( std::get<0>( *wishIt ), offsetCounter ) );
    //offsetCounter()
  return result;
}

} // namespace rrl
} // namespace visr
