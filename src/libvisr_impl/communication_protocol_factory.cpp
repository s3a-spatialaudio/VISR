/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libril/communication_protocol_factory.hpp>

#include <libril/communication_protocol_type.hpp>
#include <libril/parameter_type.hpp>

#if 0
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
#endif
#include <stdexcept>

namespace visr
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
#if 0
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::MatrixParameter<float> > >( CommunicationProtocolType::MessageQueue, ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::MatrixParameter<double> > >( CommunicationProtocolType::MessageQueue, ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::StringParameter > >( CommunicationProtocolType::MessageQueue, ParameterType::String );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::MatrixParameter<float> > >( CommunicationProtocolType::SharedData, ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::MatrixParameter<double> > >( CommunicationProtocolType::SharedData, ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::ObjectVector> >( CommunicationProtocolType::SharedData, ParameterType::ObjectVector );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::StringParameter> >( CommunicationProtocolType::SharedData, ParameterType::String );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::TimeFrequencyParameter<float> > >( CommunicationProtocolType::SharedData, ParameterType::TimeFrequencyFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::TimeFrequencyParameter<double> > >( CommunicationProtocolType::SharedData, ParameterType::TimeFrequencyDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<double> > >( CommunicationProtocolType::SharedData, ParameterType::VectorDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<float> > >( CommunicationProtocolType::SharedData, ParameterType::VectorFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<double> > >( CommunicationProtocolType::SharedData, ParameterType::VectorDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::MatrixParameter<float> > >( CommunicationProtocolType::DoubleBuffering, ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::MatrixParameter<double> > >( CommunicationProtocolType::DoubleBuffering, ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::ObjectVector> >( CommunicationProtocolType::DoubleBuffering, ParameterType::ObjectVector );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::StringParameter> >( CommunicationProtocolType::DoubleBuffering, ParameterType::String );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::VectorParameter<float> > >( CommunicationProtocolType::DoubleBuffering, ParameterType::VectorFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::VectorParameter<double> > >( CommunicationProtocolType::DoubleBuffering, ParameterType::VectorDouble );
#endif
  }
} // ; InstantiateCommunicationProtocolCreators const
cInstantiateCommunicationProtocolCreators;

} // namespace visr
