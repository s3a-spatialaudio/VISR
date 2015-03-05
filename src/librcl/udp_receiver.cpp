/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "udp_receiver.hpp"

#include <libpml/message_queue.hpp>

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

UdpReceiver::UdpReceiver( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mMode( Mode::Asynchronous)
{
}

UdpReceiver::~UdpReceiver()
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

void UdpReceiver::setup( std::size_t port, Mode mode, boost::asio::io_service* externalIoService /*= nullptr*/ )
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
    mIoService = mIoServiceInstance.get();
  }

  if( (mMode == Mode::Synchronous) or (mMode == Mode::ExternalServiceObject) )
  {
    mIoServiceWork.reset();
  }
  else
  {
    mIoServiceWork.reset( new  boost::asio::io_service::work( *mIoService) );
  }
  mInternalMessageBuffer.reset( new pml::MessageQueue< std::string >() ) ;
  mSocket.reset( new udp::socket( *mIoService,  udp::endpoint(udp::v4(), static_cast<unsigned short>(port) )) );

  mSocket->async_receive_from( boost::asio::buffer(mReceiveBuffer),
                               mRemoteEndpoint,
                               boost::bind(&UdpReceiver::handleReceiveData, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred)
                             );
  if(  mMode == Mode::Asynchronous )
  {
    mServiceThread.reset( new boost::thread( boost::bind( &boost::asio::io_service::run, mIoService ) ));
  }
}

void UdpReceiver::process( pml::MessageQueue<std::string> & msgQueue )
{
  if(  mMode == Mode::Synchronous )
  {
    mIoService->poll();
  }
  boost::lock_guard<boost::mutex> lock( mMutex );
  while( !mInternalMessageBuffer->empty() )
  {
    std::string const & nextMsg = mInternalMessageBuffer->nextElement();
    msgQueue.enqueue( nextMsg );
    mInternalMessageBuffer->popNextElement();
  }
}

void UdpReceiver::handleReceiveData( const boost::system::error_code& error,
                                     std::size_t numBytesTransferred )
{
  {
    boost::lock_guard<boost::mutex> lock( mMutex );
    mInternalMessageBuffer->enqueue( std::string( &mReceiveBuffer[0], numBytesTransferred ) );
  }
  mSocket->async_receive_from( boost::asio::buffer(mReceiveBuffer),
                               mRemoteEndpoint,
                               boost::bind(&UdpReceiver::handleReceiveData, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred)
                             );
}

} // namespace rcl
} // namespace visr
