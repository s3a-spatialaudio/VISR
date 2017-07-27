/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "double_buffering_protocol.hpp"

#include <libril/parameter_factory.hpp>

#include <algorithm>

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

DoubleBufferingProtocol::~DoubleBufferingProtocol() = default;

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
  std::for_each( mInputs.begin(), mInputs.end(), []( InputBase* port ) { port->markChanged(); } );
}


void DoubleBufferingProtocol::connectInput( CommunicationProtocolBase::Input* port )
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

void DoubleBufferingProtocol::connectOutput( CommunicationProtocolBase::Output* port )
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

bool DoubleBufferingProtocol::disconnectInput( CommunicationProtocolBase::Input* port ) noexcept
{
  InputBase * typedPort = dynamic_cast<DoubleBufferingProtocol::InputBase *>(port);
  if( not typedPort )
  {
    return false;
  }
  std::vector<InputBase*>::iterator findIt = std::find( mInputs.begin(), mInputs.end(), typedPort );
  if( findIt == mInputs.end() )
  {
    return false;
  }
  // It does not matter which overload is called, but the ambiguity has to be resolved.
  (*findIt)->setProtocolInstance( static_cast<DoubleBufferingProtocol*>(nullptr) );
  mInputs.erase( findIt );
  return true;
}

bool DoubleBufferingProtocol::disconnectOutput( CommunicationProtocolBase::Output* port ) noexcept
{
  DoubleBufferingProtocol::OutputBase * typedPort = dynamic_cast<DoubleBufferingProtocol::OutputBase *>(port);
  if( not typedPort )
  {
    return false;
  }
  if( typedPort != mOutput )
  {
    // Trying to disconnect a port that is not the previously connected output
    return false;
  }
  mOutput->setProtocolInstance( static_cast<DoubleBufferingProtocol*>(nullptr) );
  mOutput = nullptr;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// InputBase
/**
 * Suppress Doxygen warnings about "no uniquely matching class member"
 * These warnings are apparently triggered by the VISR_PML_LIBRARY_SYMBOL macro in the class definition.
 * Anyway, the functions are properly documented in the .hpp file.
 * @cond NEVER
 */
DoubleBufferingProtocol::InputBase::InputBase()
  : mProtocol( nullptr )
  , mChanged( false ) // Do not signal changed values on startup (unless they are explicitly set)
{
}

DoubleBufferingProtocol::InputBase::~InputBase() = default;

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

void DoubleBufferingProtocol::InputBase::setProtocolInstance( CommunicationProtocolBase * protocol )
{
  DoubleBufferingProtocol * dbProtocol = dynamic_cast<DoubleBufferingProtocol*>( protocol );
  if( not dbProtocol )
  {
    throw std::invalid_argument( "DoubleBufferingProtocol::InputBase::setProtocolInstance(): Nonmatching protocol type." );
  }
  setProtocolInstance( dbProtocol );
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

DoubleBufferingProtocol::OutputBase::~OutputBase() = default;

ParameterBase & DoubleBufferingProtocol::OutputBase::data()
{
  return mProtocol->frontData();
}

void DoubleBufferingProtocol::OutputBase::swapBuffers()
{
  mProtocol->swapBuffers();
}

void DoubleBufferingProtocol::OutputBase::setProtocolInstance( CommunicationProtocolBase * protocol )
{
  DoubleBufferingProtocol * mp = dynamic_cast<DoubleBufferingProtocol*>(protocol);
  if( not mp )
  {
    throw std::invalid_argument( "MessageQueueProtocol::InputBase::setProtocolInstance(): Called with nonmatching protocol. " );
  }
  setProtocolInstance( mp );
}

void DoubleBufferingProtocol::OutputBase::
setProtocolInstance( DoubleBufferingProtocol * protocol )
{
  mProtocol = protocol;
}
/// @endcond NEVER

} // namespace pml
} // namespace visr
