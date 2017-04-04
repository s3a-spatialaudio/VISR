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
#include <memory>
#include <stdexcept>

namespace visr
{
namespace pml
{

/**
 * A FIFO-type message queue template class for storing and passing message data.
 * @note This class does provide the same level of thread safety as, e.g., the STL.
 * I.e., calling code from different thread must ensure that concurrent accesses
 * to the same instances are appropriately secured against race conditions.
 */
class MessageQueueProtocol: public CommunicationProtocolBase
{
public:
  /**
  * Forward declarations of the internal data types.
  */
  class InputBase;
  template<class DataType > class Input;
  class OutputBase;
  template<class DataType > class Output;

  static constexpr CommunicationProtocolType staticType() { return communicationProtocolTypeFromString( sProtocolName ); }

  static constexpr const char * staticName() { return sProtocolName; }

  explicit MessageQueueProtocol( ParameterType const & parameterType,
                                 ParameterConfigBase const & config );

  ParameterType parameterType() const override;

  virtual CommunicationProtocolType protocolType() const override;

  /**
   * Remove all elements from the message queue.
   */
  void clear();

  /**
   * Return whether the list is empty, i.e., contains zero elements.
   */
  bool empty() const;

  /**
   * Return the number of elements currently contained in the list.
   */
  std::size_t numberOfElements() const;

  void enqueue( std::unique_ptr<ParameterBase>& val );

  /*
   * Return the next element in the FIFO queue.
   * @return A reference to the next element.
   * @throw logic_error If the queue is empty
   */
  ParameterBase const& nextElement() const
;
  /**
   * Remove the next output element from the list.
   * @throw std::logic_error If the queue is empty prior to this call.
   */
  void popNextElement();

  void connectInput( ParameterPortBase* port ) override;

  void connectOutput( ParameterPortBase* port ) override;

  bool disconnectInput( ParameterPortBase* port ) override;

  bool disconnectOutput( ParameterPortBase* port ) override;

private:
  using QueueType = std::deque<std::unique_ptr<ParameterBase> >;
  /**
   * The internal data representation.
   */
  QueueType mQueue;

  ParameterType const mParameterType;

  std::unique_ptr<ParameterConfigBase> const mParameterConfig;

  InputBase * mInput;
  OutputBase* mOutput;

  static constexpr const char * sProtocolName = "MessageQueue";
};

///////////////////////////////////////////////////////////////////////////////
// Input

class MessageQueueProtocol::InputBase: public CommunicationProtocolBase::Input
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

  MessageQueueProtocol * getProtocol() override { return mProtocol; }

  MessageQueueProtocol const * getProtocol() const override { return mProtocol; }

  bool empty() const
  {
    return mProtocol->empty();
  }

  std::size_t size() const
  {
    return mProtocol->numberOfElements();
  }

  ParameterBase const & front() const
  {
    return mProtocol->nextElement();
  }

  void pop()
  {
    mProtocol->popNextElement();
  }

  void clear()
  {
    mProtocol->clear();
  }

  void setProtocolInstance( MessageQueueProtocol * protocol )
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
  MessageQueueProtocol * mProtocol;
};

template<typename MessageType>
class MessageQueueProtocol::Input: public InputBase
{
public:
  MessageType const & front() const
  {
    return static_cast<MessageType const &>( InputBase::front() );
  }
};

///////////////////////////////////////////////////////////////////////////////
// Output

class MessageQueueProtocol::OutputBase: public CommunicationProtocolBase::Output
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

  MessageQueueProtocol * getProtocol() override { return mProtocol; }

  MessageQueueProtocol const * getProtocol() const override { return mProtocol; }

  bool empty() const
  {
    return mProtocol->empty();
  }

  std::size_t size() const
  {
    return mProtocol->numberOfElements();
  }

  void enqueue( std::unique_ptr<ParameterBase> && val )
  {
    // Move to impl object
    mProtocol->enqueue( val );
  }

  void setProtocolInstance( MessageQueueProtocol * protocol )
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
  MessageQueueProtocol * mProtocol;
};

template<class MessageType>
class MessageQueueProtocol::Output: public OutputBase
{
public:
  void enqueue( MessageType const & val )
  {
    OutputBase::enqueue( std::unique_ptr<ParameterBase>( new MessageType( val ) ) );
  }

  void enqueue( MessageType && val )
  {
    OutputBase::enqueue( std::unique_ptr<MessageType>(new MessageType( val )) );
  }
};

} // namespace pml
} // namespace visr

DEFINE_COMMUNICATION_PROTOCOL( visr::pml::MessageQueueProtocol, visr::pml::MessageQueueProtocol::staticType(), visr::pml::MessageQueueProtocol::staticName() )

#endif // VISR_PML_MESSAGE_QUEUE_PROTOCOL_HPP_INCLUDED
