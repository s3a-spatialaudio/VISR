/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_INPUT_PORT_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_INPUT_PORT_HPP_INCLUDED

#include "parameter_port_base.hpp"

#include "communication_protocol_type.hpp"
#include "parameter_type.hpp"

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
class ParameterInputPort: public ParameterPortBase, // Maybe make this protected or private
                          public ProtocolT<ParameterT>::Input
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  explicit ParameterInputPort( Component & parent,
                              std::string const & name,
                              Kind kind,
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
    return CommunicationProtocolToId<ProtocolT, ParameterT>::id;
  }

  ParameterConfigBase const & parameterConfig() const override
  {
    return mConfig;
  }

  ParameterConfigType const mConfig;
};

template< template<class> class ProtocolT, class ParameterT >
inline ParameterInputPort<ProtocolT, ParameterT >::
ParameterInputPort( Component & parent,
                           std::string const & name,
                           Kind kind,
                           ParameterConfigType const & paramConfig )
  : ParameterPortBase( parent, name, ParameterPortBase::Direction::Input, kind )
  , mConfig( paramConfig )
{
}

template< template<class> class ProtocolT, class ParameterT >
inline ParameterInputPort<ProtocolT, ParameterT >::~ParameterInputPort( )
{
}

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_INPUT_PORT_HPP_INCLUDED
