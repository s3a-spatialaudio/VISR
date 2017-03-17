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

#include <libril/atomic_component.hpp>
#include <libril/audio_sample_type.hpp>
#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_factory.hpp>
#include <libril/communication_protocol_type.hpp>

#include <libvisr_impl/audio_connection_descriptor.hpp>
#include <libvisr_impl/audio_port_base_implementation.hpp>
#include <libvisr_impl/component_impl.hpp>
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

  // TODO: Might the parameter initalisation also result in a 

  bool const initParamResult = initialiseParameterInfrastructure( checkMessages );
  if( not initParamResult )
  {
    std::cout << "Messages during parameter initialisation: " << checkMessages.str() << std::endl;
    throw std::runtime_error( "AudioSignalFlow: Parameter infrastructure could not be initialised." );
  }
  bool const initAudioResult = initialiseAudioConnections( checkMessages );

  // TODO: Come with an updated connection list

  if( not initAudioResult )
  {
    // TODO: Devise other ways to transport messages.
    std::cout << "Messages during audio initialisation: " << checkMessages.str() << std::endl;
    throw std::runtime_error( "AudioSignalFlow: Audio connections could not be initialised." );
  }



  // TODO: Use the full list of components 
  bool const initScheduleResult = initialiseSchedule( checkMessages );
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

#ifndef USE_SIGNAL_POOL
// todo: make this method protected?
void AudioSignalFlow::initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                                    std::size_t alignmentElements /* = cVectorAlignmentSamples */ )
{
  mCommArea.reset( new rrl::CommunicationArea<SampleType>( numberOfSignals, signalLength, alignmentElements ) );
}
#endif

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
#ifdef USE_SIGNAL_POOL
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

#else
  // fill the capture part of the communication area.
  std::size_t const numCaptureChannels = numberOfCaptureChannels();
  for( std::size_t captureIdx( 0 ); captureIdx < numCaptureChannels; ++captureIdx )
  {
    SampleType * const destPtr = mCommArea->at( mCaptureChannels[captureIdx] );
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
  catch( std::exception const & /*ex*/ )
  {
    // TODO: Add error message to ex.what()
    return 1;
  }

  // collect the generated output from the playback part of the communication area and 
  // fill the playbackSamples accordingly.
  std::size_t const numPlaybackChannels = numberOfPlaybackChannels( );
  for( std::size_t playbackIdx( 0 ); playbackIdx < numPlaybackChannels; ++playbackIdx )
  {
    SampleType const * const srcPtr = mCommArea->at( mPlaybackChannels[playbackIdx] );
    // Note: We cannot assume an alignment as we don't know the alignment of the passed playbackSamples.
    // TODO: Add optional argument to the AudioCallback interface to signal the alignment of the input and output samples.
    efl::ErrorCode const res = efl::vectorCopy( srcPtr, playbackSamples[playbackIdx], mFlow.period(), 0 );
    if( res != efl::noError )
    {
      throw std::runtime_error( "AudioSignalFlow: Error while copying output samples samples." );
    }
  }
#endif
  // TODO: use a sophisticated enumeration to signal error conditions
  return 0; // Means 'no error'
}

