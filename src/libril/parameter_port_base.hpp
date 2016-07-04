/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_PARAMETER_PORT_BASE_HPP_INCLUDED
#define VISR_LIBRIL_PARAMETER_PORT_BASE_HPP_INCLUDED

#include "parameter_type.hpp"
#include "Communication_protocol_type.hpp"
 
#include <string>

namespace visr
{
namespace ril
{

// Forward declarations
class Component;
enum class ParameterType;
// enum class CommunicationProtocolType;
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
                              Direction direction,
                              Kind kind );

  /**
   *
   */
  virtual ~ParameterPortBase();

  virtual ParameterType parameterType() = 0;

  virtual CommunicationProtocolType protocolType() = 0;

  virtual ParameterConfigBase const & parameterConfig() = 0;

private:
  Direction const mDirection;

  Kind const mKind;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_PARAMETER_PORT_BASE_HPP_INCLUDED
