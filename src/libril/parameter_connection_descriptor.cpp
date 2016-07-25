/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_connection_descriptor.hpp"


namespace visr
{
namespace ril
{

ParameterPortDescriptor::
ParameterPortDescriptor(std::string const & pComponent, std::string const & pPort)
 : mComponent( pComponent)
 , mPort(pPort)
{
}

bool ParameterPortDescriptor::
operator<(ParameterPortDescriptor const & rhs) const
{
  if (component() < rhs.component())
  {
    return true;
  }
  else if( rhs.component() < component() )
  {
    return false;
  }
  else return port() < rhs.port();
}

// Not used in the current code.
#if 0
ParameterConnection::
ParameterConnection( ParameterPortDescriptor const & pSender,
                     ParameterPortDescriptor const & pReceiver)
 : mSender(pSender)
 , mReceiver(pReceiver)
{
}

ParameterConnection::
ParameterConnection( std::string const & pSendComponent,
                     std::string const & pSendPort,
                     std::string const & pReceiveComponent,
                     std::string const & pReceivePort)
 : ParameterConnection( ParameterPortDescriptor( pSendComponent, pSendPort ),
	                    ParameterPortDescriptor( pReceiveComponent, pReceivePort) )
{
}

bool ParameterConnection::operator<(ParameterConnection const & rhs) const
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
#endif


} // namespace ril
} // namespace visr
