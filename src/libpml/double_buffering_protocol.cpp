/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "double_buffering_protocol.hpp"

#include <libril/parameter_factory.hpp>

// #include <string>

namespace visr
{
namespace pml
{

DoubleBufferingProtocol::DoubleBufferingProtocol( ParameterType const & parameterType,
                                                  ParameterConfigBase const & parameterConfig )
 : CommunicationProtocolBase()
 , mParameterType( parameterType )
 , mConfig( parameterConfig.clone() )
 , mOutput( nullptr )
 , mBackData( ParameterFactory::create( parameterType, parameterConfig) )
 , mFrontData( ParameterFactory::create( parameterType, parameterConfig ) )
{
}

ParameterType DoubleBufferingProtocol::parameterType() const
{
  return mParameterType;
}

ParameterBase & DoubleBufferingProtocol::frontData()
{
  return *mFrontData;
}

ParameterBase const & DoubleBufferingProtocol::frontData() const
{
  return *mFrontData;
}

ParameterBase & DoubleBufferingProtocol::backData()
{
  return *mBackData;
}

ParameterBase const & DoubleBufferingProtocol::backData() const
{
  return *mBackData;
}

void DoubleBufferingProtocol::setData( ParameterBase const & newData )
{
  *mBackData = newData;
}

void DoubleBufferingProtocol::swapBuffers()
{
  mBackData.swap( mFrontData );
  //std::for_each( mInputs.begin(), mInputs.end(), []( InputBase* port ) { port->markChanged(); } );
}


void DoubleBufferingProtocol::connectInput( ParameterPortBase* port )
{
  DoubleBufferingProtocol::InputBase * typedPort = dynamic_cast<DoubleBufferingProtocol::InputBase *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::connectInput(): port argument has wrong type." );
  }
  if( std::find( mInputs.begin(), mInputs.end(), typedPort ) == mInputs.end() )
  {
    mInputs.push_back( typedPort );
  }
  typedPort->setProtocolInstance( this );
}

void DoubleBufferingProtocol::connectOutput( ParameterPortBase* port )
{
  DoubleBufferingProtocol::OutputBase * typedPort = dynamic_cast<DoubleBufferingProtocol::OutputBase*>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::connectOutput(): port argument has wrong type." );
  }
  if( mOutput )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::connectOutput(): output port already set." );
  }
  mOutput = typedPort;
  typedPort->setProtocolInstance( this );
}

bool DoubleBufferingProtocol::disconnectInput( ParameterPortBase* port )
{
  InputBase * typedPort = dynamic_cast<DoubleBufferingProtocol::InputBase *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::connectInput(): port argument has wrong type." );
  }
  // TODO: Check parameter type (should have been checked before)
  // TODO: Re-check parameter configuration
  typename std::vector<InputBase*>::iterator findIt = std::find( mInputs.begin(), mInputs.end(), typedPort );
  if( findIt == mInputs.end() )
  {
    return false;
  }
  (*findIt)->setProtocolInstance( nullptr );
  mInputs.erase( findIt );
  return true;
}

bool DoubleBufferingProtocol::disconnectOutput( ParameterPortBase* port )
{
  DoubleBufferingProtocol::OutputBase * typedPort = dynamic_cast<DoubleBufferingProtocol::OutputBase *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::disconnectOutput(): port argument has wrong type." );
  }
  if( typedPort != mOutput )
  {
    // Trying to disconnect a port that is not the previously connected output
    return false;
  }
  mOutput->setProtocolInstance( this );
  mOutput = nullptr;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// InputBase

DoubleBufferingProtocol::InputBase::InputBase()
  : mProtocol( nullptr )
  , mChanged( true ) // Mark the data as changed for the first iteration
{
}

ParameterBase const & DoubleBufferingProtocol::InputBase::data() const
{
  return mProtocol->backData();
}

bool DoubleBufferingProtocol::InputBase::changed() const
{
  return mChanged;
}

void DoubleBufferingProtocol::InputBase::resetChanged()
{
  mChanged = false;
}

void DoubleBufferingProtocol::InputBase::
setProtocolInstance( DoubleBufferingProtocol * protocol )
{
  mProtocol = protocol;
}


///////////////////////////////////////////////////////////////////////////////
// OutputBase

DoubleBufferingProtocol::OutputBase::OutputBase()
  : mProtocol( nullptr )
{
}

ParameterBase & DoubleBufferingProtocol::OutputBase::data()
{
  return mProtocol->frontData();
}

void DoubleBufferingProtocol::OutputBase::swapBuffers()
{
  mProtocol->swapBuffers();
}

void DoubleBufferingProtocol::OutputBase::
setProtocolInstance( DoubleBufferingProtocol * protocol )
{
  mProtocol = protocol;
}

} // namespace pml
} // namespace visr
