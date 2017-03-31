/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED
#define VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED

#include "port_base_implementation.hpp"

#include <libril/parameter_type.hpp>
#include <libril/communication_protocol_type.hpp>
 
#include <memory>
#include <string>

namespace visr
{
  // Forward declarations
  class Component;
  class CommunicationProtocolBase;
  class ParameterConfigBase;
  class ParameterPortBase;

namespace impl
{

/**
 *
 *
 */
class ParameterPortBaseImplementation: public PortBaseImplementation
{
public:
  explicit ParameterPortBaseImplementation( std::string const & name,
                                            ParameterPortBase& containingPort,
                                            ComponentImplementation * parent,
                                            visr::PortBase::Direction direction,
                                            ParameterType const & parameterType,
                                            CommunicationProtocolType const & protocolType,
                                            ParameterConfigBase const & parameterConfig );

  /**
   * @ TODO: Do we intend to use parameter ports in a virtual way? Obviously yes.
   */
  virtual ~ParameterPortBaseImplementation();

  ParameterPortBase & containingPort();

  ParameterPortBase const & containingPort() const;

  ParameterType parameterType() const;

  CommunicationProtocolType protocolType() const;

  ParameterConfigBase const & parameterConfig() const;

protected:
  ParameterPortBase & mContainingPort;

  visr::ParameterType const mParameterType;

  visr::CommunicationProtocolType const mProtocolType;

  std::unique_ptr<visr::ParameterConfigBase> const mParameterConfig;
};

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED
