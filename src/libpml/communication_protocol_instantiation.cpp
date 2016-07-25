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
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::MatrixParameter<float> > >( ril::CommunicationProtocolType::MessageQueue, ril::ParameterType::MatrixFloat );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::MatrixParameter<double> > >( ril::CommunicationProtocolType::MessageQueue, ril::ParameterType::MatrixDouble );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::MessageQueueProtocol< pml::StringParameter > >( ril::CommunicationProtocolType::MessageQueue, ril::ParameterType::String );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::MatrixParameter<float> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::MatrixFloat );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::MatrixParameter<double> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::MatrixDouble );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::ObjectVector> >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::ObjectVector );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<float> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::VectorFloat );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::SharedDataProtocol< pml::VectorParameter<double> > >( ril::CommunicationProtocolType::SharedData, ril::ParameterType::VectorDouble );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::MatrixParameter<float> > >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::MatrixFloat );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::MatrixParameter<double> > >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::MatrixDouble );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::ObjectVector> >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::ObjectVector );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::StringParameter> >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::String );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::VectorParameter<float> > >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::VectorFloat );
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< pml::DoubleBufferingProtocol< pml::VectorParameter<double> > >( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::VectorDouble );
  }
} cInstantiateCommunicationProtocolCreators;

} // namespace pml
} // namespace visr
