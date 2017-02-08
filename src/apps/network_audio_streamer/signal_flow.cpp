/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <libpml/message_queue.hpp>

#include <librcl/udp_sender.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <array>
#include <vector>
#include <utility>

namespace visr
{
namespace apps
{
namespace audio_network_streamer
{

SignalFlow::SignalFlow( std::string const & sendAddresses, std::size_t period, ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , mEncoder( *this, "Encoder" )
{
  std::vector<std::string> addresses; 
  boost::split( addresses, sendAddresses, boost::is_any_of( "," ) );
  //  boost::trim( addresses );
  std::size_t const numSenders = addresses.size();
  for( std::size_t idx(0); idx < numSenders; ++idx )
  {
    std::string const & address = addresses[idx];
    std::vector<std::string> addressParts;
    boost::split( addressParts, address, boost::is_any_of(":") );
    if( addressParts.size() != 2 )
    {
      throw std::invalid_argument( "NetworkAudioStreamer: Each send address must have the form <host>:<port>" );
    }
    std::string const hostName =  boost::trim_copy( addressParts[0] );
    unsigned int port;
    try
    {
      port = boost::lexical_cast<unsigned int>(boost::trim_copy( addressParts[1] ));
    }
    catch( std::exception const & /*ex*/ )
    {
      throw std::invalid_argument( "NetworkAudioStreamer: Could not parse port name." );
    }

    std::stringstream nameStr;
    nameStr << "Sender_" << idx;
    std::unique_ptr<rcl::UdpSender> newSender( new rcl::UdpSender( *this, nameStr.str().c_str() ) );
    newSender->setup( port, hostName, port, rcl::UdpSender::Mode::Asynchronous );
    mSenders.push_back( std::move(newSender) );
  }

  mMessageQueues.resize( numSenders );

  // Set up communication area 
  initCommArea( numSenders, period, ril::cVectorAlignmentSamples );

  // connect the ports

  std::vector<std::size_t> idxList;
  std::size_t n(0);
  std::generate_n( std::back_inserter(idxList), numSenders, [&n]{ return n++; } );

  assignCommunicationIndices( "Encoder", "input", idxList );

  // Set the indices for communicating the signals from and to the outside world.
  assignCaptureIndices( idxList  );

  setInitialised( true );
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process()
{
  mEncoder.process( mMessageQueues );
  std::size_t numSignals = mMessageQueues.size();
  for( std::size_t idx(0); idx < numSignals; ++idx )
  {
    mSenders[idx]->process( mMessageQueues[idx] );
  }
}

} // namespace audio_network_streamer
} // namespace apps
} // namespace visr
