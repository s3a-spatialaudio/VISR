/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED
#define VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED

#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_type.hpp>

#include <libril/parameter_port_base.hpp>
#include <libril/parameter_type.hpp>
#include <libril/parameter_config_base.hpp>

#include <ciso646>
#include <memory>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{

/**
 * 
 */
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

  static constexpr CommunicationProtocolType staticType() { return communicationProtocolTypeFromString(sProtocolName); };

  static constexpr char const * staticName() { return sProtocolName; }

  ParameterType parameterType( ) const override;

  virtual CommunicationProtocolType protocolType( ) const override { return staticType(); }

  ParameterBase & frontData();

  ParameterBase const & frontData() const;

  ParameterBase & backData();

  ParameterBase const & backData() const;

  void setData( ParameterBase const & newData );

  void swapBuffers();

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

class DoubleBufferingProtocol::InputBase: public CommunicationProtocolBase::Input
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

class DoubleBufferingProtocol::OutputBase: public CommunicationProtocolBase::Output
{
  friend class DoubleBufferingProtocol;
public:
  OutputBase();

  virtual ~OutputBase();

  ParameterBase & data();

  void setProtocolInstance( CommunicationProtocolBase * protocol ) override;

  DoubleBufferingProtocol * getProtocol() override { return mProtocol; }

  DoubleBufferingProtocol const * getProtocol() const override { return mProtocol; }

  void swapBuffers();

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
