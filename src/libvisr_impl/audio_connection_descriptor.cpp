/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_connection_descriptor.hpp"

#include <libvisr_impl/audio_port_base_implementation.hpp>

#include <ostream>

namespace visr
{
namespace impl
{

AudioConnection::
AudioConnection( AudioPortBaseImplementation * pSender,
                 ChannelList const & pSendIndices,
                 AudioPortBaseImplementation * pReceiver,
                 ChannelList const & pReceiveIndices )
 : mSender(pSender)
 , mReceiver(pReceiver)
 , mSendIndices( pSendIndices )
 , mReceiveIndices( pReceiveIndices )
{
}

bool AudioConnection::operator<(AudioConnection const & rhs) const
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

std::ostream & operator<<( std::ostream & str, impl::AudioConnection const & conn )
{
  // todo: replace by name function that includes the component.
  str << conn.sender()->name() << "->" << conn.receiver()->name() << std::endl;
  return str;
}


std::ostream & operator<<( std::ostream & str, impl::AudioConnectionTable const & table )
{
  for( auto const & e : table )
  {
    str << e << std::endl;
  }
  return str;
}

} // namespace ril
} // namespace visr
