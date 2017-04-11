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
   * Communication protocols are instantiated and used polymophically, so the destructor needs to be virtual.
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
   * Connect a protocol input to this protocol.
   * Derived protocol types must override this pure virtual interface.
   * @param port An parameter input with compatible protocol and parameter types.
   * @throw std::exception If the protocol or parameter types do not match.
   * @throw std::exception If the connection would violate the "arity" of the protocol, i.e.,
   * attempting multiple inputs in case of a 1:1 or 1:N protocol.
   */
  virtual void connectInput( Input* port ) = 0;

  /**
   * Connect a protocol output to this protocol.
   * Derived protocol types must override this pure virtual interface.
   * @param port An parameter output with compatible protocol and parameter types.
   * @throw std::exception If the protocol or parameter types do not match.
   * @throw std::exception If the connection would violate the "arity" of the protocol, i.e.,
   * attempting multiple outputs in case of a 1:1 or N:1 protocol.
   */
  virtual void connectOutput( Output* port ) = 0;

  /**
   * Disconnect a protocol input from this protocol.
   * @return true if the disconnect was successful, i.e., if the protocol port was actually connected,
   * false otherwise.
   */
  virtual bool disconnectInput( Input* port ) noexcept = 0;

  /**
  * Disconnect a protocol output from this protocol.
  * @return true if the disconnect was successful, i.e., if the protocol port was actually connected,
  * false otherwise.
  */
  virtual bool disconnectOutput( Output* port ) noexcept= 0;
};

/**
 * Abstract interface for an input to a protocol.
 * Communication protocols derived from CommunicationProtocolBase must define classes derived from this base interface,
 * implementing its pure virtual methods
 */
class CommunicationProtocolBase::Input
{
public:
  /**
   * Virtual destructor.
   * This class is intended to be instantiated and used polymorphically.
   */
  virtual VISR_CORE_LIBRARY_SYMBOL ~Input();

  /**
   * Connect the input to protocol instance.
   */
  virtual void setProtocolInstance( CommunicationProtocolBase * protocol ) = 0;

  /**
   * Return a pointer to the connected protocol, or \p nullptr if it is not connected.
   */
  virtual CommunicationProtocolBase * getProtocol() = 0;

  /**
  * Return a pointer to the connected protocol, const version. If the input is not connected, return \p nullptr.
  */
  virtual CommunicationProtocolBase const * getProtocol() const = 0;
};

/**
 * Abstract base class for communication protocol inputs.
 * Communication protocols derived from CommunicationProtocolBase must provide a Input class derived from this base 
 * class and override its pure virtual methods.
 */
class CommunicationProtocolBase::Output
{
public:
  /**
   * Virtual destructor.
   * Derived classes are instantiated and used polymorphically, therefore the destructor has to be virtual.
   */
  virtual VISR_CORE_LIBRARY_SYMBOL ~Output();

  /**
   * Set the protocol instance for this protocol input.
   */
  virtual void setProtocolInstance( CommunicationProtocolBase * protocol ) = 0;

  /**
   * Return a pointer to the connected communication protocol, or \p nullptr if it is not connected.
   */
  virtual CommunicationProtocolBase * getProtocol() = 0;

  /**
  * Return a pointer to the connected communication protocol (const version),
  * or \p nullptr if it is not connected.
  */
  virtual CommunicationProtocolBase const * getProtocol() const = 0;
};

} // namespace visr

#endif // #ifndef VISR_COMMUNICATION_PROTOCOL_BASE_HPP_INCLUDED
