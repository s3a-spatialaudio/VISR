/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_INPUT_HPP_INCLUDED
#define VISR_PARAMETER_INPUT_HPP_INCLUDED

#include "parameter_port_base.hpp"

#include "communication_protocol_type.hpp"
#include "export_symbols.hpp"
#include "parameter_type.hpp"

#include <ciso646>
#include <stdexcept>

namespace visr
{

class VISR_CORE_LIBRARY_SYMBOL ParameterInputBase: public ParameterPortBase
{
protected:
  explicit ParameterInputBase( char const * name,
                               Component & parent,
                               ParameterType const & parameterType,
                               CommunicationProtocolType const & protocolType,
                               ParameterConfigBase const & paramConfig );

  explicit ParameterInputBase( char const * name,
                               Component & parent,
                               ParameterType const & parameterType,
                               CommunicationProtocolType const & protocolType );

  virtual ~ParameterInputBase() override;

  using ParameterPortBase::setParameterConfig;
};

/**
 *
 *
 */
template< class ProtocolT, class ParameterT >
class ParameterInput: public ParameterInputBase,
                          public ProtocolT::template Input< ParameterT >
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  template<typename ... ProtocolArgs>
  explicit ParameterInput( char const * name,
                           Component & parent,
                           ParameterConfigType const & paramConfig,
                           ProtocolArgs ... protoArgs );

  template<typename ... ProtocolArgs>
  explicit ParameterInput( char const * name,
                           Component & parent,
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
ParameterInput( char const * name,
  Component & parent,
  ParameterConfigType const & paramConfig,
  ProtocolArgs ... protoArgs )
  : ParameterInputBase( name, parent,
    ParameterToId<ParameterT>::id,
    CommunicationProtocolToId<ProtocolT>::id, paramConfig )
  , ProtocolT::template Input<ParameterT>(protoArgs...)
{
}

template< class ProtocolT, class ParameterT >
template<typename ... ProtocolArgs>
inline ParameterInput<ProtocolT, ParameterT >::
ParameterInput( char const * name,
                Component & parent,
                ProtocolArgs ... protoArgs )
  : ParameterInputBase( name, parent,
    ParameterToId<ParameterT>::id,
    CommunicationProtocolToId<ProtocolT>::id )
  , ProtocolT::template Input<ParameterT>( protoArgs... )
{
}

template< class ProtocolT, class ParameterT >
inline ParameterInput<ProtocolT, ParameterT >::~ParameterInput( ) = default;

} // namespace visr

#endif // #ifndef VISR_PARAMETER_INPUT_HPP_INCLUDED
