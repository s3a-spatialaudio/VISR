/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PARAMETER_PORT_BASE_HPP_INCLUDED
#define VISR_PARAMETER_PORT_BASE_HPP_INCLUDED

#include "port_base.hpp"

#include "parameter_type.hpp"
#include "communication_protocol_type.hpp"
 
#include <string>

namespace visr
{

// Forward declarations
class Component;
enum class ParameterType;
class CommunicationProtocolBase;
class ParameterConfigBase;

/**
 *
 *
 */
class ParameterPortBase: public PortBase
{
public:

  enum class Kind
  {
    Concrete,
    Placeholder
  };

  explicit ParameterPortBase( std::string const & name,
                              Component & parent,
                              Direction direction );

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
protected:
  /**
   * Type-specific method to check and set the connected protocol.
   * @todo Reconsider interface.
   * @throw std::invalid_argument if the protocol type does not match the concrete port type.
   * At the moment, we use RTTI as the final check.
   */
  virtual void setProtocol( CommunicationProtocolBase * protocol ) = 0;
};

} // namespace visr

#endif // #ifndef VISR_PARAMETER_PORT_BASE_HPP_INCLUDED
