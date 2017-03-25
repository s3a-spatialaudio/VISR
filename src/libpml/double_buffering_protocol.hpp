/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED
#define VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED

#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_type.hpp>

#include <libril/parameter_port_base.hpp>
#include <libril/parameter_type.hpp>
#include <libril/parameter_config_base.hpp>

#include <algorithm>
#include <ciso646>
#include <memory>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{

/**
 * A FIFO-type message queue template class for storing and passing message data.
 * @tparam MessageTypeT Type of the contained elements.
 * @note This class does provide the same level of thread safety as, e.g., the STL.
 * I.e., calling code from different thread must ensure that concurrent accesses
 * to the same instances are appropriately secured against race conditions.
 */


// CommunicationProtocolType const doubleBufferingType = ;

class DoubleBufferingProtocol: public CommunicationProtocolBase
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

  static constexpr const char protocolName[] = "DoubleBuffering";

 static constexpr CommunicationProtocolType type = communicationProtocolTypeFromString("DoubleBuffering");

  ParameterType parameterType( ) const override;

  virtual CommunicationProtocolType protocolType( ) const override { return type; }

  ParameterBase & frontData();

  ParameterBase const & frontData() const;

  ParameterBase & backData();

  ParameterBase const & backData() const;

  void setData( ParameterBase const & newData );

  void swapBuffers();

  void connectInput( ParameterPortBase* port ) override;

  void connectOutput( ParameterPortBase* port ) override;

  bool disconnectInput( ParameterPortBase* port ) override;

  bool disconnectOutput( ParameterPortBase* port ) override;

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

};

class DoubleBufferingProtocol::InputBase
{
  friend class DoubleBufferingProtocol;
public:

  explicit InputBase();

  ParameterBase const & data() const;

  bool changed() const;

  void resetChanged();

  void setProtocolInstance( DoubleBufferingProtocol * protocol );

  DoubleBufferingProtocol * getProtocol() { return mProtocol; }

  DoubleBufferingProtocol const * getProtocol() const { return mProtocol; }

private:

  DoubleBufferingProtocol * mProtocol;
  bool mChanged;
};

template<class MessageType>
class DoubleBufferingProtocol::Input: public InputBase
{
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

class DoubleBufferingProtocol::OutputBase
{
  friend class DoubleBufferingProtocol;
public:
  explicit OutputBase();

  ParameterBase & data();

  void swapBuffers();

  void setProtocolInstance( DoubleBufferingProtocol * protocol );

  DoubleBufferingProtocol * getProtocol() { return mProtocol; }

  DoubleBufferingProtocol const * getProtocol() const { return mProtocol; }

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

DEFINE_COMMUNICATION_PROTOCOL( visr::pml::DoubleBufferingProtocol, visr::pml::DoubleBufferingProtocol::type )

#endif // VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED
