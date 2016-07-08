/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_PORT_BASE_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_PORT_BASE_HPP_INCLUDED

#include "parameter_type.hpp"
#include "communication_protocol_type.hpp"
 
#include <string>

namespace visr
{
namespace ril
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
class ParameterPortBase
{
public:

  enum class Kind
  {
    Concrete,
    Placeholder
  };

  enum class Direction
  {
    Input,
    Output
  };

  explicit ParameterPortBase( Component & parent,
                              std::string const & name,
                              Direction direction );

  /**
   *
   */
  virtual ~ParameterPortBase();

  virtual ParameterType parameterType() const = 0;

  virtual CommunicationProtocolType protocolType() const = 0;

  virtual ParameterConfigBase const & parameterConfig() const = 0;

  void connectProtocol( ril::CommunicationProtocolBase * protocol );

protected:
  /**
   * Type-specific method to check and set the connected protocol.
   * @todo Reconsider interface.
   * @throw std::invalid_argument if the protovol type does not match the concrete port type.
   * At the moment, we use RTTI as the final check.
   */
  virtual void setProtocol( ril::CommunicationProtocolBase * protocol ) = 0;
private:
  Direction const mDirection;

  // Kind const mKind;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_PORT_BASE_HPP_INCLUDED
