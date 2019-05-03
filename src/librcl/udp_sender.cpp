/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "udp_sender.hpp"

#include <libpml/empty_parameter_config.hpp>

#include <boost/asio/placeholders.hpp>
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

  UdpSender::UdpSender( SignalFlowContext const & context,
                        char const * name,
                        CompositeComponent * parent,
                        std::size_t sendPort,
                        std::string const & receiverAddress,
                        std::size_t receiverPort,
                        UdpSender::Mode mode,
                        boost::asio::io_service* externalIoService /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mMode( Mode::Asynchronous)
 , mMessageInput( "messageInput", *this, pml::EmptyParameterConfig() )
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

  udp::resolver resolver( *mIoService );
  udp::resolver::query query( udp::v4( ),
                              receiverAddress,
                              std::to_string(receiverPort),
                              udp::resolver::query::flags::passive ); /* Override the default values for the flag parameter which includes "address_configured"
  that requires a network conection apart from loopback device. */
  mRemoteEndpoint = *resolver.resolve( query );
  // Debug output:
  // std::cout << "Remote endpoint: " << mRemoteEndpoint.address().to_string() << ":" << mRemoteEndpoint.port() << std::endl;
  if( mRemoteEndpoint.port() != static_cast<unsigned short>(receiverPort) )
  {
    throw std::logic_error("");
  }

  udp::endpoint localEndpoint( udp::v4( ), static_cast<unsigned short>(sendPort) );
  mSocket.reset( new udp::socket( *mIoService, localEndpoint) );

  if( mMode == Mode::Asynchronous )
  {
    mServiceThread.reset( new boost::thread( boost::bind( &boost::asio::io_service::run, mIoService ) ));
  }
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
      std::size_t bytesSent = mSocket->send_to( boost::asio::buffer(nextMsg.str(), nextMsg.size()), mRemoteEndpoint );
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
      bool const transmissionPending = not mInternalMessageBuffer.empty();
      while( not mMessageInput.empty() )
      {
        char const * nextMsg = mMessageInput.front().str();
        mInternalMessageBuffer.push_back( pml::StringParameter( nextMsg ) );
        mMessageInput.pop();
      }
      if( not mInternalMessageBuffer.empty() and not transmissionPending )
      {
        pml::StringParameter const & nextMsg = mInternalMessageBuffer.front( );
        mSocket->async_send_to( boost::asio::buffer( nextMsg.str(), nextMsg.size( ) ),
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
  assert( not mInternalMessageBuffer.empty() );
  pml::StringParameter & currMsg = mInternalMessageBuffer.front();
  if( currMsg.size() != numBytesTransferred )
  {
    throw std::runtime_error( "UdpSender: Asynchronous send operation transmitted less bytes than the message length." );
  }
  mInternalMessageBuffer.pop_front();
  if( not mInternalMessageBuffer.empty() )
  {
    pml::StringParameter const & nextMsg = mInternalMessageBuffer.front();
    mSocket->async_send_to( boost::asio::buffer( nextMsg.str( ), nextMsg.size( ) ),
                            mRemoteEndpoint,
                            boost::bind( &UdpSender::handleSentData, this,
                                         boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred ) );
  }
}

} // namespace rcl
} // namespace visr