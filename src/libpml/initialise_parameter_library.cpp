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
  
  CommunicationProtocolFactory::registerCommunicationProtocol< DoubleBufferingProtocol >( CommunicationProtocolType( "DoubleBuffering" ) );
}

} // namespace pml
} // namespace visr
