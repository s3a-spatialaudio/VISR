/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/communication_protocol_factory.hpp>

#include <libril/communication_protocol_type.hpp>
#include <libril/parameter_type.hpp>

#include <ciso646>
#include <stdexcept>

namespace visr
{

void CommunicationProtocolFactory::registerCommunicationProtocol( CommunicationProtocolType type, Creator && creator )
{
  CreatorTable & table = creatorTable();
  if( table.find( type) != table.cend() )
  {
    throw std::invalid_argument( "CommunicationProtocolFactory: Entry for protocol type already exists." );
  }
  auto const res = table.insert( std::make_pair( type, creator ) );
  if( not res.second)
  {
    throw std::invalid_argument( "CommunicationProtocolFactory: Error while registering protocol type." );

  }
}

CommunicationProtocolFactory::Creator::Creator( CreateFunction fcn, char const * name )
 : mCreateFunction( fcn )
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
  return std::unique_ptr< CommunicationProtocolBase >( mCreateFunction( paramType, config ) );
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

} // namespace visr
