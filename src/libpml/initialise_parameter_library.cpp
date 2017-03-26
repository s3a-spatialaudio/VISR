/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "initialise_parameter_library.hpp"

#include "double_buffering_protocol.hpp"
#include "message_queue_protocol.hpp"
#include "shared_data_protocol.hpp"

#include "biquad_parameter.hpp"
#include "indexed_value_parameter.hpp"
#include "matrix_parameter.hpp"
#include "listener_position.hpp"
#include "object_vector.hpp"
#include "string_parameter.hpp"
#include "signal_routing_parameter.hpp"
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
  
  //CommunicationProtocolFactory::registerCommunicationProtocol< DoubleBufferingProtocol >( DoubleBufferingProtocol::staticType(), DoubleBufferingProtocol::staticName() );
  //CommunicationProtocolFactory::registerCommunicationProtocol< MessageQueueProtocol >( MessageQueueProtocol::staticType(), DoubleBufferingProtocol::staticName() );
  //CommunicationProtocolFactory::registerCommunicationProtocol< SharedDataProtocol >( SharedDataProtocol::staticType(), DoubleBufferingProtocol::staticName() );

  CommunicationProtocolFactory::registerCommunicationProtocol< DoubleBufferingProtocol >();
  CommunicationProtocolFactory::registerCommunicationProtocol< MessageQueueProtocol >();
  CommunicationProtocolFactory::registerCommunicationProtocol< SharedDataProtocol >();

  // Register all supported parameter types.
  ParameterFactory::registerParameterType< BiquadParameterMatrix<float> >( ParameterType::BiquadMatrixFloat );
  ParameterFactory::registerParameterType< BiquadParameterMatrix<double> >( ParameterType::BiquadMatrixDouble );

  ParameterFactory::registerParameterType< IndexedVectorDoubleType >( ParameterType::IndexedVectorDouble );
  ParameterFactory::registerParameterType< IndexedVectorFloatType >( ParameterType::IndexedVectorFloat );
  ParameterFactory::registerParameterType< IndexedStringType >( ParameterType::IndexedString );

  ParameterFactory::registerParameterType< ListenerPosition >( ParameterType::ListenerPosition );

  ParameterFactory::registerParameterType< MatrixParameter<float> >( ParameterType::MatrixFloat );
  ParameterFactory::registerParameterType< MatrixParameter<double> >( ParameterType::MatrixDouble );
  ParameterFactory::registerParameterType< MatrixParameter<std::complex<float> > >( ParameterType::MatrixFloatComplex );
  ParameterFactory::registerParameterType< MatrixParameter<std::complex<double> > >( ParameterType::MatrixDoubleComplex );

  ParameterFactory::registerParameterType< ObjectVector >( ParameterType::ObjectVector );

  ParameterFactory::registerParameterType< ListenerPosition >( ParameterType::ListenerPosition );


  ParameterFactory::registerParameterType< SignalRoutingParameter >( ParameterType::SignalRouting );


  ParameterFactory::registerParameterType< StringParameter >( ParameterType::String );

  ParameterFactory::registerParameterType< TimeFrequencyParameter<float> >( ParameterType::TimeFrequencyFloat );
  ParameterFactory::registerParameterType< TimeFrequencyParameter<double> >( ParameterType::TimeFrequencyDouble );

  ParameterFactory::registerParameterType< VectorParameter<float> >( ParameterType::VectorFloat );
  ParameterFactory::registerParameterType< VectorParameter<double> >( ParameterType::VectorDouble );
}

} // namespace pml
} // namespace visr
