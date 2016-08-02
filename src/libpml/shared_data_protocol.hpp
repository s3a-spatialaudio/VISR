/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED
#define VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED

#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_type.hpp>

#include <libril/parameter_type.hpp>
#include <libril/parameter_config_base.hpp>
#include <libril/parameter_port_base.hpp>

#include <algorithm>
#include <ciso646>
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
template< typename MessageTypeT >
class SharedDataProtocol: public ril::CommunicationProtocolBase
{
public:

  /**
   * Make the message type available to code using this template.
   */
  using MessageType = MessageTypeT;

  class Input: public ril::ParameterPortBase
  {
  public:
    /**
    * Default constructor.
    */
    Input( ril::Component & parent, std::string const & name )
     : ParameterPortBase( parent, name, ParameterPortBase::Direction::Input )
     , mProtocol(nullptr)
    {}

    MessageType const & data( ) const
    {
      return mProtocol->data( );
    }
    void setProtocolInstance( SharedDataProtocol * protocol )
    {
      mProtocol = protocol;
    }
  private:
    SharedDataProtocol * mProtocol;
  };

  /**
   * Provide alias for parameter configuration class type for the contained parameter values.
   */
  using ParameterConfigType = typename ril::ParameterToConfigType<MessageTypeT>::ConfigType;

  class Output: public ril::ParameterPortBase
  {
  public:
    /**
     * Default constructor.
     */
    explicit Output( ril::Component & parent, std::string const & name )
     : ParameterPortBase( parent, name, ParameterPortBase::Direction::Output )
     , mProtocol(nullptr)
    {}

    MessageType & data()
    {
      return mProtocol ->data( );
    }

    void setProtocolInstance( SharedDataProtocol * protocol )
    {
      mProtocol = protocol;
    }
  private:
    SharedDataProtocol * mProtocol;
  };

  explicit SharedDataProtocol( ril::ParameterConfigBase const & config );

  explicit SharedDataProtocol( ParameterConfigType const & config );

  ril::ParameterType parameterType( ) const override { return ril::ParameterToId<MessageType>::id; }

  virtual ril::CommunicationProtocolType protocolType( ) const override { return ril::CommunicationProtocolType::SharedData; }

  MessageType & data()
  {
    return mData;
  }

  MessageType const & data( ) const
  {
    return mData;
  }

  void setData( MessageType const & newData )
  {
    mData.operator=( newData );
  }

  void connectInput( ril::ParameterPortBase* port ) override;

  void connectOutput( ril::ParameterPortBase* port ) override;

  bool disconnectInput( ril::ParameterPortBase* port ) override;

  bool disconnectOutput( ril::ParameterPortBase* port ) override;

private:
  ParameterConfigType const mConfig;

  /**
   * The internal data representation.
   */
  MessageTypeT mData;

  Output* mOutput;
  std::vector<Input*>  mInputs;
};

template< typename MessageTypeT >
inline SharedDataProtocol< MessageTypeT >::SharedDataProtocol( ril::ParameterConfigBase const & config )
: SharedDataProtocol( dynamic_cast<ParameterConfigType const &>(config) )
{
}

template< typename MessageTypeT >
inline SharedDataProtocol< MessageTypeT >::SharedDataProtocol( ParameterConfigType const & config )
  : mConfig( config )
  , mData( config )
  , mOutput( nullptr )
{
}

template< typename MessageTypeT >
inline void SharedDataProtocol< MessageTypeT >::connectInput( ril::ParameterPortBase* port )
{
  SharedDataProtocol< MessageTypeT >::Input * typedPort = dynamic_cast<SharedDataProtocol< MessageTypeT >::Input *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "SharedDataProtocol::connectInput(): port argument has wrong type." );
  }
  if( std::find( mInputs.begin(), mInputs.end(), typedPort ) == mInputs.end() )
  {
    mInputs.push_back( typedPort );
    typedPort->setProtocolInstance( this );
  }
}

template< typename MessageTypeT >
inline void SharedDataProtocol< MessageTypeT >::connectOutput( ril::ParameterPortBase* port )
{
  SharedDataProtocol< MessageTypeT >::Output * typedPort = dynamic_cast<SharedDataProtocol< MessageTypeT >::Output *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "SharedDataProtocol::connectOutput(): port argument has wrong type." );
  }
  if( mOutput )
  {
    throw std::invalid_argument( "SharedDataProtocol::connectOutput(): output port already set." );
  }
  mOutput = typedPort;
  typedPort->setProtocolInstance( this );
}

template< typename MessageTypeT >
inline bool SharedDataProtocol< MessageTypeT >::disconnectInput( ril::ParameterPortBase* port )
{
  SharedDataProtocol< MessageTypeT >::Input * typedPort = dynamic_cast<SharedDataProtocol< MessageTypeT >::Input *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "SharedDataProtocol::connectInput(): port argument has wrong type." );
  }
  typename std::vector<Input*>::iterator findIt = std::find( mInputs.begin( ), mInputs.end( ), typedPort );
  if( findIt == mInputs.end( ) )
  {
    return false;
  }
  (*findIt)->setProtocolInstance( nullptr );
  mInputs.erase( findIt );
  return true;
}

template< typename MessageTypeT >
inline bool SharedDataProtocol< MessageTypeT >::disconnectOutput( ril::ParameterPortBase* port )
{
  SharedDataProtocol< MessageTypeT >::Output * typedPort = dynamic_cast<SharedDataProtocol< MessageTypeT >::Output *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "SharedDataProtocol::disconnectOutput(): port argument has wrong type." );
  }
  if( typedPort != mOutput )
  {
    // Trying to disconnect a port that is not the previously connected output.
    return false;
  }
  mOutput->setProtocolInstance( nullptr );
  mOutput = nullptr;
  return true;
}

} // namespace pml
} // namespace visr

DEFINE_COMMUNICATION_PROTOCOL_TYPE( visr::pml::SharedDataProtocol, visr::ril::CommunicationProtocolType::SharedData )

#endif // VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED
