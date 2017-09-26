/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED
#define VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED

#include "port_base_implementation.hpp"

#include <libvisr/export_symbols.hpp>
#include <libvisr/parameter_type.hpp>
#include <libvisr/communication_protocol_type.hpp>
 
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
  VISR_CORE_LIBRARY_SYMBOL explicit ParameterPortBaseImplementation( char const * name,
                                            ParameterPortBase& containingPort,
                                            ComponentImplementation * parent,
                                            visr::PortBase::Direction direction,
                                            ParameterType const & parameterType,
                                            CommunicationProtocolType const & protocolType,
                                            ParameterConfigBase const & parameterConfig );

  VISR_CORE_LIBRARY_SYMBOL explicit ParameterPortBaseImplementation( char const * name,
                                            ParameterPortBase& containingPort,
                                            ComponentImplementation * parent,
                                            visr::PortBase::Direction direction,
                                            ParameterType const & parameterType,
                                            CommunicationProtocolType const & protocolType );

  /**
   * @ TODO: Do we intend to use parameter ports in a virtual way? Obviously yes.
   */
  VISR_CORE_LIBRARY_SYMBOL virtual ~ParameterPortBaseImplementation();

  /**
   * Set a new parameter type configuration for this port.
   * An already existing configuration will be replaced.
   */
  VISR_CORE_LIBRARY_SYMBOL void setParameterConfig( ParameterConfigBase const & parameterConfig );

  VISR_CORE_LIBRARY_SYMBOL ParameterPortBase & containingPort();

  VISR_CORE_LIBRARY_SYMBOL ParameterPortBase const & containingPort() const;

  VISR_CORE_LIBRARY_SYMBOL ParameterType parameterType() const;

  VISR_CORE_LIBRARY_SYMBOL CommunicationProtocolType protocolType() const;

  /**
   * Query whether a parameter configuration exists (set either in the 
   * constructor or using setParameterConfig() )
   */
  VISR_CORE_LIBRARY_SYMBOL bool hasParameterConfig() const noexcept;

  /**
   * Return the paramaeter configuration object for this port.
   * @throw std::logic_error If no parameter configuration has been set.
   */
  VISR_CORE_LIBRARY_SYMBOL ParameterConfigBase const & parameterConfig() const;

protected:
  ParameterPortBase & mContainingPort;

  visr::ParameterType const mParameterType;

  visr::CommunicationProtocolType const mProtocolType;

  std::unique_ptr<visr::ParameterConfigBase> mParameterConfig;
};

} // namespace impl
} // namespace visr

#endif // #ifndef VISR_IMPL_PARAMETER_PORT_BASE_IMPLEMENTATION_HPP_INCLUDED
