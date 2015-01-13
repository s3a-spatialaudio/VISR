/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "udp_receiver.hpp"

#include <libpml/message_queue.hpp>

#include <boost/asio/placeholders.hpp>
//#include <boost/asio.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/lock_types.hpp>

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
  if( mIoService.get() != nullptr )
  {
    mIoService->stop();
  }
  if( mServiceThread.get() != nullptr  )
  {
    mServiceThread->join();
  }
}

void UdpReceiver::setup( std::size_t port, Mode mode )
{
  using boost::asio::ip::udp;

  mMode = mode;
  mIoService.reset( new boost::asio::io_service() );
  if(  mMode == Mode::Synchronous )
  {
    mIoServiceWork.reset();
  }
  else
  {
    mIoServiceWork.reset( new  boost::asio::io_service::work( *mIoService) );
  }
  mInternalMessageBuffer.reset( new pml::MessageQueue< std::string >() ) ;
  mSocket.reset( new udp::socket( *mIoService,  udp::endpoint(udp::v4(), port )) );

  mSocket->async_receive_from( boost::asio::buffer(mReceiveBuffer),
                               mRemoteEndpoint,
                               boost::bind(&UdpReceiver::handleReceiveData, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred)
                             );

  if(  mMode == Mode::Asynchronous )
  {
    mServiceThread.reset( new boost::thread( boost::bind( &boost::asio::io_service::run, mIoService.get() ) ));
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
