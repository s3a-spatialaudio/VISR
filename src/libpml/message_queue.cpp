/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "message_queue.hpp"

#include "matrix_parameter.hpp"
#include "string_parameter.hpp"

#include <string>

namespace visr
{
namespace pml
{

// explicit instantiation
template class MessageQueue<StringParameter>;
template class MessageQueue<MatrixParameter<float> >;
template class MessageQueue<MatrixParameter<double> >;

} // namespace pml
} // namespace visr
