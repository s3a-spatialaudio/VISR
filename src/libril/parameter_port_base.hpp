/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_PORT_BASE_HPP_INCLUDED
#define VISR_PARAMETER_PORT_BASE_HPP_INCLUDED

#include "port_base.hpp"

#include "export_symbols.hpp"
#include "parameter_type.hpp"
#include "communication_protocol_type.hpp"
 
#include <memory>
#include <string>

namespace visr
{

// Forward declarations
class Component;
enum class ParameterType;
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

  explicit ParameterPortBase( std::string const & name,
                              Component & parent,
                              PortBase::Direction direction );

  /**
   * @ TODO: Do we intend to use parameter ports in a virtual way? Obviously yes.
   */
  virtual ~ParameterPortBase();

  virtual ParameterType parameterType() const = 0;

  virtual CommunicationProtocolType protocolType() const = 0;

  virtual ParameterConfigBase const & parameterConfig() const = 0;

#if 0
  /**
   * @note Done elsewhere at th moment. 
   */
  void connectProtocol( CommunicationProtocolBase * protocol );
#endif
  /**
   * Check whether the port is connected to a valid protocol.
   */
  virtual bool isConnected() const = 0;

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
  std::unique_ptr<impl::ParameterPortBaseImplementation> mImpl;
};

} // namespace visr

#endif // #ifndef VISR_PARAMETER_PORT_BASE_HPP_INCLUDED
