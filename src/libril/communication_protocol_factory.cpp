/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "communication_protocol_factory.hpp"

#include "communication_protocol_type.hpp"
#include "parameter_type.hpp"

// evil hack: Dependency to libpml
// TODO: add first concrete communication protocol class.
#include <libpml/message_queue.hpp>

#include <libpml/string_parameter.hpp>
#include <libpml/matrix_parameter.hpp>

#include <stdexcept>

namespace visr
{
namespace ril
{

CommunicationProtocolFactory::Creator::Creator( CreateFunction fcn )
 : mCreateFunction( fcn )
{
}

std::unique_ptr<CommunicationProtocolBase >
CommunicationProtocolFactory::Creator::create( ParameterConfigBase const & config ) const
{
  return std::unique_ptr< CommunicationProtocolBase >( mCreateFunction( config ) );
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
    = creatorTable().find( std::make_pair( protocolType, parameterType ) );
  if( findIt == creatorTable().end() )
  {
    throw std::invalid_argument( "CommunicationProtocolFactory: No creator function for requested parameter type " );
  }
  // todo: Need to catch construction errors?
  return std::unique_ptr<CommunicationProtocolBase>( findIt->second.create( config ) );
}

static struct InstantiateCommunicationProtocolCreators
{
  // todo: Add instantiations
  InstantiateCommunicationProtocolCreators()
  {
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueue< pml::MatrixParameter<float> > >( ril::CommunicationProtocolType::MessageQueue, ril::ParameterType::MatrixFloat );
  }
} // ; InstantiateCommunicationProtocolCreators const
cInstantiateCommunicationProtocolCreators;
  
} // namespace ril
} // namespace visr
