/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_port_base.hpp"

#include "component.hpp"

namespace visr
{
namespace ril
{

ParameterPortBase::ParameterPortBase( std::string const & name,
                                      Component & parent, 
                                      Direction direction )
  : PortBase( name, parent, direction )
{
  parent.registerParameterPort( this );
}

ParameterPortBase::~ParameterPortBase( ) 
{
  parent().unregisterParameterPort( this );
}

#if 0
void ParameterPortBase::connectProtocol( ril::CommunicationProtocolBase * protocol )
{
  if( protocol->protocolType() != protocolType() )
  {
    throw std::logic_error( "ParameterPortBase::connectProtocol(): type of communication protocol object does not match." );
  }
  if( protocol->parameterType() != parameterType() )
  {
    throw std::logic_error( "ParameterPortBase::connectProtocol(): parameter types of port and communication protocol object do not match." );
  }
  // TODO: Shall we check the parameter configuration? This would mean we have to store them in the protocol.

  // Call the specific registration function of the derived port type.
  setProtocol( protocol );
}
#endif

} // namespace ril
} // namespace visr
