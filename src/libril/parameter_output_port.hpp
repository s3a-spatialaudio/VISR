/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_OUTPUT_PORT_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_OUTPUT_PORT_HPP_INCLUDED

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
class ParameterOutputPort: public ParameterPortBase, // Maybe make this protected or private
                          public ProtocolT<ParameterT>::Input
{
public:
  using ParameterConfigType = typename ParameterToConfigType<ParameterT>::ConfigType;

  explicit ParameterOutputPort( Component & parent,
                              std::string const & name,
                              Kind kind,
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

  ParameterConfigType const mConfig;
};

template< template<class> class ProtocolT, class ParameterT >
inline ParameterOutputPort<ProtocolT, ParameterT >::
ParameterOutputPort( Component & parent,
                           std::string const & name,
                           Kind kind,
                           ParameterConfigType const & paramConfig )
  : ParameterPortBase( parent, name, ParameterPortBase::Direction::Output, kind )
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
