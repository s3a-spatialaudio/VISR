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
template< typename MessageTypeT >
class DoubleBufferingProtocol: public ril::CommunicationProtocolBase
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
    explicit Input( std::string const & name, ril::Component & parent )
    : ParameterPortBase( name, parent, ParameterPortBase::Direction::Input )
    , mProtocol(nullptr)
    , mChanged( true ) // Mark the data as changed for the first iteration
    {}

    MessageType const & data( ) const
    {
      return mProtocol->backData( );
    }

    bool hasChanged() const
    {
      return mChanged;
    }

    void resetChanged()
    {
      mChanged = false;
    }

    /**
     * To be called from the protocol.
    */
    void markChanged( )
    {
      mChanged = true;
    }


    void setProtocolInstance( DoubleBufferingProtocol * protocol )
    {
      mProtocol = protocol;
    }

    bool isConnected() const override
    {
      return mProtocol != nullptr;
    }
  protected:


  private:
    DoubleBufferingProtocol * mProtocol;
    bool mChanged;
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
    explicit Output( std::string const & name, ril::Component & parent )
     : ParameterPortBase( name, parent, ParameterPortBase::Direction::Output )
     , mProtocol(nullptr)
    {}

    MessageType & data()
    {
      return mProtocol -> frontData( );
    }

    void swapBuffers()
    {
      mProtocol->swapBuffers();
    }

    void setProtocolInstance( DoubleBufferingProtocol * protocol )
    {
      mProtocol = protocol;
    }
    /**
    * This whould not be accessible from the component.
    */

    bool isConnected() const override
    {
      return mProtocol != nullptr;
    }

  protected:

  private:
    DoubleBufferingProtocol * mProtocol;
  };

  explicit DoubleBufferingProtocol( ril::ParameterConfigBase const & config );

  explicit DoubleBufferingProtocol( ParameterConfigType const & config );

  ril::ParameterType parameterType( ) const override { return ril::ParameterToId<MessageType>::id; }

  virtual ril::CommunicationProtocolType protocolType( ) const override { return ril::CommunicationProtocolType::DoubleBuffering; }

  MessageType & frontData()
  {
    return *mFrontData;
  }

  MessageType const & frontData( ) const
  {
    return *mFrontData;
  }

  MessageType & backData( )
  {
    return *mBackData;
  }

  MessageType const & backData( ) const
  {
    return *mBackData;
  }

  void setData( MessageType const & newData )
  {
    *mBackData = newData;
  }

  void swapBuffers()
  {
    mBackData.swap( mFrontData );
    std::for_each( mInputs.begin(), mInputs.end(), []( Input* port ) { port->markChanged(); } );
  }

  void connectInput( ril::ParameterPortBase* port ) override;

  void connectOutput( ril::ParameterPortBase* port ) override;

  bool disconnectInput( ril::ParameterPortBase* port ) override;

  bool disconnectOutput( ril::ParameterPortBase* port ) override;

private:
  ParameterConfigType const mConfig;

  Output* mOutput;
  std::vector<Input*> mInputs;

  /**
   * The internal data representation.
   */
  std::unique_ptr<MessageTypeT> mBackData;
  std::unique_ptr<MessageTypeT> mFrontData;
};

template< typename MessageTypeT >
inline DoubleBufferingProtocol< MessageTypeT >::DoubleBufferingProtocol( ril::ParameterConfigBase const & config )
: DoubleBufferingProtocol( dynamic_cast<ParameterConfigType const &>(config) )
{
}

template< typename MessageTypeT >
inline DoubleBufferingProtocol< MessageTypeT >::DoubleBufferingProtocol( ParameterConfigType const & config )
  : mConfig( config )
  , mOutput( nullptr )
  , mBackData( new MessageTypeT( config ) )
  , mFrontData( new MessageTypeT( config ) )
{
}

template< typename MessageTypeT >
inline void DoubleBufferingProtocol< MessageTypeT >::connectInput( ril::ParameterPortBase* port )
{
  DoubleBufferingProtocol< MessageTypeT >::Input * typedPort = dynamic_cast<DoubleBufferingProtocol< MessageTypeT >::Input *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::connectInput(): port argument has wrong type." );
  }
  if( std::find( mInputs.begin( ), mInputs.end( ), typedPort ) == mInputs.end( ) )
  {
    mInputs.push_back( typedPort );
  }
  typedPort->setProtocolInstance( this );
}

template< typename MessageTypeT >
inline void DoubleBufferingProtocol< MessageTypeT >::connectOutput( ril::ParameterPortBase* port )
{
  DoubleBufferingProtocol< MessageTypeT >::Output * typedPort = dynamic_cast<DoubleBufferingProtocol< MessageTypeT >::Output*>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::connectOutput(): port argument has wrong type." );
  }
  if( mOutput )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::connectOutput(): output port already set." );
  }
  mOutput = typedPort;
  typedPort->setProtocolInstance( this );
}

template< typename MessageTypeT >
inline bool DoubleBufferingProtocol< MessageTypeT >::disconnectInput( ril::ParameterPortBase* port )
{
  DoubleBufferingProtocol< MessageTypeT >::Input * typedPort = dynamic_cast<DoubleBufferingProtocol< MessageTypeT >::Input *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::connectInput(): port argument has wrong type." );
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
inline bool DoubleBufferingProtocol< MessageTypeT >::disconnectOutput( ril::ParameterPortBase* port )
{
  DoubleBufferingProtocol< MessageTypeT >::Output * typedPort = dynamic_cast<DoubleBufferingProtocol< MessageTypeT >::Output *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::disconnectOutput(): port argument has wrong type." );
  }
  if( typedPort != mOutput )
  {
    // Trying to disconnect a port that is not the previously connected output
    return false;
  }
  mOutput->setProtocolInstance( this );
  mOutput = nullptr;
  return true;
}

} // namespace pml
} // namespace visr

DEFINE_COMMUNICATION_PROTOCOL_TYPE( visr::pml::DoubleBufferingProtocol, visr::ril::CommunicationProtocolType::DoubleBuffering )

#endif // VISR_PML_DOUBLE_BUFFERING_PROTOCOL_HPP_INCLUDED
