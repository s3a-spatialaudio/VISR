/* Copyright Institute of Sound and Vibration Research - All rights reserved */

/**
 * @file
 * Force instantiation of creators for the communication protocol factory.
 */

#include <libpml/double_buffering_protocol.hpp>
#include <libpml/message_queue_protocol.hpp>
#include <libpml/shared_data_protocol.hpp>

#include <libpml/matrix_parameter.hpp>
#include <libpml/object_vector.hpp>
#include <libpml/string_parameter.hpp>
#include <libpml/vector_parameter.hpp>

#include <libril/communication_protocol_factory.hpp>

namespace visr
{
namespace pml
{

/**
 * Object to force instantiation of creator objects in the factory.
 * @warning This code has no effect, an almost identical version in libril/communication_protocol_factory.cpp is effective.
 */
static struct InstantiateCommunicationProtocolCreators
{
  InstantiateCommunicationProtocolCreators( )
  {
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::MatrixParameter<float> > >( CommunicationProtocolType::MessageQueue, ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::MatrixParameter<double> > >( CommunicationProtocolType::MessageQueue, ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::StringParameter > >( CommunicationProtocolType::MessageQueue, ParameterType::String );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::MatrixParameter<float> > >( CommunicationProtocolType::SharedData, ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::MatrixParameter<double> > >( CommunicationProtocolType::SharedData, ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::ObjectVector> >( CommunicationProtocolType::SharedData, ParameterType::ObjectVector );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<float> > >( CommunicationProtocolType::SharedData, ParameterType::VectorFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<double> > >( CommunicationProtocolType::SharedData, ParameterType::VectorDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::MatrixParameter<float> > >( CommunicationProtocolType::DoubleBuffering, ParameterType::MatrixFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::MatrixParameter<double> > >( CommunicationProtocolType::DoubleBuffering, ParameterType::MatrixDouble );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::ObjectVector> >( CommunicationProtocolType::DoubleBuffering, ParameterType::ObjectVector );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::StringParameter> >( CommunicationProtocolType::DoubleBuffering, ParameterType::String );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::VectorParameter<float> > >( CommunicationProtocolType::DoubleBuffering, ParameterType::VectorFloat );
    CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::VectorParameter<double> > >( CommunicationProtocolType::DoubleBuffering, ParameterType::VectorDouble );
  }
} cInstantiateCommunicationProtocolCreators;

} // namespace pml
} // namespace visr
