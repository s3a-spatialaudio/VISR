/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "initialise_parameter_library.hpp"

#include "double_buffering_protocol.hpp"
#include "message_queue_protocol.hpp"
#include "shared_data_protocol.hpp"

#include "matrix_parameter.hpp"
#include "object_vector.hpp"
#include "string_parameter.hpp"
#include "time_frequency_parameter.hpp"
#include "vector_parameter.hpp"

#include <libril/parameter_factory.hpp>
#include <libril/communication_protocol_factory.hpp>

namespace visr
{
namespace pml
{

void initialiseParameterLibrary()
{
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
}

} // namespace pml
} // namespace visr
