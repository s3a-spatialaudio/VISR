/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parameter_port_base.hpp"

#include "component.hpp"
#include "communication_protocol_base.hpp"
#include "communication_protocol_type.hpp"
#include "parameter_config_base.hpp"

#include <ciso646>

namespace visr
{
namespace ril
{

ParameterPortBase::ParameterPortBase( Component & parent,
                                      std::string const & name,
                                       Direction direction )
  : mDirection( direction )
{
  parent.registerParameterPort( this, name );
}

ParameterPortBase::~ParameterPortBase( ) 
{
  // todo: we need a reference to the containing component to unregister ourselves.
}

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
  // TODO: Shall we check the parameter configuration? This would mean we have to check them in the protocol.

  // Call the specific registration function of the derived port type.
  setProtocol( protocol );
}

} // namespace ril
} // namespace visr
