/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_OUTPUT_HPP_INCLUDED
#define VISR_PARAMETER_OUTPUT_HPP_INCLUDED

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
class ParameterOutput: public ParameterPortBase,
                           public ProtocolT::template Output<ParameterT>
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  template<typename ... ProtocolArgs>
  explicit ParameterOutput( std::string const & name, 
                                Component & parent,
                                ParameterConfigType const & paramConfig,
                                ProtocolArgs ... protoArgs );

  /**
   *
   */
  /*virtual*/ ~ParameterOutput();

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
inline ParameterOutput<ProtocolT, ParameterT >::
ParameterOutput( std::string const & name, 
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
inline ParameterOutput<ProtocolT, ParameterT >::~ParameterOutput( )
{
}

} // namespace visr

#endif // #ifndef VISR_PARAMETER_OUTPUT_HPP_INCLUDED
