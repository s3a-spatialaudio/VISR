/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "udp_sender.hpp"

#include <libpml/string_parameter_config.hpp>

#include <boost/asio/placeholders.hpp>
//#include <boost/asio.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/lock_types.hpp>

#include <ciso646>
#include <memory>
#include <sstream>
#include <vector>

namespace visr
{
namespace rcl
{

  UdpSender::UdpSender( ril::SignalFlowContext& context,
                        char const * name,
                        ril::CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mMode( Mode::Asynchronous)
 , mMessageInput( "messageInput", *this, pml::StringParameterConfig(32768) )
{
}

UdpSender::~UdpSender()
{
  if( mIoServiceInstance.get() != nullptr )
  {
    mIoServiceInstance->stop();
  }
  if( mServiceThread.get() != nullptr  )
  {
    mServiceThread->join();
  }
}

void UdpSender::setup(std::size_t sendPort, std::string const & receiverAddress, std::size_t receiverPort,
                      UdpSender::Mode mode, boost::asio::io_service* externalIoService /*= nullptr*/ )
{
  using boost::asio::ip::udp;
  mMode = mode;
  if( mMode == Mode::ExternalServiceObject )
  {
    if( externalIoService == nullptr )
    {
      throw std::invalid_argument( "UdpReceiver: If mode == Mode::ExternalServiceObject, the \"externalServiceObject\" must not be zero." );
    }
    mIoServiceInstance.reset( );
    mIoService = externalIoService;
  }
  else
  {
    if( externalIoService != nullptr )
    {
      throw std::invalid_argument( "UdpReceiver: A non-null externalIoService parameter must be given only if mode == Mode::ExternalServiceObject" );
    }
    mIoServiceInstance.reset( new boost::asio::io_service( ) );
    mIoService = mIoServiceInstance.get( );
  }

  if( (mMode == Mode::Synchronous) or ( mMode == Mode::ExternalServiceObject ) )
  {
    mIoServiceWork.reset( );
  }
  else
  {
    mIoServiceWork.reset( new  boost::asio::io_service::work( *mIoService ) );
  }
  mInternalMessageBuffer.reset( new pml::MessageQueue< pml::StringParameter >( ) );

  udp::resolver resolver( *mIoService );
  udp::resolver::query query( udp::v4( ), receiverAddress, std::to_string(receiverPort) );
  mRemoteEndpoint = *resolver.resolve( query );
  std::cout << "Remote endpoint: " << mRemoteEndpoint.address().to_string() << ":" << mRemoteEndpoint.port() << std::endl;
  if( mRemoteEndpoint.port() != static_cast<unsigned short>(receiverPort) )
  {
    throw std::logic_error("");
  }

  udp::endpoint localEndpoint( udp::v4( ), static_cast<unsigned short>(sendPort) );
  mSocket.reset( new udp::socket( *mIoService, localEndpoint) );

  if(  mMode == Mode::Asynchronous )
  {
    mServiceThread.reset( new boost::thread( boost::bind( &boost::asio::io_service::run, mIoService ) ));
  }
}

void UdpSender::process()
{
  switch( mMode )
  {
  case Mode::Synchronous:
    // we don't operate an internal message queue in this mode.
    while( !mMessageInput.empty() )
    {
      pml::StringParameter const & nextMsg = mMessageInput.front( );
      // boost::system::error_code err;
      std::size_t bytesSent = mSocket->send_to( boost::asio::buffer(nextMsg.c_str(), nextMsg.size()), mRemoteEndpoint );
      if( bytesSent != nextMsg.size() )
      {
        throw std::runtime_error( "Number of sent bytes differs from message size." );
      }
      mMessageInput.pop( );
    }
    break;
  case Mode::Asynchronous:
  case Mode::ExternalServiceObject:
    {
      boost::lock_guard<boost::mutex> lock( mMutex );
      bool const transmissionPending = not mInternalMessageBuffer->empty();
      while( !mMessageInput.empty() )
      {
        std::string const & nextMsg = mMessageInput.front();
        mInternalMessageBuffer->enqueue( pml::StringParameter( nextMsg ) );
        mMessageInput.pop();
      }
      if( not mInternalMessageBuffer->empty() and not transmissionPending )
      {
        std::string const & nextMsg = mInternalMessageBuffer->nextElement( );
        mSocket->async_send_to( boost::asio::buffer( nextMsg.c_str( ), nextMsg.size( ) ),
                                mRemoteEndpoint,
                                boost::bind( &UdpSender::handleSentData, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred ) );
      }
    }
  }
}

void UdpSender::handleSentData( const boost::system::error_code& error,
                                std::size_t numBytesTransferred )
{
  if( error != boost::system::errc::success )
  {
    throw std::runtime_error( "UdpSender: Asynchronous send operation resulted in an error." );
  }
  boost::lock_guard<boost::mutex> lock( mMutex );
  assert( not mInternalMessageBuffer->empty() );
  std::string const & currMsg = mInternalMessageBuffer->nextElement();
  if( currMsg.size() != numBytesTransferred )
  {
    throw std::runtime_error( "UdpSender: Asynchronous send operation transmitted less bytes than the message length." );
  }
  mInternalMessageBuffer->popNextElement();
  if( not mInternalMessageBuffer->empty() )
  {
    std::string const & nextMsg = mInternalMessageBuffer->nextElement();
    mSocket->async_send_to( boost::asio::buffer( nextMsg.c_str( ), nextMsg.size( ) ),
                            mRemoteEndpoint,
                            boost::bind( &UdpSender::handleSentData, this,
                                         boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred ) );
  }
}

} // namespace rcl
} // namespace visr
