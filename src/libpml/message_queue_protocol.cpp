/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "message_queue_protocol.hpp"

namespace visr
{
namespace pml
{

MessageQueueProtocol::MessageQueueProtocol( ParameterType const & parameterType,
                                            ParameterConfigBase const & parameterConfig )
 : mParameterType( parameterType )
 , mParameterConfig( parameterConfig.clone() )
 , mInput( nullptr )
 , mOutput( nullptr )
{
}

ParameterType MessageQueueProtocol::parameterType() const
{
  return mParameterType;
}

CommunicationProtocolType MessageQueueProtocol::protocolType() const
{
  return staticType();
}

void MessageQueueProtocol::clear()
{
  mQueue.clear();
}

bool MessageQueueProtocol::empty() const 
{
  return mQueue.empty();
}

std::size_t MessageQueueProtocol::numberOfElements() const
{
  return mQueue.size();
}

void MessageQueueProtocol::enqueue( std::unique_ptr<ParameterBase>& val )
{
  mQueue.push_front( std::move( val ) );
}

ParameterBase const& MessageQueueProtocol::nextElement() const
{
  if( empty() )
  {
    throw std::logic_error( "Calling nextElement() on an empty message queue." );
  }
  return *(mQueue.back());
}

void MessageQueueProtocol::popNextElement()
{
  if( empty() )
  {
    throw std::logic_error( "Calling nextElement() on an empty message queue." );
  }
  mQueue.pop_back();
}

void MessageQueueProtocol::connectInput( CommunicationProtocolBase::Input* port )
{
  MessageQueueProtocol::InputBase * typedPort = dynamic_cast<MessageQueueProtocol::InputBase*>(port);
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

void MessageQueueProtocol::connectOutput( CommunicationProtocolBase::Output* port )
{
  MessageQueueProtocol::OutputBase * typedPort = dynamic_cast<MessageQueueProtocol::OutputBase *>(port);
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

bool MessageQueueProtocol::disconnectInput( CommunicationProtocolBase::Input* port ) noexcept
{
  MessageQueueProtocol::InputBase * typedPort = dynamic_cast<MessageQueueProtocol::InputBase *>(port);
  if( not typedPort )
  {
    return false;
  }
  if( typedPort != mInput )
  {
    // Trying to disconnect a port that is not the previously connected input." );
    return false;
  }
  mInput->setProtocolInstance( static_cast<MessageQueueProtocol*>(nullptr) );
  mInput = nullptr;
  return true;
}

bool MessageQueueProtocol::disconnectOutput( CommunicationProtocolBase::Output* port ) noexcept
{
  MessageQueueProtocol::OutputBase * typedPort = dynamic_cast<MessageQueueProtocol::OutputBase *>(port);
  if( not typedPort )
  {
    return false;
  }
  if( typedPort != mOutput )
  {
    // Trying to disconnect a port that is not the previously connected output." );
    return false;
  }
  mOutput->setProtocolInstance( static_cast<MessageQueueProtocol*>(nullptr) );
  mOutput = nullptr;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// InputBase

MessageQueueProtocol::InputBase::~InputBase() = default;

void MessageQueueProtocol::InputBase::setProtocolInstance( CommunicationProtocolBase * protocol )
{
  MessageQueueProtocol * mp = dynamic_cast<MessageQueueProtocol*>( protocol );
  if( not mp )
  {
    throw std::invalid_argument( "MessageQueueProtocol::InputBase::setProtocolInstance(): Called with nonmatching protocol. ");
  }
  setProtocolInstance( mp );
}

///////////////////////////////////////////////////////////////////////////////
// OutputBase

MessageQueueProtocol::OutputBase::~OutputBase() = default;

void MessageQueueProtocol::OutputBase::setProtocolInstance( CommunicationProtocolBase * protocol )
{
  MessageQueueProtocol * mp = dynamic_cast<MessageQueueProtocol*>(protocol);
  if( not mp )
  {
    throw std::invalid_argument( "MessageQueueProtocol::InputBase::setProtocolInstance(): Called with nonmatching protocol. " );
  }
  setProtocolInstance( mp );
}

} // namespace pml
} // namespace visr
