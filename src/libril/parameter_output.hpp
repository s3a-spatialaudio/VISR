/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_OUTPUT_HPP_INCLUDED
#define VISR_PARAMETER_OUTPUT_HPP_INCLUDED

#include "parameter_port_base.hpp"

#include "communication_protocol_base.hpp"
#include "communication_protocol_type.hpp"
#include "export_symbols.hpp"
#include "parameter_type.hpp"

#include <ciso646>
#include <stdexcept>

namespace visr
{

class VISR_CORE_LIBRARY_SYMBOL ParameterOutputBase: public ParameterPortBase
{
public:
  explicit ParameterOutputBase( char const * name,
                                Component & parent,
                                ParameterType const & parameterType,
                                CommunicationProtocolType const & protocolType,
                                ParameterConfigBase const & paramConfig );

  explicit ParameterOutputBase( char const * name,
                                Component & parent,
                                ParameterType const & parameterType,
                                CommunicationProtocolType const & protocolType );

  virtual ~ParameterOutputBase() override;

  virtual CommunicationProtocolBase::Output & protocolOutput() = 0;

  using ParameterPortBase::setParameterConfig;
};

/**
 *
 *
 */
template< class ProtocolT, class ParameterT >
class ParameterOutput: public ParameterOutputBase,
                       public ProtocolT::template Output<ParameterT>
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  template<typename ... ProtocolArgs>
  explicit ParameterOutput( char const * name, 
                            Component & parent,
                            ParameterConfigType const & paramConfig,
                            ProtocolArgs ... protoArgs );

  template<typename ... ProtocolArgs>
  explicit ParameterOutput( char const * name,
                            Component & parent,
                            ProtocolArgs ... protoArgs );


  /**
   *
   */
  /*virtual*/ ~ParameterOutput();

  CommunicationProtocolBase::Output & protocolOutput() override
  {
    return *this; // cast to protocol input type.
  };

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

  /**
  * Set the parameter configuration for this port.
  * This method can be called during initialisation. It replaces a previous configuration.
  * @throw std::logic_error if the method is called after the parameter infrastracture is initialised.
  */
  void setParameterConfig( ParameterConfigType const & parameterConfig )
  {
    ParameterOutputBase::setParameterConfig( parameterConfig );
  }

protected:
  void setProtocol( CommunicationProtocolBase * protocol ) override
  {
    ProtocolT * typedProtocol
      = dynamic_cast< ProtocolT * >(protocol);
    if( not typedProtocol )
    {
      throw std::invalid_argument( "ParameterOutput::setProtocol(): Protocol class type does not match the protocol type for this port." );
    }
    this->setProtocolInstance( typedProtocol );
  }
private:
};

template< class ProtocolT, class ParameterT >
template<typename ... ProtocolArgs>
inline ParameterOutput<ProtocolT, ParameterT >::
ParameterOutput( char const * name, 
                 Component & parent,
                 ParameterConfigType const & paramConfig,
                 ProtocolArgs ... protoArgs )
  : ParameterOutputBase( name, parent,
                         ParameterToId<ParameterT>::id,
                         CommunicationProtocolToId<ProtocolT>::id,
                         paramConfig )
  , ProtocolT::template Output<ParameterT>(protoArgs...)
{
}

template< class ProtocolT, class ParameterT >
template<typename ... ProtocolArgs>
inline ParameterOutput<ProtocolT, ParameterT >::
ParameterOutput( char const * name,
                 Component & parent,
                 ProtocolArgs ... protoArgs )
  : ParameterOutputBase( name, parent,
    ParameterToId<ParameterT>::id,
    CommunicationProtocolToId<ProtocolT>::id )
  , ProtocolT::template Output<ParameterT>( protoArgs... )
{
}

template<class ProtocolT, class ParameterT >
inline ParameterOutput<ProtocolT, ParameterT >::~ParameterOutput( )
{
}

} // namespace visr

#endif // #ifndef VISR_PARAMETER_OUTPUT_HPP_INCLUDED
