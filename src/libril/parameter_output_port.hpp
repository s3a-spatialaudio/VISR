/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_OUTPUT_PORT_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_OUTPUT_PORT_HPP_INCLUDED

#include "parameter_port_base.hpp"

#include "communication_protocol_type.hpp"
#include "parameter_type.hpp"

#include <ciso646>
#include <string>

namespace visr
{
namespace ril
{

/**
 *
 *
 */
template< template<class> class ProtocolT, class ParameterT >
class ParameterOutputPort: public ParameterPortBase, // Maybe make this protected or private
                          public ProtocolT<ParameterT>::Output
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  explicit ParameterOutputPort( Component & parent,
                                std::string const & name,
                                ParameterConfigType const & paramConfig );

  /**
   *
   */
  /*virtual*/ ~ParameterOutputPort() override;

  ParameterType parameterType() const override
  {
    return ParameterToId<ParameterT>::id;
  }

  CommunicationProtocolType protocolType() const override
  {
    return CommunicationProtocolToId<ProtocolT, ParameterT>::id;
  }

  ParameterConfigBase const & parameterConfig() const override
  {
    return mConfig;
  }

protected:
  void setProtocol( ril::CommunicationProtocolBase * protocol ) override
  {
    ProtocolT<ParameterT> * typedProtocol
      = dynamic_cast< ProtocolT<ParameterT> * >(protocol);
    if( not typedProtocol )
    {
      throw std::invalid_argument( "MessageQueueProtocol::MessageQueueProtocol::Input::setProtocol(): Protocol class type does not match" );
    }
    this->setProtocolInstance( typedProtocol );
  }
private:
  ParameterConfigType const mConfig;
};

template< template<class> class ProtocolT, class ParameterT >
inline ParameterOutputPort<ProtocolT, ParameterT >::
ParameterOutputPort( Component & parent,
                     std::string const & name,
                     ParameterConfigType const & paramConfig )
  : ParameterPortBase( parent, name, ParameterPortBase::Direction::Output )
  , mConfig( paramConfig )
{
}

template< template<class> class ProtocolT, class ParameterT >
inline ParameterOutputPort<ProtocolT, ParameterT >::~ParameterOutputPort( )
{
}

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_OUTPUT_PORT_HPP_INCLUDED
