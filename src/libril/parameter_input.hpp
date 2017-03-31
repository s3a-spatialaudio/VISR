/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_INPUT_HPP_INCLUDED
#define VISR_PARAMETER_INPUT_HPP_INCLUDED

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
class ParameterInput: public ParameterPortBase,
                          public ProtocolT::template Input< ParameterT >
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  template<typename ... ProtocolArgs>
  explicit ParameterInput( std::string const & name,
                               Component & parent,
                               ParameterConfigType const & paramConfig,
                               ProtocolArgs ... protoArgs );

  /**
   * Virtual desctructor
   */
  /*virtual*/ ~ParameterInput() override;

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
template<typename ... ProtocolArgs>
inline ParameterInput<ProtocolT, ParameterT >::
ParameterInput( std::string const & name,
  Component & parent,
  ParameterConfigType const & paramConfig,
  ProtocolArgs ... protoArgs )
  : ParameterPortBase( name, parent, PortBase::Direction::Input,
    ParameterToId<ParameterT>::id,
    CommunicationProtocolToId<ProtocolT>::id,
    paramConfig )
  , ProtocolT::template Input<ParameterT>(protoArgs...)
{
}

template< class ProtocolT, class ParameterT >
inline ParameterInput<ProtocolT, ParameterT >::~ParameterInput( ) = default;

} // namespace visr

#endif // #ifndef VISR_PARAMETER_INPUT_HPP_INCLUDED
