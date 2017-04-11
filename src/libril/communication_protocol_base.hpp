/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED
#define VISR_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED

#include "export_symbols.hpp"

#include "communication_protocol_type.hpp"
#include "parameter_type.hpp"

namespace visr
{

// Forward declarations
class ParameterPortBase;

/**
 * Abstract base class for communication communication protocols which define the 
 * semantics of data transmissions between parameter ports.
 */
class CommunicationProtocolBase
{
public:
  // Forward declarations
  class Input;
  class Output;

  /**
   * Destructor, virtual.
   * Communication protocols are instantiated and used polymophically, so the desctructor needs to be virtual.
   */
  virtual  VISR_CORE_LIBRARY_SYMBOL ~CommunicationProtocolBase();

  /**
   * Return the protocol type (a numerical id) of the concrete derived protocol object.
   * Derived protocol types are required to override this method.
   */
  virtual CommunicationProtocolType protocolType() const = 0;

  /**
   * Return the configured parameter type of this protocol instance.
   * Derived protocols are requirted to override this method.
   */
  virtual ParameterType parameterType() const = 0;

  /**
   * Connect an input port to this protocol.
   * Derived protocol types must override this pure virtual interface.
   * @param port An parameter input with compatible protocol and parameter types.
   * @throw std::exception If the protocol or parameter types do not match.
   * @throw std::exception If the connection would violate the "arity" of the protocol, i.e.,
   * attempting multiple inputs in case of a 1:1 or 1:N protocol.
   */
  virtual void connectInput( Input* port ) = 0;

  /**
   * Connect an output port to this protocol.
   * Derived protocol types must override this pure virtual interface.
   * @param port An parameter output with compatible protocol and parameter types.
   * @throw std::exception If the protocol or parameter types do not match.
   * @throw std::exception If the connection would violate the "arity" of the protocol, i.e.,
   * attempting multiple outputs in case of a 1:1 or N:1 protocol.
   */
  virtual void connectOutput( Output* port ) = 0;

  /**
   */
  virtual bool disconnectInput( Input* port ) noexcept = 0;

  virtual bool disconnectOutput( Output* port ) noexcept= 0;

  class Input
  {
  public:
    virtual VISR_CORE_LIBRARY_SYMBOL ~Input();

    virtual void setProtocolInstance( CommunicationProtocolBase * protocol ) = 0;

    virtual CommunicationProtocolBase * getProtocol() = 0;

    virtual CommunicationProtocolBase const * getProtocol() const = 0;
  };

  class Output
  {
  public:
    virtual VISR_CORE_LIBRARY_SYMBOL ~Output();

    virtual void setProtocolInstance( CommunicationProtocolBase * protocol ) = 0;

    virtual CommunicationProtocolBase * getProtocol() = 0;

    virtual CommunicationProtocolBase const * getProtocol() const = 0;
  };

};

} // namespace visr

#endif // #ifndef VISR_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED
