/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_connection_descriptor.hpp"

namespace visr
{
namespace impl
{

AudioConnection::
AudioConnection( AudioPortBase * pSender,
                 ChannelList const & pSendIndices,
                 AudioPortBase * pReceiver,
                 ChannelList const & pReceiveIndices )
 : mSender(pSender)
 , mReceiver(pReceiver)
 , mSendIndices( pSendIndices )
 , mReceiveIndices( pReceiveIndices )
{
}

#if 0
AudioConnection::
AudioConnection( std::string const & pSendComponent,
                     std::string const & pSendPort,
                     AudioChannelIndexVector const & pSendIndices,
                     std::string const & pReceiveComponent,
                     std::string const & pReceivePort,
                     AudioChannelIndexVector const & pReceiveIndices )
 : AudioConnection( AudioPortDescriptor( pSendComponent, pSendPort ),
                    pSendIndices,
                    AudioPortDescriptor( pReceiveComponent, pReceivePort),
                    pReceiveIndices )
{
}
#endif

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

} // namespace ril
} // namespace visr
