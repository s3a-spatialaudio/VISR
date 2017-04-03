/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/communication_protocol_factory.hpp>

#include <libril/communication_protocol_type.hpp>
#include <libril/parameter_type.hpp>

#include <ciso646>
#include <stdexcept>
#include <string>

namespace visr
{

void CommunicationProtocolFactory::registerCommunicationProtocol( CommunicationProtocolType type, Creator && creator )
{
  CreatorTable & table = creatorTable();
  auto const findIt = table.find( type );
  
  if( findIt != table.cend() )
  {
    throw std::invalid_argument( std::string("CommunicationProtocolFactory: Entry for protocol type \"") + findIt->second.name() + "\" already exists." );
  }
  auto const res = table.insert( std::make_pair( type, creator ) );
  if( not res.second)
  {
    throw std::invalid_argument( "CommunicationProtocolFactory: Error while registering protocol type." );

  }
}

CommunicationProtocolFactory::Creator::Creator( CreateFunction fcn, 
                                                InputCreateFunction inputCreator,
                                                OutputCreateFunction outputCreator,
                                                char const * name )
 : mCreateFunction( fcn )
 , mInputCreateFunction( inputCreator )
 , mOutputCreateFunction( outputCreator )
 , mName( name )
{
}

std::string const & CommunicationProtocolFactory::Creator::name() const
{
  return mName;
}

std::unique_ptr<CommunicationProtocolBase >
CommunicationProtocolFactory::Creator::create( ParameterType const & paramType, ParameterConfigBase const & config ) const
{
  return mCreateFunction( paramType, config );
}

std::unique_ptr<CommunicationProtocolBase::Input> 
CommunicationProtocolFactory::Creator::createInput() const
{
  return mInputCreateFunction();
}

std::unique_ptr<CommunicationProtocolBase::Output> 
CommunicationProtocolFactory::Creator::createOutput() const
{
  return mOutputCreateFunction();
}


/*static*/ CommunicationProtocolFactory::CreatorTable &
CommunicationProtocolFactory::creatorTable()
{
  static CommunicationProtocolFactory::CreatorTable sCreatorTable;
  return sCreatorTable;
}

/*static*/ std::unique_ptr<CommunicationProtocolBase>
CommunicationProtocolFactory::create( CommunicationProtocolType const & protocolType,
                                      ParameterType const & parameterType,
                                      ParameterConfigBase const & config )
{
  CreatorTable::const_iterator findIt
    = creatorTable().find( protocolType );
  if( findIt == creatorTable().end() )
  {
    throw std::invalid_argument( "CommunicationProtocolFactory: No creator function for requested parameter type " );
  }
  // todo: Need to catch construction errors?
  return std::unique_ptr<CommunicationProtocolBase>( findIt->second.create( parameterType, config ) );
}

/*static*/ std::unique_ptr<CommunicationProtocolBase::Input> 
CommunicationProtocolFactory::createInput( CommunicationProtocolType const & protocolType )
{
  CreatorTable::const_iterator findIt
    = creatorTable().find( protocolType );
  if( findIt == creatorTable().end() )
  {
    throw std::invalid_argument( "CommunicationProtocolFactory: No creator function for requested parameter type " );
  }
  // todo: Need to catch construction errors?
  return std::unique_ptr<CommunicationProtocolBase::Input>( findIt->second.createInput() );
}

/*static*/ std::unique_ptr<CommunicationProtocolBase::Output> 
CommunicationProtocolFactory::createOutput( CommunicationProtocolType const & protocolType )
{
  CreatorTable::const_iterator findIt
    = creatorTable().find( protocolType );
  if( findIt == creatorTable().end() )
  {
    throw std::invalid_argument( "CommunicationProtocolFactory: No creator function for requested parameter type " );
  }
  // todo: Need to catch construction errors?
  return std::unique_ptr<CommunicationProtocolBase::Output>( findIt->second.createOutput() );
}


} // namespace visr
