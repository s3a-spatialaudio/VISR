/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "message_queue_protocol.hpp"

#include "matrix_parameter.hpp"
#include "string_parameter.hpp"

#include <string>

namespace visr
{
namespace pml
{

// explicit instantiation
template class MessageQueueProtocol<StringParameter>;
template class MessageQueueProtocol<MatrixParameter<float> >;

} // namespace pml
} // namespace visr