void AudioSignalFlow::executeComponents()
{
  for( ProcessableInterface * pc : mProcessingSchedule )
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

bool AudioSignalFlow::initialiseParameterInfrastructure( std::ostream & messages )
{
  bool result = true;
  mCommunicationProtocols.clear();
  if( not mFlow.isComposite() )
  {
    // If the flow is not composite, we also have to initialise the top-level parameter ports.
    return result;
  }
  // - Recursively iterate over all composite components
  // - Collect all parameter ports (concrete and placeholders) in containers.
  // - Collect all connections and store them in a lookup map.
  // - Infer all connections from the receive end to check whether they contain no loops.
  ParameterConnectionMap allConnections;
  bool res = fillRecursive( allConnections, mFlow, messages );
  if( not res )
  {
    messages << "AudioSignalFlow: Parameter connections are inconsistent.\n";
    return false;
  }
  ParameterConnectionMap concreteConnections = resolvePlaceholders( allConnections );

  ParameterConnectionGraph const connectionGraph( concreteConnections );
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
        = CommunicationProtocolFactory::create( protocol, paramType, paramConfig );
      if( not protocolInstance )
      {
        messages << "AudioSignalFlow: Could not instantiate communication protocol for parameter connection.\n";
        result = false;
      }
#ifdef NEW_AUDIO_CONNECTION
      // Connect to all ports of the connection.
      // This will also fail if the connection does not match the arity (one-to-one, one-to-many ...) of the protocol
      for( auto portIt( connection.sendPorts().begin() ); portIt < connection.sendPorts().end(); ++portIt )
      {
        protocolInstance->connectOutput( *portIt );
      }
      for( auto portIt( connection.receivePorts().begin() ); portIt < connection.receivePorts().end(); ++portIt )
      {
        protocolInstance->connectInput( *portIt );
      }
#endif
      mCommunicationProtocols.push_back( std::move( protocolInstance ) );
    }
    catch( const std::exception& ex )
    {
      messages << "AudioSignalFlow: Error while initialising the communication parameter infrastructure: "
               << ex.what() << ".\n";
      result = false;
    }
  }

  // Check whether all ports have been connected.
  PortLookup<impl::ParameterPortBaseImplementation> allPorts( mFlow, true );
  for( impl::ParameterPortBaseImplementation const * const port : allPorts.allNonPlaceholderSendPorts() )
  {
#ifdef NEW_AUDIO_CONNECTION
    if( not port->isConnected() )
    {
      messages << "Parameter send port \"" << fullyQualifiedName( *port ) << "\" is not connected to a valid protocol.\n";
      result = false;
    }
#endif
  }
  for( impl::ParameterPortBaseImplementation const * const port : allPorts.allNonPlaceholderReceivePorts() )
  {
#ifdef NEW_AUDIO_CONNECTION
    if( not port->isConnected() )
    {
      messages << "Parameter receive port \"" << fullyQualifiedName( *port ) << "\" is not connected to a valid protocol.\n";
      result = false;
    }
#endif
  }
  return result;
}

bool AudioSignalFlow::initialiseSchedule( std::ostream & messages )
{
  bool result = true;
  if( not mFlow.isComposite() )
  {
    mProcessingSchedule.clear();
    mProcessingSchedule.push_back( dynamic_cast<AtomicComponent *>(&mFlow) );
  }
  else
  {
    AudioConnectionMap allAudioConnections;
    result &= allAudioConnections.fill( mFlow, messages, true/*recursive*/ );
    AudioConnectionMap const concreteAudioConnections = allAudioConnections.resolvePlaceholders();

    ParameterConnectionMap allParameterConnections;
    result &= fillRecursive( allParameterConnections, mFlow, messages );
    ParameterConnectionMap const concreteParameterConnections = resolvePlaceholders( allParameterConnections );

    if( not result ) // No use to go on with schedule computation if the connections are inconsistent.
    {
      return result;
    }

    SchedulingGraph depGraph;
    // TODO: check result!
    depGraph.initialise( mFlow, concreteAudioConnections, concreteParameterConnections );
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
      [receivePort]( AudioConnectionMap::ValueType const & conn ) { return conn.second.port() == receivePort; } );
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

std::ostream & operator<<( std::ostream & str, impl::AudioPortBaseImplementation const * port )
{
  return str << fullyQualifiedName( *port );
}

} // namespace unnamed.

