/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED
#define VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED

#include "port_base_implementation.hpp"

#include <libril/export_symbols.hpp>
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
class VISR_CORE_LIBRARY_SYMBOL ParameterPortBaseImplementation: public PortBaseImplementation
{
public:
  explicit ParameterPortBaseImplementation( char const * name,
                                            ParameterPortBase& containingPort,
                                            ComponentImplementation * parent,
                                            visr::PortBase::Direction direction,
                                            ParameterType const & parameterType,
                                            CommunicationProtocolType const & protocolType,
                                            ParameterConfigBase const & parameterConfig );

  explicit ParameterPortBaseImplementation( char const * name,
                                            ParameterPortBase& containingPort,
                                            ComponentImplementation * parent,
                                            visr::PortBase::Direction direction,
                                            ParameterType const & parameterType,
                                            CommunicationProtocolType const & protocolType );

  /**
   * @ TODO: Do we intend to use parameter ports in a virtual way? Obviously yes.
   */
  virtual ~ParameterPortBaseImplementation();

  /**
   * Set a new parameter type configuration for this port.
   * An already existing configuration will be replaced.
   */
  void setParameterConfig( ParameterConfigBase const & parameterConfig );

  ParameterPortBase & containingPort();

  ParameterPortBase const & containingPort() const;

  ParameterType parameterType() const;

  CommunicationProtocolType protocolType() const;

  /**
   * Query whether a parameter configuration exists (set either in the 
   * constructor or using setParameterConfig() )
   */
  bool hasParameterConfig() const noexcept;

  /**
   * Return the paramaeter configuration object for this port.
   * @throw std::logic_error If no parameter configuration has been set.
   */
  ParameterConfigBase const & parameterConfig() const;

protected:
  ParameterPortBase & mContainingPort;

  visr::ParameterType const mParameterType;

  visr::CommunicationProtocolType const mProtocolType;

  std::unique_ptr<visr::ParameterConfigBase> mParameterConfig;
};

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED
