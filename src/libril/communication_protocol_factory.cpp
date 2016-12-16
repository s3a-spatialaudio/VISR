/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "communication_protocol_factory.hpp"

#include "communication_protocol_type.hpp"
#include "parameter_type.hpp"

// evil hack: Dependency to libpml
// TODO: add first concrete communication protocol class.
#include <libpml/double_buffering_protocol.hpp>
#include <libpml/message_queue_protocol.hpp>
#include <libpml/shared_data_protocol.hpp>

#include <libpml/matrix_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/string_parameter.hpp>
#include <libpml/time_frequency_parameter.hpp>
#include <libpml/vector_parameter.hpp>

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
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::MatrixParameter<float> > >( ril::CommunicationProtocolType::MessageQueue, ril::ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::MatrixParameter<double> > >( ril::CommunicationProtocolType::MessageQueue, ril::ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::StringParameter > >( ril::CommunicationProtocolType::MessageQueue, ril::ParameterType::String );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::MatrixParameter<float> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::MatrixParameter<double> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::ObjectVector> >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::ObjectVector );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::StringParameter> >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::String );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::TimeFrequencyParameter<float> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::TimeFrequencyFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::TimeFrequencyParameter<double> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::TimeFrequencyDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<double> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::VectorDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<float> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::VectorFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<double> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::VectorDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::MatrixParameter<float> > >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::MatrixParameter<double> > >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::ObjectVector> >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::ObjectVector );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::StringParameter> >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::String );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::VectorParameter<float> > >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::VectorFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::VectorParameter<double> > >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::VectorDouble );
  }
} // ; InstantiateCommunicationProtocolCreators const
cInstantiateCommunicationProtocolCreators;

} // namespace ril
} // namespace visr
