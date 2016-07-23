/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "double_buffering_protocol.hpp"

#include <libril/communication_protocol_factory.hpp>

#include "listener_position.hpp"
#include "matrix_parameter.hpp"
#include "string_parameter.hpp"

#include <string>

namespace visr
{
namespace pml
{

// explicit instantiation
template class DoubleBufferingProtocol<ListenerPosition>;
template class DoubleBufferingProtocol<StringParameter>;
template class DoubleBufferingProtocol<MatrixParameter<float> >;
template class DoubleBufferingProtocol<MatrixParameter<double> >;

template< template<class> class ProtocolTypeT, class ParameterTypeT >
struct RegisterProtocolHelper
{
  RegisterProtocolHelper( ril::CommunicationProtocolType protocolType, ril::ParameterType parameterType )
  {
    ril::CommunicationProtocolFactory::registerCommunicationProtocolType< ProtocolTypeT< ParameterTypeT > >( protocolType, parameterType );
  }
};

RegisterProtocolHelper< DoubleBufferingProtocol, ListenerPosition > doubleBufferListeningPosition( ril::CommunicationProtocolType::DoubleBuffering, ril::ParameterType::ListenerPosition );

} // namespace pml
} // namespace visr
