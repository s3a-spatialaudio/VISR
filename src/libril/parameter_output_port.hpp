/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_OUTPUT_PORT_HPP_INCLUDED
#define VISR_PARAMETER_OUTPUT_PORT_HPP_INCLUDED

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
class ParameterOutputPort: public ParameterPortBase,
                           public ProtocolT::template Output<ParameterT>
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  template<typename ... ProtocolArgs>
  explicit ParameterOutputPort( std::string const & name, 
                                Component & parent,
                                ParameterConfigType const & paramConfig,
                                ProtocolArgs ... protoArgs );

  /**
   *
   */
  /*virtual*/ ~ParameterOutputPort();

  ParameterType parameterType() const
  {
    return ParameterToId<ParameterT>::id;
  }

  CommunicationProtocolType protocolType() const
  {
    return CommunicationProtocolToId<ProtocolT>::id;
  }

  ParameterConfigBase const & parameterConfig() const
  {
    return ParameterPortBase::parameterConfig();
  }

protected:
  void setProtocol( CommunicationProtocolBase * protocol )
  {
    ProtocolT * typedProtocol
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
inline ParameterOutputPort<ProtocolT, ParameterT >::
ParameterOutputPort( std::string const & name, 
                     Component & parent,
                     ParameterConfigType const & paramConfig,
                     ProtocolArgs ... protoArgs )
  : ParameterPortBase( name, parent, PortBase::Direction::Output, 
                       ParameterToId<ParameterT>::id,
                       CommunicationProtocolToId<ProtocolT>::id,
                       paramConfig )
  , ProtocolT::template Output<ParameterT>(protoArgs...)
{
}

template<class ProtocolT, class ParameterT >
inline ParameterOutputPort<ProtocolT, ParameterT >::~ParameterOutputPort( )
{
}

} // namespace visr

#endif // #ifndef VISR_PARAMETER_OUTPUT_PORT_HPP_INCLUDED
