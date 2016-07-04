/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_MESSAGE_QUEUE_HPP_INCLUDED
#define VISR_PML_MESSAGE_QUEUE_HPP_INCLUDED

#include <libril/communication_protocol_base.hpp>

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
class MessageQueue: public ril::CommunicationProtocolBase
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

  class Input
  {
  public:
    /**
     * Default constructor.
     */
    Input() {}

    void enqueue( MessageType const & val )
    {
      mParent->enqueue( val );
    }

    /**
     * This whould not be accessible from the component.
     */
  private:
    MessageQueue * mParent;
  };

  class Output
  {
  public:
    /**
    * Default constructor.
    */
    inline Output() {}

    bool empty() const
    {
      return mParent->empty();
    }

    std::size_t size() const
    {
      return mParent->numberOfElements();
    }

    MessageType const & front() const
    {
      return mParent->nextElement();
    }

    void pop()
    {
      mParent->popNextElement();
    }

    void clear()
    {
      mParent->clear();
    }

    /**
    * This whould not be accessible from the component.
    */
  private:
    MessageQueue * mParent;
  };

  explicit MessageQueue( ril::ParameterConfigBase const & config );

  explicit MessageQueue( ParameterConfigType const & config );

  ril::ParameterType type() const override { return ril::ParameterToId<MessageTypeT>::id; }

  /**
   * Default constructor, creates an empty message queue.
   */
  MessageQueue();

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
inline MessageQueue< MessageTypeT >::MessageQueue( ril::ParameterConfigBase const & config )
: MessageQueue( dynamic_cast<ParameterConfigType const &>(config) )
{
}

template< typename MessageTypeT >
inline MessageQueue< MessageTypeT >::MessageQueue( ParameterConfigType const & config )
  : mConfig( config )
{
}


template< typename MessageTypeT >
inline MessageQueue< MessageTypeT >::MessageQueue( )
  : mConfig( 0 )
{
}



} // namespace pml
} // namespace visr


#endif // VISR_PML_MESSAGE_QUEUE_HPP_INCLUDED
