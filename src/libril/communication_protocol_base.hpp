/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED
#define VISR_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED

#include "export_symbols.hpp"

#include "parameter_type.hpp"

namespace visr
{

// Forward declarations
enum class CommunicationProtocolType;
class ParameterPortBase;

/**
 *
 *
 */
class  VISR_CORE_LIBRARY_SYMBOL CommunicationProtocolBase
{
public:

  CommunicationProtocolBase();

  /**
   *
   */
  virtual ~CommunicationProtocolBase();

  virtual CommunicationProtocolType protocolType() const = 0;

  virtual ParameterType parameterType() const = 0;

  virtual void connectInput( ParameterPortBase* port ) = 0;

  virtual void connectOutput( ParameterPortBase* port ) = 0;

  virtual bool disconnectInput( ParameterPortBase* port ) = 0;

  virtual bool disconnectOutput( ParameterPortBase* port ) = 0;
};

} // namespace visr

#endif // #ifndef VISR_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED
