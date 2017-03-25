/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_INPUT_PORT_HPP_INCLUDED
#define VISR_PARAMETER_INPUT_PORT_HPP_INCLUDED

#include "parameter_port_base.hpp"

#include "communication_protocol_type.hpp"
#include "parameter_type.hpp"

#include <ciso646>
#include <stdexcept>
#include <string>

namespace visr
{

/**
 *
 *
 */
template< class ProtocolT, class ParameterT >
class ParameterInputPort: public ParameterPortBase,
                          public ProtocolT::template Input< ParameterT >
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  explicit ParameterInputPort( std::string const & name,
                               Component & parent,
                               ParameterConfigType const & paramConfig );

  /**
   *
   */
  /*virtual*/ ~ParameterInputPort() override;

  ParameterType parameterType() const override
  {
    return ParameterToId<ParameterT>::id;
  }

  CommunicationProtocolType protocolType() const override
  {
    return CommunicationProtocolToId<ProtocolT>::id;
  }

  ParameterConfigBase const & parameterConfig() const override
  {
    return ParameterPortBase::parameterConfig();
  }
protected:
  void setProtocol( CommunicationProtocolBase * protocol ) override
  {
    ProtocolT* typedProtocol
      = dynamic_cast< ProtocolT * >(protocol);
    if( not typedProtocol )
    {
      throw std::invalid_argument( "MessageQueueProtocol::MessageQueueProtocol::Input::setProtocol(): Protocol class type does not match" );
    }
    this->setProtocolInstance( typedProtocol );
  }
private:
};

template< class ProtocolT, class ParameterT >
inline ParameterInputPort<ProtocolT, ParameterT >::
ParameterInputPort( std::string const & name,
  Component & parent,
  ParameterConfigType const & paramConfig )
  : ParameterPortBase( name, parent, PortBase::Direction::Input,
    ParameterToId<ParameterT>::id,
    CommunicationProtocolToId<ProtocolT>::id,
    paramConfig )
  , ProtocolT::template Input<ParameterT>()
{
}

template< class ProtocolT, class ParameterT >
inline ParameterInputPort<ProtocolT, ParameterT >::~ParameterInputPort( ) = default;

} // namespace visr

#endif // #ifndef VISR_PARAMETER_INPUT_PORT_HPP_INCLUDED