bool AudioSignalFlow::initialiseAudioConnections( std::ostream & messages )
{
  mTopLevelAudioInputs.clear();
  mTopLevelAudioOutputs.clear();
  mCaptureChannels.clear();
  mPlaybackChannels.clear();
  bool result = true; // Result variable, is set to false if an error occurs.
#if 1
  PortLookup<impl::AudioPortBaseImplementation> const allPorts( mFlow, true /*recursive*/ );
  std::cout << "All ports:\n" << allPorts << "\n\n" << std::endl;

  AudioConnectionMap allConnections;

  std::stringstream errMessages;
  if( not allConnections.fill( mFlow, errMessages, true/*recursive*/ ) )
  {
    throw std::logic_error( "AudioSignalFlow: Audio connections are inconsistent." + errMessages.str() );
  }

  std::cout << "All audio connections:\n" << allConnections << "\n\n" << std::endl;

  // TODO: catch exceptions
  AudioConnectionMap const flatConnections = allConnections.resolvePlaceholders();

  std::cout << "Flat audio connections:\n" << flatConnections << "\n\n" << std::endl;


  std::size_t const alignment = cVectorAlignmentBytes; // Maybe replace by a function that returns the current alignment setting.
  std::size_t const blockSize = mFlow.period();

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
      ChannelVector sends = sendChannels( port, flatConnections );
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
    ChannelVector sends = sendChannels( port, flatConnections );
    if( not contiguousRange( sends ) )
    {
      totalBufferSize += calcBufferRequirement( port, blockSize, alignment );
    }
  }
  mAudioSignalPool.reset( new AudioSignalPool( totalBufferSize, alignment ) );

  std::size_t captureChannelsRunningIndex = 0;
  std::size_t playbackChannelsRunningIndex = 0;

  // For the actual assignment, operate per sample type
  // Collect all audio sample types used.
  std::set<AudioSampleType::Id> usedSampleTypes;
  std::transform( allPorts.allNonPlaceholderReceivePorts().begin(), allPorts.allNonPlaceholderReceivePorts().end(),
    std::inserter( usedSampleTypes, usedSampleTypes.begin() ), []( impl::AudioPortBaseImplementation const * port ) { return port->sampleType(); } );
  std::transform( allPorts.allNonPlaceholderSendPorts().begin(), allPorts.allNonPlaceholderSendPorts().end(),
    std::inserter( usedSampleTypes, usedSampleTypes.begin() ), []( impl::AudioPortBaseImplementation const * port ) { return port->sampleType(); } );

  std::size_t currentOffset = 0;
  for( AudioSampleType::Id sampleType : usedSampleTypes )
  {
    std::size_t const dataTypeOffset= currentOffset;

    using PortOffsetLookup = std::map<impl::AudioPortBaseImplementation const *, std::size_t>;
    PortOffsetLookup sendOffsetLookup;
    std::size_t numTypedChannels = 0; // Channels allocated for that type.
    std::size_t const channelStrideBytes = efl::nextAlignedSize( blockSize * AudioSampleType::typeSize( sampleType ), alignment );
    std::size_t const channelStrideSamples = channelStrideBytes / AudioSampleType::typeSize( sampleType );
    std::vector<impl::AudioPortBaseImplementation * > typedPorts;
    // First, get and assign all the capture ports. This keeps the capture ports in front.
    // Note: It should be possible to put both types in the list, one after each other, and then handle them in the same loop.
    std::copy_if( allPorts.externalCapturePorts().begin(), allPorts.externalCapturePorts().end(), std::back_inserter( typedPorts ),
      [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
    if( mFlow.isComposite() )
    {
      std::copy_if( allPorts.concreteSendPorts().begin(), allPorts.concreteSendPorts().end(), std::back_inserter( typedPorts ),
      [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
    }
    else
    {
      // In case of atomic top-level components, 
      assert( allPorts.concreteReceivePorts().empty() and allPorts.concreteSendPorts().empty() );
      std::copy_if( allPorts.externalPlaybackPorts().begin(), allPorts.externalPlaybackPorts().end(), std::back_inserter( typedPorts ),
        [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
    }

    for( impl::AudioPortBaseImplementation * port : typedPorts )
    {
      sendOffsetLookup.insert( std::make_pair( port, currentOffset ) );
      port->setBasePointer( mAudioSignalPool->basePointer() + currentOffset );
      port->setChannelStrideSamples( channelStrideSamples );
      currentOffset += port->width() * channelStrideBytes;
    }
    // Now tackle all receive ports. Put the external playback ports first in the list.
    typedPorts.clear();
    if( mFlow.isComposite() ) // In case of an atomic top-level component, there are no receive ports that need to be connected
    {
      std::copy_if( allPorts.externalPlaybackPorts().begin(), allPorts.externalPlaybackPorts().end(), std::back_inserter( typedPorts ),
        [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
      std::copy_if( allPorts.concreteReceivePorts().begin(), allPorts.concreteReceivePorts().end(), std::back_inserter( typedPorts ),
        [sampleType]( impl::AudioPortBaseImplementation const * port ) { return port->sampleType() == sampleType; } );
    }
    for( impl::AudioPortBaseImplementation * port : typedPorts )
    {
      port->setChannelStrideSamples( channelStrideSamples );
      ChannelVector const sends = sendChannels( port, flatConnections );
      if( port->width() == 0 )
      {
        continue; // Ports of width zero are permitted.
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
        port->setChannelStrideSamples( channelStrideSamples );
      }
      else
      {
        messages << "Temporarily not implemented\n";
        return false;
        currentOffset += port->width() + channelStrideBytes;
      }
    }
    // Test code to initialise the communication areas with 'marker' values.
    // Makes sense only for float data
    // TODO: Remove ASAP
    if( sampleType == AudioSampleType::floatId )
    {
      std::size_t const numChannels = (currentOffset - dataTypeOffset) / channelStrideBytes;
      for( std::size_t channelIdx( 0 ); channelIdx < numChannels; ++channelIdx )
      {
        float val = static_cast<float>(channelIdx);
        std::fill_n( reinterpret_cast<float*>(mAudioSignalPool->basePointer() + dataTypeOffset + channelStrideBytes * channelIdx),
        period(), val );
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
#else
  PortLookup<impl::AudioPortBaseImplementation> const portLookup( mFlow, true /*recursive*/ );

  // Compute the number of audio channels for the different categories that are kept in the communicatio area.
  //  std::size_t const numPlaybackChannels = countAudioChannels( portLookup.mExternalPlaybackPorts );
  std::size_t const numCaptureChannels = countAudioChannels( portLookup.externalCapturePorts() );
  std::size_t const numConcreteOutputChannels = countAudioChannels( portLookup.concreteSendPorts() );
  std::size_t const totalSignalChannels = numCaptureChannels + numConcreteOutputChannels;

  std::size_t const period = mFlow.period();

  mCommArea.reset( new CommunicationArea<SampleType>( totalSignalChannels, period, cVectorAlignmentSamples ) );

  // Assign consecutive indices to the ports that need physical communication vectors.
  // First we do that for the external capture ports, because this part is identical for atomic and component top-level signal flows.
  SignalIndexType offset = 0;
  // std::size_t const captureSignalOffset = offset; // ATM unused.
#ifdef NEW_AUDIO_CONNECTION
  std::for_each( portLookup.externalCapturePorts().begin( ), portLookup.externalCapturePorts().end( ),
    std::bind( assignConsecutiveIndices, std::placeholders::_1, std::ref( offset ), std::ref( *mCommArea ) ) );
  // Initialise the 'concrete' internal receive ports.
  // std::size_t const concreteOutputSignalOffset = offset; // ATM unused
  std::for_each( portLookup.concreteSendPorts().begin( ), portLookup.concreteSendPorts().end( ),
    std::bind( assignConsecutiveIndices, std::placeholders::_1, std::ref( offset ), std::ref( *mCommArea ) ) );
  if( offset != mCommArea->numberOfSignals( ) )
  {
    throw std::logic_error( "AudioSignalFlow::initialiseAudioConnections(): number of allocated " );
  }
#endif

  // Initialise the data structures for the external inputs (capture ports)
  mTopLevelAudioInputs.reserve( portLookup.externalCapturePorts().size( ) );
  mCaptureChannels.resize( numCaptureChannels );
  std::size_t captureOffset = 0;
  for( impl::AudioPortBaseImplementation * capturePort : portLookup.externalCapturePorts() )
  {
    mTopLevelAudioInputs.push_back( capturePort );
#ifdef NEW_AUDIO_CONNECTION
    std::copy( capturePort->indices( ), capturePort->indices( ) + capturePort->width( ), &mCaptureChannels[captureOffset] );
#endif
    captureOffset += capturePort->width( );
  }

  // Allocate space for the external playback ports and indices.
  // Filling is different whether the top-level flow is composite or atomic.
  mPlaybackChannels.resize( countAudioChannels( portLookup.externalPlaybackPorts() ) );
  mTopLevelAudioOutputs.reserve( portLookup.externalPlaybackPorts().size( ) );
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
      concreteConnections = std::move(allConnections.resolvePlaceholders());
    }
    catch( std::exception const & ex )
    {
      errMessages << ex.what() << '\n';
      result = false;
    }

    std::cout << "\n\n\nConcrete connections:\n" << concreteConnections << std::endl;

    // Initialise the audio ports.
    for( impl::AudioPortBaseImplementation * receivePort : portLookup.concreteReceivePorts() )
    {
      std::size_t const portWidth = receivePort->width( );
      std::vector<SignalIndexType> receiveIndices( portWidth );
      for( std::size_t chIdx( 0 ); chIdx < portWidth; ++chIdx )
      {
        // equal_range is used to detect multiple connections
        // Normally this should have been detected before in the checking phase.
        std::pair<AudioConnectionMap::const_iterator, AudioConnectionMap::const_iterator >
          findRange = concreteConnections.equal_range( AudioChannel( receivePort, chIdx ) );
        if( findRange.first == concreteConnections.end( ) )
        {
          throw std::runtime_error( "Audio signal connections : Did not find connection entry for receive channel " );
        }
        if( std::distance( findRange.first, findRange.second ) != 1 )
        {
          throw std::runtime_error( "Audio signal connections : Multiple connections detected." );
        }
#ifdef NEW_AUDIO_CONNECTION
        receiveIndices[chIdx] = findRange.first->second.mPort->indices( )[findRange.first->second.mIndex];
#endif
      }
#ifdef NEW_AUDIO_CONNECTION
      receivePort->setAudioBasePointer( mCommArea->data( ) );
      receivePort->setAudioChannelStride( mCommArea->signalStride( ) );
      receivePort->assignCommunicationIndices( receiveIndices.begin( ), receiveIndices.end( ) );
#endif
    }

    // same for the external playback ports
    // Note: the ordering of the external ports is defined by the order they are arranged in this set.
    for( impl::AudioPortBaseImplementation * playbackPort : portLookup.externalPlaybackPorts() )
    {
      std::size_t const portWidth = playbackPort->width( );
      std::vector<SignalIndexType> receiveIndices( portWidth );
      for( std::size_t chIdx( 0 ); chIdx < portWidth; ++chIdx )
      {
        // equal_range is used to detect multiple connections
        // Normally this should have been detected before in the checking phase.
        std::pair<AudioConnectionMap::const_iterator, AudioConnectionMap::const_iterator >
          findRange = concreteConnections.equal_range( AudioChannel( playbackPort, chIdx ) );
        if( findRange.first == concreteConnections.end( ) )
        {
          throw std::runtime_error( "Audio signal connections : Did not find connection entry for receive channel." );
        }
        if( std::distance( findRange.first, findRange.second ) != 1 )
        {
          throw std::runtime_error( "Audio signal connections : Multiple connections detected." );
        }
#ifdef NEW_AUDIO_CONNECTION
        receiveIndices[chIdx] = findRange.first->second.mPort->indices( )[findRange.first->second.mIndex];
#endif
      }
#ifdef NEW_AUDIO_CONNECTION
      playbackPort->setAudioBasePointer( mCommArea->data( ) );
      playbackPort->setAudioChannelStride( mCommArea->signalStride( ) );
      playbackPort->assignCommunicationIndices( receiveIndices.begin( ), receiveIndices.end( ) );
#endif
      mTopLevelAudioOutputs.push_back( playbackPort );
      std::copy( receiveIndices.begin( ), receiveIndices.end( ), &mPlaybackIndices[playbackIndexOffset] );
      playbackIndexOffset += portWidth;
    }
   } // Additional handling for composite top-level flows finished
  else // The flow consists only of a single atomic component.
  {
    // We need a separate treatment for the external playback indices.
    // This are linked (channel-by-channel) to the respective 'physical' output port of the atomic component.
    for( impl::AudioPortBaseImplementation * playbackPort : portLookup.externalPlaybackPorts() )
    {
      mTopLevelAudioOutputs.push_back( playbackPort );
#ifdef NEW_AUDIO_CONNECTION
      std::copy( playbackPort->indices(), playbackPort->indices( )+playbackPort->width(), &mPlaybackIndices[playbackIndexOffset] );
#endif
      playbackIndexOffset += playbackPort->width();
    }

    // This actual requires a back link from the implementation object to the actual (subclassed) atom.
    AtomicComponent * atom = static_cast<AtomicComponent *>( &(mFlow.component()) );
    mProcessingSchedule.clear();
    mProcessingSchedule.push_back( atom );
  } 
#endif
  return result;
}

} // namespace rrl
} // namespace visr
