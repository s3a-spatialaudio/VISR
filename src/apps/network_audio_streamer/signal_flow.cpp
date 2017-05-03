/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

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

SignalFlow::SignalFlow( SignalFlowContext const & context,
                        char const * name,
                        CompositeComponent * parent,
                        std::string const & sendAddresses )
 : CompositeComponent( context, name, parent )
 , mAudioInput( "audioIn", *this )
 , mEncoder( context, "Encoder", this )
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
    std::unique_ptr<rcl::UdpSender> newSender( new rcl::UdpSender( context, nameStr.str().c_str(), this ) );
    newSender->setup( 0, hostName, port, rcl::UdpSender::Mode::Asynchronous ); // '0' means that an available port is used at the local endpoint.

    std::stringstream portNameStr;
    portNameStr << "msgOut" << idx;
    parameterConnection( mEncoder.parameterPort(portNameStr.str().c_str()), newSender->parameterPort( "msgIn" ) );

    mSenders.push_back( std::move(newSender) );
  }
  mEncoder.setup( numSenders, period() );
  mAudioInput.setWidth( numSenders );
  audioConnection( mAudioInput, mEncoder.audioPort("input") );
}

SignalFlow::~SignalFlow( )
{
}
 
} // namespace audio_network_streamer
} // namespace apps
} // namespace visr
