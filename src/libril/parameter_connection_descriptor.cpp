/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_connection_descriptor.hpp"


namespace visr
{
namespace ril
{

#if 0
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
#endif

ParameterConnection::
ParameterConnection( ParameterPortBase * pSender,
                     ParameterPortBase * pReceiver)
 : mSender(pSender)
 , mReceiver(pReceiver)
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

} // namespace ril
} // namespace visr
