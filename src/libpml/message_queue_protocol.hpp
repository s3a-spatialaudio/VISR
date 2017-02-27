/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED
#define VISR_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED

#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_type.hpp>

#include <libril/parameter_port_base.hpp>
#include <libril/parameter_type.hpp>
#include <libril/parameter_config_base.hpp>

#include <ciso646>
#include <deque>
#include <stdexcept>

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
class MessageQueueProtocol: public CommunicationProtocolBase
{
public:

  /**
   * Make the message type available to code using this template.
   */
  using MessageType = MessageTypeT;


  /**
   * Provide alias for parameter configuration class type for the contained parameter values.
   */
  using ParameterConfigType = typename ParameterToConfigType<MessageTypeT>::ConfigType;

  class Output: public ParameterPortBase
  {
  public:
    /**
     * Default constructor.
     */
    explicit Output( std::string const & name, Component & parent)
     : ParameterPortBase( name, parent, ParameterPortBase::Direction::Output )
     , mProtocol( nullptr )
    {}

    bool empty( ) const
    {
      return mProtocol->empty( );
    }

    std::size_t size( ) const
    {
      return mProtocol ->numberOfElements( );
    }

    void enqueue( MessageType const & val )
    {
      // Move to impl object
      mProtocol ->enqueue( val );
    }


    void setProtocolInstance( MessageQueueProtocol * protocol )
    {
      mProtocol = protocol;
    }
    bool isConnected() const override
    {
      return mProtocol != nullptr;
    }
  private:
    /**
     * This should not be part of the user-defined template
     */
    MessageQueueProtocol * mProtocol;
  };

  class Input: public ParameterPortBase
  {
  public:
    /**
    * Default constructor.
    */
    explicit Input( std::string const & name, Component & parent )
     : ParameterPortBase( name, parent, ParameterPortBase::Direction::Input )
     , mProtocol( nullptr )
    {}

    bool empty() const
    {
      return mProtocol ->empty();
    }

    std::size_t size() const
    {
      return mProtocol ->numberOfElements();
    }

    MessageType const & front() const
    {
      return mProtocol ->nextElement();
    }

    void pop()
    {
      mProtocol ->popNextElement();
    }

    void clear()
    {
      mProtocol ->clear();
    }

    void setProtocolInstance( MessageQueueProtocol * protocol )
    {
      mProtocol = protocol;
    }

    bool isConnected() const override
    {
      return mProtocol != nullptr;
    }
  private:
    MessageQueueProtocol * mProtocol;
  };

  explicit MessageQueueProtocol( ParameterConfigBase const & config );

  explicit MessageQueueProtocol( ParameterConfigType const & config );

  ParameterType parameterType() const override { return ParameterToId<MessageTypeT>::id; }

  virtual CommunicationProtocolType protocolType() const override { return CommunicationProtocolType::MessageQueue; }

  /**
   * Remove all elements from the message queue.
   */
  void clear() { mQueue.clear(); }

  /**
   * Return whether the list is empty, i.e., contains zero elements.
   */
  bool empty() const { return mQueue.empty(); }

  /**
   * Return the number of elements currently contained in the list.
   */
  std::size_t numberOfElements() const { return mQueue.size(); }

  void enqueue( MessageType const & val ) { mQueue.push_front( val ); }

  void enqueue( MessageType && val ) { mQueue.push_front( val ); }

  /**
   * Return the next element in the FIFO queue.
   * @return A reference to the next element.
   * @throw logic_error If the queue is empty
   */
  MessageType const& nextElement() const
  {
    if( empty() )
    {
      throw std::logic_error( "Calling nextElement() on an empty message queue." );
    }
    return mQueue.back();
  }

  /**
   * Remove the next output element from the list.
   * @throw std::logic_error If the queue is empty prior to this call.
   */
  void popNextElement()
  {
    if( empty() )
    {
      throw std::logic_error( "Calling nextElement() on an empty message queue." );
    }
    mQueue.pop_back();
  }

  void connectInput( ParameterPortBase* port ) override;

  void connectOutput( ParameterPortBase* port ) override;

  bool disconnectInput( ParameterPortBase* port ) override;

  bool disconnectOutput( ParameterPortBase* port ) override;

private:
  /**
   * The internal data representation.
   */
  std::deque<MessageTypeT> mQueue;

  ParameterConfigType const mConfig;

  Input * mInput;
  Output* mOutput;
};

template< typename MessageTypeT >
inline MessageQueueProtocol< MessageTypeT >::MessageQueueProtocol( ParameterConfigBase const & config )
: MessageQueueProtocol( dynamic_cast<ParameterConfigType const &>(config) )
{
}

template< typename MessageTypeT >
inline MessageQueueProtocol< MessageTypeT >::MessageQueueProtocol( ParameterConfigType const & config )
  : mConfig( config )
  , mInput( nullptr )
  , mOutput( nullptr )
{
}

template< typename MessageTypeT >
inline void MessageQueueProtocol< MessageTypeT >::connectInput( ParameterPortBase* port )
{
  MessageQueueProtocol< MessageTypeT >::Input * typedPort = dynamic_cast<MessageQueueProtocol< MessageTypeT >::Input *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "MessageQueueProtocol::connectInput(): port argument has wrong type." );
  }
  if( mInput )
  {
    throw std::invalid_argument( "MessageQueueProtocol::connectInput(): input port already set." );
  }
  mInput = typedPort;
  mInput->setProtocolInstance( this );
}

template< typename MessageTypeT >
inline void MessageQueueProtocol< MessageTypeT >::connectOutput( ParameterPortBase* port )
{
  MessageQueueProtocol< MessageTypeT >::Output * typedPort = dynamic_cast<MessageQueueProtocol< MessageTypeT >::Output *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "MessageQueueProtocol::connectOutput(): port argument has wrong type." );
  }
  if( mOutput )
  {
    throw std::invalid_argument( "MessageQueueProtocol::connectOutput(): output port already set." );
  }
  mOutput = typedPort;
  mOutput->setProtocolInstance( this );
}

template< typename MessageTypeT >
inline bool MessageQueueProtocol< MessageTypeT >::disconnectInput( ParameterPortBase* port )
{
  MessageQueueProtocol< MessageTypeT >::Input * typedPort = dynamic_cast<MessageQueueProtocol< MessageTypeT >::Input *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "MessageQueueProtocol::connectInput(): port argument has wrong type." );
  }
  if( typedPort != mInput )
  {
    // Trying to disconnect a port that is not the previously connected input." );
    return false;
  }
  mInput->setProtocolInstance( nullptr );
  mInput = nullptr;
  return true;
}

template< typename MessageTypeT >
inline bool MessageQueueProtocol< MessageTypeT >::disconnectOutput( ParameterPortBase* port )
{
  MessageQueueProtocol< MessageTypeT >::Output * typedPort = dynamic_cast<MessageQueueProtocol< MessageTypeT >::Output *>(port);
  if( not typedPort )
  {
    std::invalid_argument( "MessageQueueProtocol::disconnectOutput(): port argument has wrong type." );
  }
  if( typedPort != mOutput )
  {
    // Trying to disconnect a port that is not the previously connected output." );
    return false;
  }
  mOutput->setProtocolInstance( nullptr );
  mOutput = nullptr;
  return true;
}


} // namespace pml
} // namespace visr

DEFINE_COMMUNICATION_PROTOCOL_TYPE( visr::pml::MessageQueueProtocol, visr::CommunicationProtocolType::MessageQueue )

#endif // VISR_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED
