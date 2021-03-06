/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED
#define VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/communication_protocol_base.hpp>
#include <libvisr/communication_protocol_type.hpp>

#include <libvisr/parameter_type.hpp>
#include <libvisr/parameter_config_base.hpp>
#include <libvisr/parameter_port_base.hpp>

#include <ciso646>
#include <memory>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{

class VISR_PML_LIBRARY_SYMBOL SharedDataProtocol: public CommunicationProtocolBase
{
public:
  /**
   * Forward declarations of inner classes.
   */
  class InputBase;
  template<class DataType > class Input;
  class OutputBase;
  template<class DataType > class Output;

  explicit SharedDataProtocol( ParameterType const & config,
                               ParameterConfigBase const & parameterConfig );

  virtual ~SharedDataProtocol() override;

  ParameterType parameterType( ) const override;

  virtual CommunicationProtocolType protocolType( ) const override;

  static constexpr CommunicationProtocolType staticType() { return communicationProtocolTypeFromString( sProtocolName ); };

  static constexpr const char * staticName() { return sProtocolName; }

  ParameterBase & data()
  {
    return *mData;
  }

  ParameterBase const & data( ) const
  {
    return *mData;
  }

  void connectInput( CommunicationProtocolBase::Input* port ) override;

  void connectOutput( CommunicationProtocolBase::Output* port ) override;

  bool disconnectInput( CommunicationProtocolBase::Input* port ) noexcept override;

  bool disconnectOutput( CommunicationProtocolBase::Output* port ) noexcept override;

private:
  ParameterType mParameterType;

  std::unique_ptr<ParameterConfigBase> const mParameterConfig;

  /**
   * The internal data representation.
   */
  std::unique_ptr<ParameterBase> mData;

  OutputBase* mOutput;
  std::vector<InputBase*>  mInputs;

  static constexpr const char * sProtocolName = "SharedData";
};

class VISR_PML_LIBRARY_SYMBOL SharedDataProtocol::InputBase: public CommunicationProtocolBase::Input
{
public:
  /**
  * Default constructor.
  */
  InputBase()
    : mProtocol( nullptr )
  {
  }

  virtual ~InputBase();

  void setProtocolInstance( CommunicationProtocolBase * protocol ) override;

  SharedDataProtocol * getProtocol() override { return mProtocol; }

  SharedDataProtocol const * getProtocol() const override { return mProtocol; }

  ParameterBase const & data() const
  {
    return mProtocol->data();
  }

  void setProtocolInstance( SharedDataProtocol * protocol )
  {
    mProtocol = protocol;
  }
private:
  SharedDataProtocol * mProtocol;
}; // class InputBase

template<class MessageType>
class SharedDataProtocol::Input: public InputBase
{
  friend class InputBase;
public:
  MessageType const & data() const
  {
    return static_cast<MessageType const&>(InputBase::data());
  }
};

//////////////////////////////////////////////////////////////////
// Output

class VISR_PML_LIBRARY_SYMBOL SharedDataProtocol::OutputBase: public CommunicationProtocolBase::Output
{
public:
  /**
  * Default constructor.
  */
  OutputBase()
    : mProtocol( nullptr )
  {
  }

  virtual ~OutputBase();

  void setProtocolInstance( CommunicationProtocolBase * protocol ) override;

  SharedDataProtocol * getProtocol() override { return mProtocol; }

  SharedDataProtocol const * getProtocol() const override { return mProtocol; }

  ParameterBase & data()
  {
    return mProtocol->data();
  }

  void setProtocolInstance( SharedDataProtocol * protocol )
  {
    mProtocol = protocol;
  }

#if 0
  bool isConnected() const override
  {
    return mProtocol != nullptr;
  }
#endif
private:
  SharedDataProtocol * mProtocol;
}; // class OutputBase

template<class MessageType>
class SharedDataProtocol::Output: public OutputBase
{
public:
  MessageType & data()
  {
    return static_cast<MessageType&>(OutputBase::data());
  }
};

} // namespace pml
} // namespace visr

DEFINE_COMMUNICATION_PROTOCOL( visr::pml::SharedDataProtocol, visr::pml::SharedDataProtocol::staticType(), visr::pml::SharedDataProtocol::staticName() )

#endif // VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED
