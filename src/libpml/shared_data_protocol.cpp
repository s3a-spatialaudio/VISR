/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "shared_data_protocol.hpp"

#include <libril/parameter_factory.hpp>

#include <algorithm>

namespace visr
{
namespace pml
{

SharedDataProtocol::SharedDataProtocol( ParameterType const & parameterType,
                                        ParameterConfigBase const & parameterConfig )
  : CommunicationProtocolBase()
  , mParameterType( parameterType )
  , mParameterConfig( parameterConfig.clone() )
  , mData( ParameterFactory::create( parameterType, parameterConfig ) )
{
}

ParameterType SharedDataProtocol::parameterType() const
{
  return mParameterType;
}

CommunicationProtocolType SharedDataProtocol::protocolType() const
{
  return staticType();
}

void SharedDataProtocol::connectInput( CommunicationProtocolBase::Input* port )
{
  SharedDataProtocol::InputBase * typedPort = dynamic_cast<SharedDataProtocol::InputBase *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "SharedDataProtocol::connectInput(): port argument has wrong type." );
  }
  if( std::find( mInputs.begin(), mInputs.end(), typedPort ) == mInputs.end() )
  {
    mInputs.push_back( typedPort );
    typedPort->setProtocolInstance( this );
  }
}

void SharedDataProtocol::connectOutput( CommunicationProtocolBase::Output* port )
{
  OutputBase * typedPort = dynamic_cast<OutputBase *>(port);
  if( not typedPort )
  {
    throw std::invalid_argument( "SharedDataProtocol::connectOutput(): port argument has wrong type." );
  }
  if( mOutput )
  {
    throw std::invalid_argument( "SharedDataProtocol::connectOutput(): output port already set." );
  }
  mOutput = typedPort;
  typedPort->setProtocolInstance( this );
}

bool SharedDataProtocol::disconnectInput( CommunicationProtocolBase::Input* port ) noexcept
{
  InputBase * typedPort = dynamic_cast<InputBase *>(port);
  if( not typedPort )
  {
    return false;
  }
  std::vector<InputBase*>::iterator findIt = std::find( mInputs.begin(), mInputs.end(), typedPort );
  if( findIt == mInputs.end() )
  {
    return false;
  }
  (*findIt)->setProtocolInstance( static_cast<SharedDataProtocol*>(nullptr) );
  mInputs.erase( findIt );
  return true;
}

bool SharedDataProtocol::disconnectOutput( CommunicationProtocolBase::Output* port ) noexcept
{
  OutputBase * typedPort = dynamic_cast<OutputBase *>(port);
  if( not typedPort )
  {
    return false;
  }
  if( typedPort != mOutput )
  {
    // Trying to disconnect a port that is not the previously connected output.
    return false;
  }
  mOutput->setProtocolInstance( static_cast<SharedDataProtocol*>(nullptr) );
  mOutput = nullptr;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// InputBase

SharedDataProtocol::InputBase::~InputBase() = default;

void SharedDataProtocol::InputBase::setProtocolInstance( CommunicationProtocolBase * protocol )
{
  SharedDataProtocol * dbp = dynamic_cast<SharedDataProtocol*>(protocol);
  if( not dbp )
  {
    throw std::invalid_argument( " SharedDataProtocol::InputBase::setProtocolInstance(): Called with nonmatching protocol. " );
  }
  setProtocolInstance( dbp );
}


///////////////////////////////////////////////////////////////////////////////
// OutputBase

SharedDataProtocol::OutputBase::~OutputBase() = default;

void SharedDataProtocol::OutputBase::setProtocolInstance( CommunicationProtocolBase * protocol )
{
  SharedDataProtocol * dbp = dynamic_cast<SharedDataProtocol*>(protocol);
  if( not dbp )
  {
    throw std::invalid_argument( " SharedDataProtocol::InputBase::setProtocolInstance(): Called with nonmatching protocol. " );
  }
  setProtocolInstance( dbp );
}

} // namespace pml
} // namespace visr

