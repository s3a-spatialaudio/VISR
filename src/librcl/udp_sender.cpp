/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "udp_sender.hpp"

#include <libpml/message_queue.hpp>

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

UdpSender::UdpSender( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mMode( Mode::Asynchronous)
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

void UdpSender::setup(std::string const & receiverAddress, std::size_t receiverPort,
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
  mInternalMessageBuffer.reset( new pml::MessageQueue< std::string >( ) );

  udp::resolver resolver( *mIoService );
  udp::resolver::query query( udp::v4( ), receiverAddress, std::to_string(receiverPort) );
  udp::endpoint resolvedEndpoint = *resolver.resolve( query );

  mSocket.reset( new udp::socket( *mIoService, resolvedEndpoint ) );

  //mSocket->async_send_to( boost::asio::buffer(mReceiveBuffer),
  //                             mRemoteEndpoint,
  //                             boost::bind(&UdpSender::handleSentData, this,
  //                                          boost::asio::placeholders::error,
  //                                          boost::asio::placeholders::bytes_transferred)
                             //);

  if(  mMode == Mode::Asynchronous )
  {
    mServiceThread.reset( new boost::thread( boost::bind( &boost::asio::io_service::run, mIoService ) ));
  }
}

void UdpSender::process( pml::MessageQueue<std::string> & msgQueue )
{
  switch( mMode )
  {
  case Mode::Synchronous:
    // we don't operate an internal message queue in this mode.
    while( !msgQueue.empty( ) )
    {
      std::string const & nextMsg = msgQueue.nextElement( );
      // boost::system::error_code err;
      mSocket->send( boost::asio::buffer(nextMsg.c_str(), nextMsg.size()) );
      msgQueue.popNextElement( );
    }
    break;
  case Mode::Asynchronous:
  case Mode::ExternalServiceObject:
    {
      boost::lock_guard<boost::mutex> lock( mMutex );
      bool const inTransmission = mInternalMessageBuffer->empty();
      while( !msgQueue.empty() )
      {
        std::string const & nextMsg = msgQueue.nextElement();
        mInternalMessageBuffer->enqueue( nextMsg );
        msgQueue.popNextElement();
      }
      if( not mInternalMessageBuffer->empty() and not inTransmission )
      {
        std::string const & nextMsg = mInternalMessageBuffer->nextElement( );
        mSocket->async_send( boost::asio::buffer( nextMsg.c_str(), nextMsg.size() ),
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
  boost::lock_guard<boost::mutex> lock( mMutex );
  assert( not mInternalMessageBuffer->empty() );
  mInternalMessageBuffer->popNextElement();
  if( not mInternalMessageBuffer->empty() )
  {
    std::string const & nextMsg = mInternalMessageBuffer->nextElement();
    mSocket->async_send( boost::asio::buffer( nextMsg.c_str( ), nextMsg.size( ) ),
                         boost::bind( &UdpSender::handleSentData, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred ) );
  }
}

} // namespace rcl
} // namespace visr
