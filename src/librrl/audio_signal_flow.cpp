/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_signal_flow.hpp"

#include "audio_connection_map.hpp"
#include "communication_area.hpp"
#include "integrity_checking.hpp"
#include "parameter_connection_graph.hpp"
#include "parameter_connection_map.hpp"
#include "port_utilities.hpp"
#include "scheduling_graph.hpp"

#include <libril/atomic_component.hpp>
#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_factory.hpp>
#include <libril/communication_protocol_type.hpp>

#include <libvisr_impl/audio_connection_descriptor.hpp>
#include <libvisr_impl/audio_port_base_implementation.hpp>
#include <libvisr_impl/component_impl.hpp>
#include <libvisr_impl/composite_component_implementation.hpp>
#include <libvisr_impl/parameter_port_base_implementation.hpp>

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

// todo: make this method protected?
void AudioSignalFlow::initCommArea( std::size_t numberOfSignals, std::size_t signalLength,
                                    std::size_t alignmentElements /* = cVectorAlignmentSamples */ )
{
  mCommArea.reset( new rrl::CommunicationArea<SampleType>( numberOfSignals, signalLength, alignmentElements ) );
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

  // fill the capture part of the communication area.
  std::size_t const numCaptureChannels = numberOfCaptureChannels();
  for( std::size_t captureIdx( 0 ); captureIdx < numCaptureChannels; ++captureIdx )
  {
    SampleType * const destPtr = mCommArea->at( mCaptureIndices[captureIdx] );
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
    SampleType const * const srcPtr = mCommArea->at( mPlaybackIndices[playbackIdx] );
    // Note: We cannot assume an alignment as we don't know the alignment of the passed playbackSamples.
    // TODO: Add optional argument to the AudioCallback interface to signal the alignment of the input and output samples.
    efl::ErrorCode const res = efl::vectorCopy( srcPtr, playbackSamples[playbackIdx], mFlow.period(), 0 );
    if( res != efl::noError )
    {
      throw std::runtime_error( "AudioSignalFlow: Error while copying output samples samples." );
    }
  }

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

} // namespace unnamed.

bool AudioSignalFlow::initialiseAudioConnections( std::ostream & messages )
{
  mTopLevelAudioInputs.clear();
  mTopLevelAudioOutputs.clear();
  mCaptureIndices.clear();
  mPlaybackIndices.clear();
  bool result = true; // Result variable, is set to false if an error occurs.
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
  mCaptureIndices.resize( numCaptureChannels );
  std::size_t captureOffset = 0;
  for( impl::AudioPortBaseImplementation * capturePort : portLookup.externalCapturePorts() )
  {
    mTopLevelAudioInputs.push_back( capturePort );
#ifdef NEW_AUDIO_CONNECTION
    std::copy( capturePort->indices( ), capturePort->indices( ) + capturePort->width( ), &mCaptureIndices[captureOffset] );
#endif
    captureOffset += capturePort->width( );
  }

  // Allocate space for the external playback ports and indices.
  // Filling is different whether the top-level flow is composite or atomic.
  mPlaybackIndices.resize( countAudioChannels( portLookup.externalPlaybackPorts() ) );
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

  return result;
}

} // namespace rrl
} // namespace visr
