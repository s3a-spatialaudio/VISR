/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "udp_receiver.hpp"

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

  UdpReceiver::UdpReceiver( ril::SignalFlowContext& context,
                            char const * name,
                            ril::CompositeComponent * parent /*= nullptr*/ )
 : AtomicComponent( context, name, parent )
 , mMode( Mode::Asynchronous)
 , mDatagramOutput( "messageOutput", *this, pml::StringParameterConfig(255) )
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

void UdpReceiver::process()
{
  if(  mMode == Mode::Synchronous )
  {
    mIoService->poll();
  }
  boost::lock_guard<boost::mutex> lock( mMutex );
  while( not mInternalMessageBuffer.empty() )
  {
    pml::StringParameter const & nextMsg = mInternalMessageBuffer.front();
    mDatagramOutput.enqueue( nextMsg  );
    mInternalMessageBuffer.pop_front();
  }
}

void UdpReceiver::handleReceiveData( const boost::system::error_code& error,
                                     std::size_t numBytesTransferred )
{
  {
    boost::lock_guard<boost::mutex> lock( mMutex );
    mInternalMessageBuffer.push_back( pml::StringParameter( std::string( &mReceiveBuffer[0], numBytesTransferred ) ) );
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
