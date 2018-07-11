/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED
#define VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/communication_protocol_base.hpp>
#include <libvisr/communication_protocol_type.hpp>

#include <libvisr/parameter_port_base.hpp>
#include <libvisr/parameter_type.hpp>
#include <libvisr/parameter_config_base.hpp>

#include <ciso646>
#include <memory>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{

/**
 * Communication protocol for double-buffered parameters.
 * Parameters in output (send) ports can be modified, and the change become visible in connected input (receive) ports after a call to swapBuffers().
 * The protocol supports 1:N connections. (single send port to an arbitray number of outputs).
 */
class VISR_PML_LIBRARY_SYMBOL DoubleBufferingProtocol: public CommunicationProtocolBase
{
public:
  /**
   * Forward declarations of the internal data types.
    */
  class InputBase;
  template<class DataType > class Input;
  class OutputBase;
  template<class DataType > class Output;

  /**
   * Constructor.
   * @param parameterType The parameter type that will transmitted through the protocol.
   * @param parameterConfig The configuration of the parameter. The dynamic type must ne compatible with the parameter type.
   */
  explicit DoubleBufferingProtocol( ParameterType const & parameterType,
                                    ParameterConfigBase const & parameterConfig );

  /**
   * Destructor.
   * The destructor is virtual because the class is intended to be used polymorphically.
   */
  virtual ~DoubleBufferingProtocol() override;

  /**
   * Return the type id for the protocol.
   */
  static constexpr CommunicationProtocolType staticType() { return communicationProtocolTypeFromString(sProtocolName); };

  /**
   * Return the name of the protocol.
   */
  static constexpr char const * staticName() { return sProtocolName; }

  /**
   * Return the parameter ID for the transmitted parameter type.
   */
  ParameterType parameterType( ) const override;

  /**
   * Return the actual protocol type.
   * This function is virtual and non-static, i.e., it is typically used polymorphically through a base class pointer.
   * @see staticType for the static function.
   */
  virtual CommunicationProtocolType protocolType( ) const override { return staticType(); }

  /**
   * Return a reference to the 'front', i.e., the sending side of the buffer where data is set.
   */
  ParameterBase & frontData();

  /**
   * Return a reference to the 'front', i.e., the sending side of the buffer where data is set.
   * Const version.
   */
  ParameterBase const & frontData() const;

  /**
   * Return a reference to the 'back', i.e., receiving end of the connection where parameters are read.
   */
  ParameterBase & backData();

  /**
   * Return a reference to the 'back', i.e., receiving end of the connection where parameters are read.
   * Const version.
   */
  ParameterBase const & backData() const;

  /**
   * Trigger an parameter change in all connected receive ports.
   * @param copyValue Flag whether the parameter value visible from the output will be preserved by the swap. This means that the current value is copied to the other buffer during the switch.
   * If set to false, the new parameter can have an arbitrary value, and has to be set completely.
   */
  void swapBuffers( bool copyValue );

  void connectInput( CommunicationProtocolBase::Input* port ) override;

  void connectOutput( CommunicationProtocolBase::Output* port ) override;

  bool disconnectInput( CommunicationProtocolBase::Input* port ) noexcept override;

  bool disconnectOutput( CommunicationProtocolBase::Output* port ) noexcept override;

private:
  ParameterType const mParameterType;

  std::unique_ptr<ParameterConfigBase> const mConfig;

  OutputBase* mOutput;
  std::vector<InputBase*> mInputs;

  /**
  * The internal data representation.
  */
  std::unique_ptr<ParameterBase> mBackData;
  std::unique_ptr<ParameterBase> mFrontData;

  static constexpr const char * sProtocolName = "DoubleBuffering";
};

class
///@cond NEVER
VISR_PML_LIBRARY_SYMBOL
///@endcond NEVER
DoubleBufferingProtocol::InputBase: public CommunicationProtocolBase::Input
{
  friend class DoubleBufferingProtocol;
public:

  InputBase();

  virtual ~InputBase();

  ParameterBase const & data() const;

  bool changed() const;

  void resetChanged();

  void setProtocolInstance( CommunicationProtocolBase * protocol ) override;

  DoubleBufferingProtocol * getProtocol() override { return mProtocol; }

  DoubleBufferingProtocol const * getProtocol() const override { return mProtocol; }

  void setProtocolInstance( DoubleBufferingProtocol * protocol );

private:
  void markChanged() { mChanged = true; }

  DoubleBufferingProtocol * mProtocol;
  bool mChanged;
};

template<class MessageType>
class DoubleBufferingProtocol::Input: public InputBase
{
  friend class InputBase;
public:
  using InputBase::changed;

  using InputBase::resetChanged;

  MessageType const & data() const
  {
    return static_cast<MessageType const&>(InputBase::data());
  }

protected:

private:
};

//////////////////////////////////////////////////////////////////
// Output


class
///@cond NEVER
VISR_PML_LIBRARY_SYMBOL
///@endcond NEVER
 DoubleBufferingProtocol::OutputBase: public CommunicationProtocolBase::Output
{
  friend class DoubleBufferingProtocol;
public:
  OutputBase();

  virtual ~OutputBase();

  ParameterBase & data();

  void setProtocolInstance( CommunicationProtocolBase * protocol ) override;

  DoubleBufferingProtocol * getProtocol() override { return mProtocol; }

  DoubleBufferingProtocol const * getProtocol() const override { return mProtocol; }

  /**
   * Make the current parameter available to the receiving ports.
   * @param copyValue Whether the parameter value is copied to the new output buffer. Otherwise the output
   * contains an arbitrary parameter value, and must be set/assigned completely.
   */
  void swapBuffers( bool copyValue = false );

  void setProtocolInstance( DoubleBufferingProtocol * protocol );

private:
  DoubleBufferingProtocol * mProtocol;
};

template<class MessageType>
class DoubleBufferingProtocol::Output: public OutputBase
{
public:
  using OutputBase::swapBuffers;

  MessageType & data()
  {
    return static_cast<MessageType&>(mProtocol->frontData());
  }
};

} // namespace pml
} // namespace visr

DEFINE_COMMUNICATION_PROTOCOL( visr::pml::DoubleBufferingProtocol, visr::pml::DoubleBufferingProtocol::staticType(), visr::pml::DoubleBufferingProtocol::staticName() )

#endif // VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED
