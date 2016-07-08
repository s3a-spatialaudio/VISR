/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED
#define VISR_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED

#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_type.hpp>

#include <libril/parameter_type.hpp>
#include <libril/parameter_config_base.hpp>

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
class MessageQueueProtocol: public ril::CommunicationProtocolBase
{
public:

  /**
   * Make the message type available to code using this template.
   */
  using MessageType = MessageTypeT;


  /**
   * Provide alias for parameter configuration class type for the contained parameter values.
   */
  using ParameterConfigType = typename ril::ParameterToConfigType<MessageTypeT>::ConfigType;

  class Output
  {
  public:
    /**
     * Default constructor.
     */
    Output()
      : mProtocol( nullptr )
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
      mProtocol ->enqueue( val );
    }

    /**
     * This whould not be accessible from the component.
     */
  protected:
    void setProtocolInstance( MessageQueueProtocol * protocol )
    {
      mProtocol = protocol;
    }
  private:
    MessageQueueProtocol * mProtocol;
  };

  class Input
  {
  public:
    /**
    * Default constructor.
    */
    Input()
      : mProtocol( nullptr )
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

  protected:
    void setProtocolInstance( MessageQueueProtocol * protocol )
    {
      mProtocol = protocol;
    }
  private:
    MessageQueueProtocol * mProtocol;
  };

  explicit MessageQueueProtocol( ril::ParameterConfigBase const & config );

  explicit MessageQueueProtocol( ParameterConfigType const & config );

  ril::ParameterType parameterType() const override { return ril::ParameterToId<MessageTypeT>::id; }

  virtual ril::CommunicationProtocolType protocolType() const override { return ril::CommunicationProtocolType::MessageQueue; }

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

private:
  /**
   * The internal data representation.
   */
  std::deque<MessageTypeT> mQueue;

  ParameterConfigType const mConfig;
};

template< typename MessageTypeT >
inline MessageQueueProtocol< MessageTypeT >::MessageQueueProtocol( ril::ParameterConfigBase const & config )
: MessageQueueProtocol( dynamic_cast<ParameterConfigType const &>(config) )
{
}

template< typename MessageTypeT >
inline MessageQueueProtocol< MessageTypeT >::MessageQueueProtocol( ParameterConfigType const & config )
  : mConfig( config )
{
}

} // namespace pml
} // namespace visr

DEFINE_COMMUNICATION_PROTOCOL_TYPE( visr::pml::MessageQueueProtocol, visr::ril::CommunicationProtocolType::MessageQueue )

#endif // VISR_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED
