/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_PORT_BASE_HPP_INCLUDED
#define VISR_PARAMETER_PORT_BASE_HPP_INCLUDED

#include "port_base.hpp"

#include "export_symbols.hpp"
#include "parameter_type.hpp"
#include "communication_protocol_type.hpp"

namespace visr
{

// Forward declarations
class Component;
class CommunicationProtocolBase;
class ParameterConfigBase;

namespace impl
{
class ParameterPortBaseImplementation;
}

/**
 *
 *
 */
class  VISR_CORE_LIBRARY_SYMBOL ParameterPortBase
{
public:
  /**
   * Temporary solution, as PortBase is going to vanish.
   * @ todo: Decide whether we need a global enumerarion for port directions,
   * or whether the 'direction' parameter can be hidden from the user API.
   */
  using Direction = PortBase::Direction;

  explicit ParameterPortBase( char const * name,
                              Component & parent,
                              PortBase::Direction direction,
                              ParameterType const & parameterType,
                              CommunicationProtocolType const & protocolType,
                              ParameterConfigBase const & parameterConfig );

  ParameterPortBase( char const * name,
                     Component & parent,
                     PortBase::Direction direction,
                     ParameterType const & parameterType,
                     CommunicationProtocolType const & protocolType );

  /**
   * Deleted copy constructor to prevent copy construction.
   */
  ParameterPortBase( ParameterPortBase const & ) = delete;

  /**
   * Deleted move constructor to prevent copy construction.
   */
  ParameterPortBase( ParameterPortBase && ) = delete;

  /**
   * Deleted assignment operator to prevent assignment.
   */
  ParameterPortBase& operator=( ParameterPortBase const & ) = delete;

  /**
   * Deleted move assignment operator to prevent moving.
   */
  ParameterPortBase& operator=( ParameterPortBase && ) = delete;


  /**
  * @ TODO: Do we intend to use parameter ports in a virtual way? Obviously yes.
  */
  virtual ~ParameterPortBase();

  /**
   * Set a new parameter configuration. If a configuration is already set, it is overwritten.
   * Must be called only during the initialisation phase
   * @param parameterConfig The new parameter configuration.
   * @throw std::runtime_error When called after the signal flow is initialised.
   */
  void setParameterConfig( ParameterConfigBase const & parameterConfig );


  ParameterType parameterType() const;

  CommunicationProtocolType protocolType() const;

  ParameterConfigBase const & parameterConfig() const;

#if 0
  /**
   * @note Done elsewhere at the moment. 
   */
  void connectProtocol( CommunicationProtocolBase * protocol );
#endif
  /**
   * Check whether the port is connected to a valid protocol.
   */
  // virtual bool isConnected() const = 0;

  impl::ParameterPortBaseImplementation & implementation();

  impl::ParameterPortBaseImplementation const & implementation() const;
protected:
  /**
   * Type-specific method to check and set the connected protocol.
   * @todo Reconsider interface.
   * @throw std::invalid_argument if the protocol type does not match the concrete port type.
   * At the moment, we use RTTI as the final check.
   */
  virtual void setProtocol( CommunicationProtocolBase * protocol ) = 0;

private:

  /**
   * Opaque pointer to the implementation object.
   * @note This is deliberately a plain, not a smart, pointer in order to make it independent of the 
   * possible implementations by the runtime system.
   */
  impl::ParameterPortBaseImplementation* mImpl;
};

} // namespace visr

#endif // #ifndef VISR_PARAMETER_PORT_BASE_HPP_INCLUDED
